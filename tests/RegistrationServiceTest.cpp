#include <gtest.h>

#include "RegistrationService.hpp"
#include "mocks/QSqlCreatorMock.hpp"
#include "mocks/QSqlQueryMock.hpp"
#include "mocks/WarningIndicatorMock.hpp"

class RegistrationServiceShould : public testing::Test
{
public:
    RegistrationServiceShould()
    {}

    ~RegistrationServiceShould()
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
    std::unique_ptr<RegistrationService> sut_{std::make_unique<RegistrationService>(warningIndicatorMock_, queryCreatorMock_)};
};

TEST_F(RegistrationServiceShould, SuccessfullyValidateIfPasswordNotEmpty)
{
    QString password{"DUMMY_PASSWORD"};
    ASSERT_TRUE(sut_->validatePassword(password));
}

TEST_F(RegistrationServiceShould, SetWarningIfPasswordEmpty)
{
    QString password{""};
    QString expectedWrn{"Password cannot be empty"};

    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, WarningType::REGISTRATION));
    ASSERT_FALSE(sut_->validatePassword(password));
}

TEST_F(RegistrationServiceShould, shouldReturnFalseAndSetWarningIfEmptyUsernameToValidate)
{
    QString loginToCheck{""};
    QString expectedWrn{"Empty username"};

    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, WarningType::REGISTRATION));
    ASSERT_FALSE(sut_->validateUsername(loginToCheck));
}

TEST_F(RegistrationServiceShould, shouldReturnFalseAndSetWarningIfUserToValidateAlreadyExist)
{
    QString loginToCheck{"AWESOME_USER"};
    QString expectedWrn{"Username: " + loginToCheck + " already used"};

    expectCheckIfLoginAlreadyExist(loginToCheck, true);
    EXPECT_CALL(*warningIndicatorMock_, setWarning(expectedWrn, WarningType::REGISTRATION));
    ASSERT_FALSE(sut_->validateUsername(loginToCheck));
}

TEST_F(RegistrationServiceShould, shouldSuccessfullyValidateIfUsernameNotExist)
{
    QString loginToCheck{"USER_RESU"};
    expectCheckIfLoginAlreadyExist(loginToCheck, false);
    ASSERT_TRUE(sut_->validateUsername(loginToCheck));
}

TEST_F(RegistrationServiceShould, shouldDeleteUser)
{
    QString loginToDelete{"BAD_USER_42"};
    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("DELETE FROM users WHERE login=:login"))).WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(loginToDelete)));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true));

    ASSERT_TRUE(sut_->deleteUser(loginToDelete));
}

TEST_F(RegistrationServiceShould, shouldAddNewUser)
{
    QString loginToAdd{"NEW_SUPER_USER_997"};
    QString password{"password"};
    QVariant hashedPassword;
    QVariant createdSalt;
    EXPECT_CALL(*queryCreatorMock_, create(QString("CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE,"
        "password TEXT,"
        "salt TEXT)")));

    EXPECT_CALL(*queryCreatorMock_, create()).WillOnce(testing::Return(queryMock_)).RetiresOnSaturation();
    EXPECT_CALL(*queryMock_, prepare(QString("INSERT INTO users(login, password, salt) VALUES(:login, :password, :salt);")))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(*queryMock_, bindValue(QString(":login"), QVariant(loginToAdd)));
    EXPECT_CALL(*queryMock_, bindValue(QString(":password"), testing::_))
            .WillOnce(testing::SaveArg<1>(&hashedPassword));
    EXPECT_CALL(*queryMock_, bindValue(QString(":salt"), testing::_))
        .WillOnce(testing::SaveArg<1>(&createdSalt));
    EXPECT_CALL(*queryMock_, exec()).WillOnce(testing::Return(true));

    sut_->addUser(loginToAdd, password);

    QString expectedHash = QCryptographicHash::hash((password + createdSalt.toString()).toUtf8(), QCryptographicHash::Sha1).toHex();
    ASSERT_EQ(hashedPassword.toString(), expectedHash);
}
