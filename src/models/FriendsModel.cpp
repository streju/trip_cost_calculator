#include "FriendsModel.hpp"

#include <QDebug>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>

#include "CurrentStateProvider.hpp"

FriendsModel::FriendsModel(const std::shared_ptr<ValidationBase>& validation,
    const CurrentStateProviderPtr& currentStateProvider, const QSqlQueryCreatorPtr& queryCreator)
    : validation_(validation), currentStateProvider_(currentStateProvider), queryCreator_(queryCreator)
{
    CurrentStateProvider* stateProvider = dynamic_cast<CurrentStateProvider*>(currentStateProvider);
    connect(stateProvider, &CurrentStateProvider::currentLoggedUserChange,
        this, &FriendsModel::refreshQuery);
}

void FriendsModel::refreshQuery()
{
    if (auto gr_name = currentStateProvider_->getCurrentLoggedUser(); !gr_name.isEmpty())
    {
        const auto query = queryCreator_->create();
        query->prepare("SELECT "
            "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
            "FROM friends WHERE user_1=:username OR user_2=:username");
        query->bindValue(":username", gr_name);
        query->exec();
        setQuery(query->get());
    }
}

void FriendsModel::deleteUser(const QString& username)
{
    const auto query = queryCreator_->create();
    query->prepare("DELETE FROM friends WHERE user_1=:login OR user_2=:login");
    query->bindValue(":login", username);
    query->exec();
    refreshModel();
}

bool FriendsModel::addFriend(const QString& userToAddToFriends)
{
    const auto& currentUser = currentStateProvider_->getCurrentLoggedUser();
    if (currentUser == userToAddToFriends)
    {
        return false;
    }
    if (!validation_->isLoginAlreadyUsed(userToAddToFriends))
    {
        qDebug() << "login " << userToAddToFriends << " does not exist";
        return false;
    }
    qDebug() << "login exist";
    if (isFriendshipExist(currentUser, userToAddToFriends))
    {
        qDebug() << currentUser << " and " << userToAddToFriends << " are already friends";
        return false;
    }
    queryCreator_->create(
        "CREATE TABLE IF NOT EXISTS friends ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_1 TEXT,"
        "user_2 TEXT)");

    const auto query = queryCreator_->create();
    query->prepare("INSERT INTO friends(user_1, user_2) VALUES(:user_1, :user_2)");
    query->bindValue(":user_1", currentUser);
    query->bindValue(":user_2", userToAddToFriends);
    bool result = query->exec();
    refreshModel();
    return result;
}

bool FriendsModel::removeFriend(const QString &friendToRemove)
{
    const auto& currentUser = currentStateProvider_->getCurrentLoggedUser();

    const auto query = queryCreator_->create();
    query->prepare("DELETE FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4");
    query->bindValue(":user1", currentUser);
    query->bindValue(":user2", friendToRemove);
    query->bindValue(":user3", friendToRemove);
    query->bindValue(":user4", currentUser);
    if (query->exec())
    {
        qDebug() << "succesfully remove friendship";
        refreshModel();
        return true;
    }

    qDebug() << "fail to remove friendship because: " << query->lastError().text();
    return false;
}

double FriendsModel::getBalanceWithFriend(const QString& friendLogin)
{
    const auto query = queryCreator_->create();
    query->prepare("SELECT SUM(CASE WHEN user_1=:username THEN value ELSE -value END)"
        " AS 'value' FROM groups_relations WHERE (user_1=:username AND user_2=:friend) OR (user_1=:friend AND user_2=:username)");
    query->bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query->bindValue(":friend", friendLogin);
    query->exec();
    query->next();
    return query->value("value").toDouble();
}

bool FriendsModel::isFriendshipExist(const QString& user1, const QString& user2)
{
    const auto query = queryCreator_->create();
    query->prepare("SELECT 1 FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4");
    query->bindValue(":user1", user1);
    query->bindValue(":user2", user2);
    query->bindValue(":user3", user2);
    query->bindValue(":user4", user1);

    if (query->exec()) qDebug() << "succesfully check friendship";
    else qDebug() << "fail to check friendship because: " << query->lastError().text();

    return query->next();
}

void FriendsModel::refreshModel()
{
    clear();
    query().clear();
    refreshQuery();
}
