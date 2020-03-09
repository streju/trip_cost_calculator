#include "GroupDetails.hpp"

#include <QDateTime>
#include <QDebug>
#include <QNetworkReply>
#include <QSqlError>
#include <QSqlQueryModel>

#include "CurrentStateProvider.hpp"

GroupDetails::GroupDetails(const CurrentStateProviderPtr& currentStateProvider,
    const QSqlQueryCreatorPtr& queryCreator)
    : currentStateProvider_(currentStateProvider)
    , queryCreator_(queryCreator)
{
    CurrentStateProvider* stateProvider = dynamic_cast<CurrentStateProvider*>(currentStateProvider);
    QObject::connect(stateProvider, &CurrentStateProvider::currentLoggedUserChange,
        this, &GroupDetails::refreshQuery);
    QObject::connect(stateProvider, &CurrentStateProvider::currentGroupChange,
        this, &GroupDetails::refreshQuery);
}

void GroupDetails::refreshQuery()
{
    if (currentStateProvider_->getCurrentGroup().isEmpty()
        || currentStateProvider_->getCurrentLoggedUser().isEmpty())
    {
        qDebug() << "GroupName or Username not set, unavailable to set query";
        return;
    }

    const auto query = queryCreator_->create();
    query->prepare("SELECT "
        "CASE WHEN user_1=:username THEN value ELSE -value END AS value, "
        "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
        "FROM groups_relations WHERE group_name=:groupname AND "
            "(user_1=:username OR user_2=:username)");
    query->bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query->bindValue(":groupname", currentStateProvider_->getCurrentGroup());
    query->exec();
    BaseQSqlQueryModel::setQuery(query->get());
}

void GroupDetails::addDebt(const QString& description)
{
    if (newUsersBills_.empty())
    {
        qDebug() << "There is no new bill to add to group " << currentStateProvider_->getCurrentGroup();
        return;
    }

    insertBillToDb(description);
}

void GroupDetails::insertBillToDb(const QString& description)
{
    const auto& groupName = currentStateProvider_->getCurrentGroup();
    const auto gr_activities = groupName + "_activities";

    for (const auto& userBill : newUsersBills_)
    {
        const auto fullDescription = userBill.getUserName() + " needs to pay: " + std::to_string(userBill.getValue()).c_str() + " for " + description;

        const auto query = queryCreator_->create();
        query->prepare("INSERT INTO " + gr_activities + "(data, added_by, value, description) "
            "VALUES(:data, :added_by, :value, :description);");
        query->bindValue(":data", QDateTime::currentDateTime().toString());
        query->bindValue(":added_by", currentStateProvider_->getCurrentLoggedUser());
        query->bindValue(":value", userBill.getValue());
        query->bindValue(":description", fullDescription);

        if (query->exec()) qDebug() << "succesfully added to: " << gr_activities;
        else qDebug() << "fail to add to " << gr_activities << " because: " << query->lastError().text();

        query->clear();
        query->prepare("UPDATE groups_relations "
            "SET value= CASE WHEN user_1=:username THEN value+:value ELSE value-:value END "
            "WHERE group_name=:groupname AND ((user_1=:username AND user_2=:userToPay) OR (user_1=:userToPay AND user_2=:username))");
        query->bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
        query->bindValue(":value", userBill.getValue());
        query->bindValue(":groupname", groupName);
        query->bindValue(":userToPay", userBill.getUserName());

        if (query->exec()) qDebug() << "succesfully added to: groups_relations";
        else qDebug() << "fail to add to groups_relations, because: " << query->lastError().text();
    }

    refreshModel();
    qDebug() << "Clearing waiting bills after added to group: " << groupName;
    newUsersBills_.clear();
}

void GroupDetails::addDebtEqually(const QString& description, double totalValue)
{
    newUsersBills_.clear();

    const auto query = queryCreator_->create();
    query->prepare("SELECT "
        "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
        "FROM groups_relations WHERE group_name=:groupname AND (user_1=:username OR user_2=:username)");
    query->bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query->bindValue(":groupname", currentStateProvider_->getCurrentGroup());
    query->exec();

    std::vector<QString> users;
    while (query->next()){users.push_back(query->value("related_user").toString());}

    double valuePerUser = totalValue/(users.size()+1);
    for (const auto& user : users) {newUsersBills_.emplace_back(user, valuePerUser);}

    insertBillToDb(description);
}

void GroupDetails::registerUserBill(const UserBill &bill)
{
    newUsersBills_.push_back(bill);
}

void GroupDetails::clearDebts(const QString& user)
{
    qDebug() << "Going to clear debt : " << user;
    const auto gr_activities = currentStateProvider_->getCurrentGroup() + "_activities";

    const auto query = queryCreator_->create();
    query->prepare("INSERT INTO " + gr_activities + "(data, added_by, value, description) "
        "VALUES(:data, :added_by, :value, :description);");
    query->bindValue(":data", QDateTime::currentDateTime().toString());
    query->bindValue(":added_by", currentStateProvider_->getCurrentLoggedUser());
    query->bindValue(":value", 0);
    query->bindValue(":description",
        "User " + currentStateProvider_->getCurrentLoggedUser() +" cleared balance with user " + user);
    query->exec();

    query->clear();
    query->prepare("UPDATE groups_relations "
        "SET value=0 "
        "WHERE group_name=:groupname AND ((user_1=:username AND user_2=:secUser) OR (user_1=:secUser AND user_2=:username))");
    query->bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query->bindValue(":groupname", currentStateProvider_->getCurrentGroup());
    query->bindValue(":secUser", user);
    query->exec();

    refreshModel();
}

void GroupDetails::refreshModel()
{
    clear();
    query().clear();
    refreshQuery();
}
