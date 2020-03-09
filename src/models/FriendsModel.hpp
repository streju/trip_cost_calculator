#ifndef FRIENDS_HPP
#define FRIENDS_HPP

#include <memory>

#include <QObject>
#include <QString>

#include "BaseQSqlQueryModel.hpp"
#include "Interfaces/ICurrentStateProvider.hpp"
#include "Interfaces/IQSqlQueryCreator.hpp"
#include "UserValidation.hpp"

class FriendsModel : public BaseQSqlQueryModel
{
public:
    explicit FriendsModel(const std::shared_ptr<ValidationBase>& validation,
        const CurrentStateProviderPtr& currentStateProvider, const QSqlQueryCreatorPtr& queryCreator);
    bool addFriend(const QString& userToAddToFriends);
    bool removeFriend(const QString& friendToRemove);
    Q_INVOKABLE double getBalanceWithFriend(const QString& friendLogin);
    void refreshQuery();
    void deleteUser(const QString& username);
private:
    bool isFriendshipExist(const QString& user1, const QString& user2);
    void refreshModel();

    Q_OBJECT
    std::shared_ptr<ValidationBase> validation_;
    CurrentStateProviderPtr currentStateProvider_;
    QSqlQueryCreatorPtr queryCreator_;
};

using FriendsModelPtr = FriendsModel* const;

#endif // FRIENDS_HPP
