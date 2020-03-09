#include "FriendsModel.hpp"

#include <QDebug>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>

FriendsModel::FriendsModel(const UserValidationPtr& userValidation,
    const CurrentStateProviderPtr& currentStateProvider)
    : userValidation_(userValidation), currentStateProvider_(currentStateProvider)
{
    connect(currentStateProvider, &CurrentStateProvider::currentLoggedUserChange,
        this, &FriendsModel::refreshQuery);
}

void FriendsModel::refreshQuery()
{
    if (auto gr_name = currentStateProvider_->getCurrentLoggedUser(); !gr_name.isEmpty())
    {
        QSqlQuery query;
        query.prepare("SELECT "
            "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
            "FROM friends WHERE user_1=:username OR user_2=:username");
        query.bindValue(":username", gr_name);
        query.exec();
        setQuery(query);
    }
}

void FriendsModel::deleteUser(const QString& username)
{
    QSqlQuery query;
    query.prepare("DELETE FROM friends WHERE user_1=:login OR user_2=:login");
    query.bindValue(":login", username);
    query.exec();
    refreshModel();
}

bool FriendsModel::addFriend(const QString& userToAddToFriends)
{
    const auto& currentUser = currentStateProvider_->getCurrentLoggedUser();
    if (currentUser == userToAddToFriends)
    {
        return false;
    }
    if (!userValidation_->isLoginAlreadyUsed(userToAddToFriends))
    {
        qDebug() << "login " << userToAddToFriends << " does not exist";
        return false;
    }
    if (isFriendshipExist(currentUser, userToAddToFriends))
    {
        qDebug() << currentUser << " and " << userToAddToFriends << " are already friends";
        return false;
    }
    QSqlQuery("CREATE TABLE IF NOT EXISTS friends ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_1 TEXT,"
        "user_2 TEXT)");

    QSqlQuery query;
    query.prepare("INSERT INTO friends(user_1, user_2) VALUES(:user_1, :user_2)");
    query.bindValue(":user_1", currentUser);
    query.bindValue(":user_2", userToAddToFriends);
    bool result = query.exec();
    refreshModel();
    return result;
}

bool FriendsModel::removeFriend(const QString &friendToRemove)
{
    const auto& currentUser = currentStateProvider_->getCurrentLoggedUser();

    QSqlQuery query;
    query.prepare("DELETE FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4");
    query.bindValue(":user1", currentUser);
    query.bindValue(":user2", friendToRemove);
    query.bindValue(":user3", friendToRemove);
    query.bindValue(":user4", currentUser);
    if (query.exec())
    {
        qDebug() << "succesfully remove friendship";
        refreshModel();
        return true;
    }

    qDebug() << "fail to remove friendship because: " << query.lastError().text();
    return false;
}

double FriendsModel::getBalanceWithFriend(const QString& friendLogin)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(CASE WHEN user_1=:username THEN value ELSE -value END)"
        " AS 'value' FROM groups_relations WHERE (user_1=:username AND user_2=:friend) OR (user_1=:friend AND user_2=:username)");
    query.bindValue(":username", currentStateProvider_->getCurrentLoggedUser());
    query.bindValue(":friend", friendLogin);
    query.exec();
    query.next();
    return query.value("value").toDouble();
}

bool FriendsModel::isFriendshipExist(const QString& user1, const QString& user2)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4");
    query.bindValue(":user1", user1);
    query.bindValue(":user2", user2);
    query.bindValue(":user3", user2);
    query.bindValue(":user4", user1);

    if (query.exec()) qDebug() << "succesfully check friendship";
    else qDebug() << "fail to check friendship because: " << query.lastError().text();

    return query.next();
}

void FriendsModel::refreshModel()
{
    clear();
    query().clear();
    refreshQuery();
}
