#include <gtest.h>

#include "models/FriendsModel.hpp"
#include "mocks/CurrentStateProviderMock.hpp"
#include "mocks/QSqlCreatorMock.hpp"
#include "mocks/QSqlQueryMock.hpp"
#include "mocks/WarningIndicatorMock.hpp"

class FriendsModelShould : public testing::Test
{
public:
    FriendsModelShould(){}
    ~FriendsModelShould() {delete currentStateProviderMock_; delete warningIndicatorMock_;}

    void expectRefreshModel()
    {
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(QString("SELECT "
            "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
            "FROM friends WHERE user_1=:username OR user_2=:username"))).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, get()).WillOnce(testing::Return(QSqlQuery()));
    }

    void expectCheckIfLoginAlreadyExist(const QString& login, bool checkResult)
    {
        QString expectedQuery{"SELECT 1 FROM users WHERE login ='" + login +"';"};
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec(expectedQuery)).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(checkResult)).RetiresOnSaturation();
    }

    void expectCheckFriendship(const QString& login, bool checkResult)
    {
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(
            QString("SELECT 1 FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4")))
            .WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":user1"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":user2"), QVariant(login))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":user3"), QVariant(login))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":user4"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(checkResult)).RetiresOnSaturation();
    }

    QString currentUser_{"DUMMY_USER"};

    WarningIndicatorMock* warningIndicatorMock_{new WarningIndicatorMock()};
    std::shared_ptr<QSqlQueryMock> queryMock_{std::make_shared<QSqlQueryMock>()};
    CurrentStateProviderMock* currentStateProviderMock_{new CurrentStateProviderMock()};
    std::shared_ptr<QSqlQueryCreatorMock> queryCreatorMock_{std::make_shared<QSqlQueryCreatorMock>()};
    std::shared_ptr<UserValidation> validation_{std::make_shared<UserValidation>(warningIndicatorMock_, queryCreatorMock_)};
    FriendsModel sut_{validation_, currentStateProviderMock_, queryCreatorMock_};
};

TEST_F(FriendsModelShould, refreshQuery)
{
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
    expectRefreshModel();
    sut_.refreshQuery();
}

TEST_F(FriendsModelShould, deleteUser)
{
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("DELETE FROM friends WHERE user_1=:login OR user_2=:login")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    expectRefreshModel();
    sut_.deleteUser(currentUser_);
}

TEST_F(FriendsModelShould, removeFriend)
{
    QString friendToRemove{"fRiEnD"};
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(
        QString("DELETE FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user1"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user2"), QVariant(friendToRemove))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user3"), QVariant(friendToRemove))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user4"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    expectRefreshModel();

    ASSERT_TRUE(sut_.removeFriend(friendToRemove));
}

TEST_F(FriendsModelShould, provideBalanceWithFriend)
{
    QString friendLogin{"my_friend77"};
    double expectedBalance{23.45};
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("SELECT SUM(CASE WHEN user_1=:username THEN value ELSE -value END)"
        " AS 'value' FROM groups_relations WHERE (user_1=:username AND user_2=:friend) OR (user_1=:friend AND user_2=:username)")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":friend"), QVariant(friendLogin))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, value(QString("value"))).WillOnce(testing::Return(QVariant(expectedBalance))).RetiresOnSaturation();

    ASSERT_EQ(sut_.getBalanceWithFriend(friendLogin), expectedBalance);
}

TEST_F(FriendsModelShould, addFriend)
{
    QString friendToAdd{"MYnewFriend21"};

    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    testing::Sequence seq;
    QString expectedQuery{"SELECT 1 FROM users WHERE login ='" + friendToAdd +"';"};
    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec(expectedQuery)).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(
        QString("SELECT 1 FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4")))
        .InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user1"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user2"), QVariant(friendToAdd))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user3"), QVariant(friendToAdd))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user4"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(false)).RetiresOnSaturation();

    EXPECT_CALL(*queryCreatorMock_, create(QString(
        "CREATE TABLE IF NOT EXISTS friends ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_1 TEXT,"
        "user_2 TEXT)"))).InSequence(seq);

    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(
        QString("INSERT INTO friends(user_1, user_2) VALUES(:user_1, :user_2)")))
        .InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user_1"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user_2"), QVariant(friendToAdd))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("SELECT "
        "CASE WHEN user_1=:username THEN user_2 ELSE user_1 END AS related_user "
        "FROM friends WHERE user_1=:username OR user_2=:username"))).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, get()).InSequence(seq).WillOnce(testing::Return(QSqlQuery()));

    ASSERT_TRUE(sut_.addFriend(friendToAdd));
}

TEST_F(FriendsModelShould, notAddFriendWhenFriendToAddEqualWithCurrentUser)
{
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
    EXPECT_CALL(*queryCreatorMock_, create()).Times(0).RetiresOnSaturation();
    ASSERT_FALSE(sut_.addFriend(currentUser_));
}

TEST_F(FriendsModelShould, notAddFriendWhenFriendToAddNotExist)
{
    QString friendToAdd{"MYnewFriend21"};
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    expectCheckIfLoginAlreadyExist(friendToAdd, false);
    ASSERT_FALSE(sut_.addFriend(friendToAdd));
}

TEST_F(FriendsModelShould, notAddFriendWhenFriendWhenFriendshipAlreadyExist)
{
    QString friendToAdd{"MYnewFriend21"};
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));

    testing::Sequence seq;
    QString expectedQuery{"SELECT 1 FROM users WHERE login ='" + friendToAdd +"';"};
    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec(expectedQuery)).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*queryCreatorMock_, create()).InSequence(seq).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(
        QString("SELECT 1 FROM friends WHERE user_1=:user1 AND user_2=:user2 OR user_1=:user3 AND user_2=:user4")))
        .InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user1"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user2"), QVariant(friendToAdd))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user3"), QVariant(friendToAdd))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user4"), QVariant(currentUser_))).InSequence(seq).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    ASSERT_FALSE(sut_.addFriend(friendToAdd));
}
