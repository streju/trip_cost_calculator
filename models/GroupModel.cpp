#include "GroupModel.hpp"

#include <algorithm>

#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

std::set<QString> transformVariantListToSet(const QVariantList& list)
{
    std::set<QString> result;
    std::transform(list.begin(), list.end(), std::inserter(result, result.end()),
        [](const QVariant& variant){return variant.value<QString>();});
    return result;
}

GroupModel::GroupModel(WarningIndicatorPtr warningIndicator, const CurrentStateProviderPtr& currentStateProvider)
    : warningIndicator_(warningIndicator), currentStateProvider_(currentStateProvider)
{
    QObject::connect(currentStateProvider, &CurrentStateProvider::currentLoggedUserChange,
        this, &GroupModel::refreshQuery);
}

void GroupModel::refreshQuery()
{
    QSqlQuery query;
    query.prepare("SELECT group_name, user_1, user_2, SUM(CASE WHEN user_1=:username THEN value ELSE -value END) AS 'value'"
        "FROM groups_relations WHERE user_1=:username OR user_2=:username GROUP BY group_name");
    query.bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query.exec();
    setQuery(query);
    updateUserTotalBalance();
}

bool GroupModel::validateGroupName(const QString& groupName) const
{
    QSqlQuery query;
    query.exec("SELECT 1 FROM groups WHERE name ='" + groupName + "';");
    bool result = query.next();
    if (result)
    {
        warningIndicator_->setWarning("Already exist group with name: " + groupName, WarningType::GROUP_CREATION);
    }
    return result;
}

double GroupModel::getUserTotalBalance()
{
    return userTotalBalance_;
}

void GroupModel::updateUserTotalBalance()
{
    QSqlQuery query;
    query.prepare("SELECT SUM(CASE WHEN user_1=:username THEN value ELSE -value END)"
        " AS 'value' FROM groups_relations WHERE user_1=:username OR user_2=:username");
    query.bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query.exec();
    query.next();
    qDebug() << "TOTAL BALANCE " << userTotalBalance_;
    userTotalBalance_ = query.value("value").toDouble();
    emit userTotalBalanceChanged();
}

bool GroupModel::addGroup(const QString& groupName, QVariantList usersToAddToGroup)
{
    QSqlQuery("CREATE TABLE IF NOT EXISTS groups ("
        "group_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "currency TEXT NULL)");

    if (usersToAddToGroup.empty())
    {
        warningIndicator_->setWarning("Cannot create group without users.", WarningType::GROUP_CREATION);
        return false;
    }
    usersToAddToGroup.push_back(currentStateProvider_->getCurrentLoggedUser());
    if (groupName.isEmpty())
    {
        warningIndicator_->setWarning("Group has to have name.", WarningType::GROUP_CREATION);
        return false;
    }
    if (validateGroupName(groupName))
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO groups(name, currency) VALUES(:name, :currency);");
    query.bindValue(":name", groupName);
    query.bindValue(":currency", "PLN");
    if (!query.exec()) return false;
    if (!insertInitialActivity(groupName)) return false;
    if (!assignUsersToGroup(groupName, transformVariantListToSet(usersToAddToGroup))) return false;
    refreshModel();
    return true;
}

bool GroupModel::assignUsersToGroup(const QString& groupName, const std::set<QString>& groupUsers) const
{
    QSqlQuery("CREATE TABLE IF NOT EXISTS groups_relations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "group_name TEXT,"
        "user_1 TEXT,"
        "user_2 TEXT,"
        "value REAL)");

    auto usersToAddRelations = groupUsers;
    for (const auto& user_1 : groupUsers)
    {
        usersToAddRelations.erase(user_1);
        for (const auto& user_2 : usersToAddRelations)
        {
            QSqlQuery query;
            query.prepare("INSERT INTO groups_relations(group_name, user_1, user_2, value) "
                "VALUES(:group_name, :user_1, :user_2, :value);");
            query.bindValue(":group_name", groupName);
            query.bindValue(":user_1", user_1);
            query.bindValue(":user_2", user_2);
            query.bindValue(":value", 0);
            if (!query.exec())
            {
                qDebug() << "fail to insert users relations for group: " <<
                    groupName << ", because: " << query.lastError().text();
                return false;
            }
        }
    }
    return true;
}

