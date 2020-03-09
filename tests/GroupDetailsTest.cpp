#include <gtest.h>

#include "models/GroupDetails.hpp"
#include "mocks/CurrentStateProviderMock.hpp"
#include "mocks/QSqlCreatorMock.hpp"
#include "mocks/QSqlQueryMock.hpp"

class GroupDetailsTest : public testing::Test
{
public:
    GroupDetailsTest(){}
    ~GroupDetailsTest() {delete currentStateProviderMock_;}

    void expectRefreshModel()
    {
        EXPECT_CALL(*currentStateProviderMock_, getCurrentGroup()).WillRepeatedly(testing::Return(currentGroup_));
        EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":groupname"), QVariant(currentGroup_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, get()).WillOnce(testing::Return(QSqlQuery()));
    }

    void expectAddDebtForUser(const QString userName, double value)
    {
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":data"), testing::_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":added_by"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":value"), QVariant(value))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":description"), testing::_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

        EXPECT_CALL(*queryMock_, clear()).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":value"), QVariant(value))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":groupname"), QVariant(currentGroup_))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, bindValue(QString(":userToPay"), QVariant(userName))).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    }

    QString currentGroup_{"DUMMY_GROUP"};
    QString currentUser_{"DUMMY_USER"};

    std::shared_ptr<QSqlQueryMock> queryMock_{std::make_shared<QSqlQueryMock>()};
    CurrentStateProviderMock* currentStateProviderMock_{new CurrentStateProviderMock()};
    std::shared_ptr<QSqlQueryCreatorMock> queryCreatorMock_{std::make_shared<QSqlQueryCreatorMock>()};
    std::unique_ptr<GroupDetails> sut_{std::make_unique<GroupDetails>(currentStateProviderMock_, queryCreatorMock_)};
};

TEST_F(GroupDetailsTest, shouldRefreshQuery)
{
    expectRefreshModel();
    sut_->refreshQuery();
}

TEST_F(GroupDetailsTest, shouldClearDebts)
{
    QString userToClear{"USER_234"};
    QString description{"User " + currentUser_ +" cleared balance with user " + userToClear};

    EXPECT_CALL(*currentStateProviderMock_, getCurrentGroup()).WillRepeatedly(testing::Return(currentGroup_));
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":data"), testing::_));
    EXPECT_CALL(*queryMock_, bindValue(QString(":added_by"), QVariant(currentUser_)));
    EXPECT_CALL(*queryMock_, bindValue(QString(":value"), QVariant(0)));
    EXPECT_CALL(*queryMock_, bindValue(QString(":description"), QVariant(description)));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    EXPECT_CALL(*queryMock_, clear()).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":groupname"), QVariant(currentGroup_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":secUser"), QVariant(userToClear)));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();

    expectRefreshModel();
    sut_->clearDebts(userToClear);
}

TEST_F(GroupDetailsTest, shouldAddDebtEqually)
{
    testing::Sequence seq;
    QString description{"Dinner in rennid"};
    double value{290371.23};
    QString relatedUser1{"USER_234"};
    QString relatedUser2{"USER20391"};

    EXPECT_CALL(*currentStateProviderMock_, getCurrentGroup()).WillRepeatedly(testing::Return(currentGroup_));
    EXPECT_CALL(*currentStateProviderMock_, getCurrentLoggedUser()).WillRepeatedly(testing::Return(currentUser_));
    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(testing::_)).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":username"), QVariant(currentUser_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, bindValue(QString(":groupname"), QVariant(currentGroup_))).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, value(QString("related_user"))).InSequence(seq)
        .WillOnce(testing::Return(relatedUser1)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(true)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, value(QString("related_user"))).InSequence(seq)
        .WillOnce(testing::Return(relatedUser2)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, next()).InSequence(seq).WillOnce(testing::Return(false)).RetiresOnSaturation();

    expectAddDebtForUser(relatedUser1, value/3);
    expectAddDebtForUser(relatedUser2, value/3);

    expectRefreshModel();
    sut_->addDebtEqually(description, value);
}

TEST_F(GroupDetailsTest, shouldAddDebtNotEqually)
{
    QString relatedUser1{"USER_234"};
    double valueUser1{290371.23};
    QString relatedUser2{"USER20391"};
    double valueUser2{907.98};
    QString relatedUser3{"_sa2USER_4"};
    double valueUser3{0.9867785};
    QString description{"Lasers"};

    EXPECT_CALL(*queryCreatorMock_, create()).Times(0).RetiresOnSaturation();
    sut_->addDebt(description);

    sut_->registerUserBill({relatedUser1, valueUser1});
    sut_->registerUserBill({relatedUser2, valueUser2});
    sut_->registerUserBill({relatedUser3, valueUser3});

    expectAddDebtForUser(relatedUser1, valueUser1);
    expectAddDebtForUser(relatedUser2, valueUser2);
    expectAddDebtForUser(relatedUser3, valueUser3);

    expectRefreshModel();
    sut_->addDebt(description);
}
