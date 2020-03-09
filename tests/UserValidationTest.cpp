#include <gtest.h>

#include "UserValidation.hpp"
#include "mocks/QSqlCreatorMock.hpp"
#include "mocks/QSqlQueryMock.hpp"
#include "mocks/WarningIndicatorMock.hpp"

class UserValidationTest : public testing::Test
{
public:
    UserValidationTest()
    {}

    ~UserValidationTest()
    {
        delete warningIndicatorMock_;
    }

    void expectCheckIfLoginAlreadyExist(const QString& login, bool checkResult)
    {
        QString expectedQuery{"SELECT 1 FROM users WHERE login ='" + login +"';"};
        EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
        EXPECT_CALL(*queryMock_, exec(expectedQuery)).WillOnce(testing::Return(true));
        EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(checkResult)).RetiresOnSaturation();
    }

    std::shared_ptr<QSqlQueryMock> queryMock_{std::make_shared<QSqlQueryMock>()};
    WarningIndicatorMock* warningIndicatorMock_{new WarningIndicatorMock()};
    std::shared_ptr<QSqlQueryCreatorMock> queryCreatorMock_{std::make_shared<QSqlQueryCreatorMock>()};
    std::unique_ptr<UserValidation> sut_{std::make_unique<UserValidation>(warningIndicatorMock_, queryCreatorMock_)};
};

TEST_F(UserValidationTest, shouldCheckIfLoginAlreadyExist)
{
    QString loginToCheck{"DUMMY_USER"};
    expectCheckIfLoginAlreadyExist(loginToCheck, true);
    ASSERT_TRUE(sut_->isLoginAlreadyUsed(loginToCheck));
}

TEST_F(UserValidationTest, shouldReturnFalseAndSetWarningIfEmptyUsernameToValidate)
{
    const auto warningType = WarningType::VALIDATION;
    QString loginToCheck{""};
    QString expectedWrn{"Empty username"};

    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, warningType));
    ASSERT_FALSE(sut_->validateUsername(loginToCheck, warningType));
}

TEST_F(UserValidationTest, shouldReturnFalseAndSetWarningIfUserToValidateNotExist)
{
    const auto warningType = WarningType::GROUP_CREATION;
    QString loginToCheck{"USER1992"};
    QString expectedWrn{loginToCheck + " does not exist"};
    expectCheckIfLoginAlreadyExist(loginToCheck, false);
    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, warningType));
    ASSERT_FALSE(sut_->validateUsername(loginToCheck, warningType));
}

TEST_F(UserValidationTest, shouldSuccessfullyValidateIfUsernameExist)
{
    const auto warningType = WarningType::GROUP_CREATION;
    QString loginToCheck{"USER1992"};
    expectCheckIfLoginAlreadyExist(loginToCheck, true);
    ASSERT_TRUE(sut_->validateUsername(loginToCheck, warningType));
}

TEST_F(UserValidationTest, shouldReturnFalseAndSetWarningIfEmptyPasswordToValidate)
{
    QString loginToCheck{"afasf21_pn"};
    QString pswdTocheck{""};
    QString expectedWrn{"Please entry password"};
    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, WarningType::VALIDATION));
    ASSERT_FALSE(sut_->validatePassword(loginToCheck, pswdTocheck));
}

TEST_F(UserValidationTest, shouldReturnFalseAndSetWarningIfUserToValidatePasswordNotExist)
{
    QString loginToCheck{"afasf21_pn"};
    QString pswdTocheck{"1234abcd"};
    QString expectedWrn{loginToCheck + " does not exist"};
    expectCheckIfLoginAlreadyExist(loginToCheck, false);
    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, WarningType::VALIDATION));
    ASSERT_FALSE(sut_->validatePassword(loginToCheck, pswdTocheck));
}

TEST_F(UserValidationTest, shouldReturnFalseAndNotSetWarningIfPasswordNotCorrect)
{
    QString loginToCheck{"afasf21_pn"};
    QString pswdTocheck{"1234abcd"};
    expectCheckIfLoginAlreadyExist(loginToCheck, true);

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("SELECT password, salt FROM users WHERE login=:login")))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(loginToCheck)));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(true)).RetiresOnSaturation();;
    EXPECT_CALL(*queryMock_, value(QString("password"))).WillOnce(testing::Return(QString("pomporam2n23m")));
    EXPECT_CALL(*queryMock_, value(QString("salt"))).WillOnce(testing::Return(QString("acab")));

    ASSERT_FALSE(sut_->validatePassword(loginToCheck, pswdTocheck));
}

TEST_F(UserValidationTest, shouldReturnTrueIfPasswordIsCorrect)
{
    QString loginToCheck{"afasf21_pn"};
    QString pswdTocheck{"1234abcd"};
    QString salt{"1238109#41*&"};
    QString hashedPswd = QCryptographicHash::hash((pswdTocheck + salt).toUtf8(), QCryptographicHash::Sha1).toHex();

    expectCheckIfLoginAlreadyExist(loginToCheck, true);

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("SELECT password, salt FROM users WHERE login=:login")))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(loginToCheck)));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, next()).WillOnce(testing::Return(true)).RetiresOnSaturation();;
    EXPECT_CALL(*queryMock_, value(QString("password"))).WillOnce(testing::Return(hashedPswd));
    EXPECT_CALL(*queryMock_, value(QString("salt"))).WillOnce(testing::Return(salt));

    ASSERT_TRUE(sut_->validatePassword(loginToCheck, pswdTocheck));
}