void GroupModel::refreshModel()
{
    clear();
    query().clear();
    refreshQuery();
    updateUserTotalBalance();
}

void GroupModel::deleteUser(const QString &user)
{
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE user_1=:login OR user_2=:login");
    query.bindValue(":login", user);
    query.exec();
    refreshModel();
}

void GroupModel::deleteGroup()
{
    const auto groupname = currentStateProvider_->getCurrentGroup();

    QSqlQuery query;
    query.prepare("DELETE FROM groups WHERE name=:groupname");
    query.bindValue(":groupname", groupname);
    query.exec();

    query.clear();
    query.prepare("DELETE FROM groups_relations WHERE group_name=:groupname");
    query.bindValue(":groupname", groupname);
    if (!query.exec())
    {
        qDebug() << "fail to delete users relations for group: " << groupname << ", because: " << query.lastError().text();
        warningIndicator_->setWarning("Communication problem with database.", WarningType::GROUP_OPERATIONS);
        return;
    }

    const auto gr_activities = groupname + "_activities";
    query.clear();
    query.prepare("DROP TABLE " + gr_activities);
    if (!query.exec())
    {
        qDebug() << "fail to delete " << gr_activities << ", because: " << query.lastError().text();
        warningIndicator_->setWarning("Communication problem with database.", WarningType::GROUP_OPERATIONS);
        return;
    }

    refreshModel();
    warningIndicator_->setWarning("", WarningType::GROUP_OPERATIONS); // clear any warning
}

void GroupModel::assignUserToGroup(const QString groupName, const QString user)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM groups_relations WHERE (user_1=:username OR user_2=:username) AND group_name=:gr_name");
    query.bindValue(":username", user);
    query.bindValue(":gr_name", groupName);
    query.exec();
    if (query.next())
    {
        warningIndicator_->setWarning("User " + user + " already belongs to group " + groupName, WarningType::GROUP_OPERATIONS);
        return;
    }
    query.clear();
    query.prepare("SELECT DISTINCT user_1 AS user FROM groups_relations WHERE group_name=:groupname "
        "UNION SELECT DISTINCT user_2 FROM groups_relations WHERE group_name=:groupname");
    query.bindValue(":groupname", groupName);
    query.exec();
    while (query.next())
    {
        qDebug() << query.value("user").toString();
        QSqlQuery addingQuery;
        addingQuery.prepare("INSERT INTO groups_relations(group_name, user_1, user_2, value) "
            "VALUES(:group_name, :user_1, :user_2, :value);");
        addingQuery.bindValue(":group_name", groupName);
        addingQuery.bindValue(":user_1", user);
        addingQuery.bindValue(":user_2", query.value("user").toString());
        addingQuery.bindValue(":value", 0);
        addingQuery.exec();
    }

    warningIndicator_->setWarning("", WarningType::GROUP_OPERATIONS); // clear any warning
}

bool GroupModel::insertInitialActivity(const QString& groupName)
{
    QSqlQuery query;
    const auto gr_activities = groupName + "_activities";
    query.prepare("CREATE TABLE IF NOT EXISTS " + gr_activities + " ("
        "data TEXT,"
        "added_by TEXT,"
        "value REAL,"
        "description TEXT)");
    if (!query.exec())
    {
        qDebug() << "fail to create " << gr_activities << " because: " << query.lastError().text();
        warningIndicator_->setWarning("Communication problem with database.", WarningType::GROUP_CREATION);
        return false;
    }

    query.clear();
    query.prepare("INSERT INTO " + gr_activities + "(data, added_by, value, description) "
        "VALUES(:data, :added_by, :value, :description);");
    query.bindValue(":data", QDateTime::currentDateTime().toString());
    query.bindValue(":added_by", currentStateProvider_->getCurrentLoggedUser());
    query.bindValue(":value", 0);
    query.bindValue(":description", "Group creation");
    if (!query.exec())
    {
        qDebug() << "fail to add to " << gr_activities << " because: " << query.lastError().text();
        warningIndicator_->setWarning("Communication problem with database.", WarningType::GROUP_CREATION);
        return false;
    }
    return true;
}
