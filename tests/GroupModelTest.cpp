#include <gtest.h>
#include <QSignalSpy>

#include "models/GroupModel.hpp"
#include "mocks/CurrentStateProviderMock.hpp"
#include "mocks/QSqlCreatorMock.hpp"
#include "mocks/QSqlQueryMock.hpp"
#include "mocks/WarningIndicatorMock.hpp"

class GroupModelShould : public testing::Test
{
public:
    GroupModelShould(){}
    ~GroupModelShould() {delete currentStateProviderMock_; delete warningIndicatorMock_;}

    void expectRefreshModel(double balance)
    {
        EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(QString(
            "SELECT group_name, user_1, user_2, SUM(CASE WHEN user_1=:username THEN value ELSE -value END) AS 'value'"
            "FROM groups_relations WHERE user_1=:username OR user_2=:username GROUP BY group_name")))
            .WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, get()).WillOnce(testing::Return(QSqlQuery()));

       expectUpdateUserTotalBalance(balance);
    }

    void expectUpdateUserTotalBalance(double balance)
    {
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(QString("SELECT SUM(CASE WHEN user_1=:username THEN value ELSE -value END)"
            " AS 'value' FROM groups_relations WHERE user_1=:username OR user_2=:username")))
            .WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, value(QString("value"))).WillOnce(testing::Return(QVariant(balance))).RetiresOnSaturation();
    }

    QString currentGroup_{"DUMMY_GROUP"};
    QString currentUser_{"DUMMY_USER"};

    std::shared_ptr<QSqlQueryMock> queryMock_{std::make_shared<QSqlQueryMock>()};
    WarningIndicatorMock* warningIndicatorMock_{new WarningIndicatorMock()};
    CurrentStateProviderMock* currentStateProviderMock_{new CurrentStateProviderMock()};
    std::shared_ptr<QSqlQueryCreatorMock> queryCreatorMock_{std::make_shared<QSqlQueryCreatorMock>()};
    std::unique_ptr<GroupModel> sut_{std::make_unique<GroupModel>(warningIndicatorMock_, currentStateProviderMock_, queryCreatorMock_)};
};

TEST_F(GroupModelShould, refreshQuery)
{
    double balance{217};
    QSignalSpy spy(sut_.get(), SIGNAL(userTotalBalanceChanged()));

    expectRefreshModel(balance);
    sut_->refreshQuery();

    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(sut_->property("userTotalBalance").toDouble(), balance);
}

TEST_F(GroupModelShould, setWarningDuringValidateGroupName)
{
    QString newGroupName("fasfasfganlhy");

    QString expectedQuery{"SELECT 1 FROM groups WHERE name ='" + newGroupName + "';"};
    QString expectedWarning{"Already exist group with name: " + newGroupName};
    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_));
    EXPECT_CALL(*queryMock_, exec(expectedQuery)).WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(true));
    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWarning, WarningType::GROUP_CREATION));

    ASSERT_TRUE(sut_->validateGroupName(newGroupName));
}

TEST_F(GroupModelShould, validateGroupName)
{
    QString newGroupName("salkfa_gr");

    QString expectedQuery{"SELECT 1 FROM groups WHERE name ='" + newGroupName + "';"};
    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_));
    EXPECT_CALL(*queryMock_, exec(expectedQuery)).WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(false));

    ASSERT_FALSE(sut_->validateGroupName(newGroupName));
}

TEST_F(GroupModelShould, reactToDeleteUser)
{
    QString deletedUser("bAD_USR");

    QSignalSpy spy(sut_.get(), SIGNAL(userTotalBalanceChanged()));

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("DELETE FROM users WHERE user_1=:login OR user_2=:login")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(deletedUser))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    expectRefreshModel(0);
    expectUpdateUserTotalBalance(0);
    sut_->deleteUser(deletedUser);

    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(sut_->property("userTotalBalance").toDouble(), 0);
}

TEST_F(GroupModelShould, assignUserToGroup)
{
    QString userToAssign("usr8");
    QString groupToAssignUser("120###");

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString(
        "SELECT 1 FROM groups_relations WHERE (user_1=:username OR user_2=:username) AND group_name=:gr_name")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(userToAssign))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":gr_name"), QVariant(groupToAssignUser))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(false))
        .WillOnce(testing::Return(true))
        .WillOnce(testing::Return(false)).RetiresOnSaturation();

    EXPECT_CALL(*queryMock_, clear()).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString(
        "SELECT DISTINCT user_1 AS user FROM groups_relations WHERE group_name=:groupname "
        "UNION SELECT DISTINCT user_2 FROM groups_relations WHERE group_name=:groupname")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":groupname"), QVariant(groupToAssignUser))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString(
        "INSERT INTO groups_relations(group_name, user_1, user_2, value) "
        "VALUES(:group_name, :user_1, :user_2, :value);")))
        .WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":group_name"), QVariant(groupToAssignUser))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user_1"), QVariant(userToAssign))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, value(QString("user"))).WillOnce(testing::Return("relatedUser")).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":user_2"), QVariant("relatedUser"))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":value"), QVariant(0))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*warningIndicatorMock_, setWarning(QString(""), WarningType::GROUP_OPERATIONS));
    sut_->assignUserToGroup(groupToAssignUser, userToAssign);
}
