#include <gtest.h>
#include <QSignalSpy>

#include "Controller.hpp"
#include "SqlConnectionHelper.hpp"
#include "RegistrationService.hpp"
#include "QSqlQueryCreator.hpp"
#include "WarningIndicator.hpp"
#include "CurrentStateProvider.hpp"

class ControllerShould : public testing::Test
{
public:
    ControllerShould()
    {}

    ~ControllerShould()
    {
        delete warningIndicator_;
        delete currentStateProvider_;
        delete currencyService_;
        delete groupDetails_;
        delete activitiesModel_;
        delete singleGroupActivitiesModel_;
        delete friendsModel_;
        delete groupModel_;
    }
    SqlConnectionHelper sqlHelper{true};
    WarningIndicator* const warningIndicator_{new WarningIndicator()};
    std::shared_ptr<QSqlQueryCreator> queryCreator_{std::make_shared<QSqlQueryCreator>()};
    std::shared_ptr<UserValidation> userValidation_{std::make_shared<UserValidation>(warningIndicator_, queryCreator_)};
    CurrentStateProviderPtr currentStateProvider_{new CurrentStateProvider()};
    CurrencyServicePtr currencyService_{CurrencyService::create(warningIndicator_)};
    GroupDetails* const groupDetails_{new GroupDetails(currentStateProvider_, queryCreator_)};
    ActivitiesModel* const activitiesModel_{new ActivitiesModel(currentStateProvider_, queryCreator_)};
    GroupActivitiesModelPtr singleGroupActivitiesModel_{new GroupActivitiesModel(currentStateProvider_)};
    FriendsModelPtr friendsModel_{new FriendsModel(userValidation_, currentStateProvider_, queryCreator_)};
    GroupModelPtr groupModel_{new GroupModel(warningIndicator_, currentStateProvider_, queryCreator_)};
    Controller sut_{userValidation_, groupDetails_, friendsModel_, singleGroupActivitiesModel_, groupModel_,
        currencyService_, warningIndicator_, currentStateProvider_, std::make_unique<RegistrationService>(warningIndicator_, queryCreator_)};
};

TEST_F(ControllerShould, validateAndRegisterUser)
{
    QString user{"User876"};
    QString user_pswd{"pass"};

    ASSERT_FALSE(sut_.validateEnteredUsername("", WarningType::VALIDATION));
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString("Empty username"));

    ASSERT_FALSE(sut_.validateEnteredUsername(user, WarningType::VALIDATION));
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString(user + " does not exist"));

    ASSERT_FALSE(sut_.validateEnteredUsername("", WarningType::REGISTRATION));
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString("Empty username"));

    ASSERT_TRUE(sut_.validateEnteredUsername(user, WarningType::REGISTRATION));
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    ASSERT_FALSE(sut_.validateNewUserPassword(""));
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString("Password cannot be empty"));

    ASSERT_TRUE(sut_.validateNewUserPassword(user_pswd));
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString(""));
}

TEST_F(ControllerShould, addNewUser)
{
    QString user{"User876"};
    QString user_pswd{"pass"};

    sut_.addNewUser("", "");
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString("Empty username"));

    sut_.addNewUser(user, user_pswd);
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    ASSERT_FALSE(sut_.validateEnteredUsername(user, WarningType::REGISTRATION));
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString("Username: " + user + " already used"));

    ASSERT_TRUE(sut_.validateEnteredUsername(user, WarningType::VALIDATION));

    ASSERT_FALSE(sut_.validateEnteredPassword(user, "p983"));
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());

    ASSERT_TRUE(sut_.validateEnteredPassword(user, user_pswd));
}

TEST_F(ControllerShould, logInUser)
{
    QString user{"42numberoppmn"};
    QString user_pswd{";p.,ams"};

    ASSERT_FALSE(sut_.logInUser(user, user_pswd));
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString(user + " does not exist"));

    sut_.addNewUser(user, user_pswd);
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    ASSERT_FALSE(sut_.logInUser(user, "1234"));

    ASSERT_TRUE(sut_.logInUser(user, user_pswd));
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
}

TEST_F(ControllerShould, endAppWithoutCrash)
{
    sut_.endApp();
}

TEST_F(ControllerShould, deleteUser)
{
    QString user{"42numberoppmn"};
    QString user_pswd{"deleteMe"};

    ASSERT_FALSE(sut_.deleteUser(user));

    sut_.addNewUser(user, user_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    currentStateProvider_->setCurrentLoggedUser(user);

    ASSERT_TRUE(sut_.deleteUser(user));
}

TEST_F(ControllerShould, addNewGroup)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QVariantList usersToAddToGroup{user1, user2, user3};
    QString newGroupName{"new_awesome_dummy_group"};

    sut_.addNewUser(user1, user1_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());
    sut_.addNewUser(user2, user2_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());
    sut_.addNewUser(user3, user3_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    currentStateProvider_->setCurrentLoggedUser(user1);

    ASSERT_FALSE(sut_.validateGroupName(newGroupName));

    ASSERT_FALSE(sut_.addGroup(newGroupName, QVariantList()));
    ASSERT_EQ(warningIndicator_->property("groupCreationWarning").toString(), QString("Cannot create group without users."));

    ASSERT_FALSE(sut_.addGroup("", usersToAddToGroup));
    ASSERT_EQ(warningIndicator_->property("groupCreationWarning").toString(), QString("Group has to have name."));

    ASSERT_TRUE(sut_.addGroup(newGroupName, usersToAddToGroup));
    ASSERT_EQ(warningIndicator_->property("groupCreationWarning").toString(), QString());

    ASSERT_TRUE(sut_.validateGroupName(newGroupName));
    ASSERT_EQ(warningIndicator_->property("groupCreationWarning").toString(), QString("Already exist group with name: " + newGroupName));
}

TEST_F(ControllerShould, deleteGroup)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QVariantList usersToAddToGroup{user1, user2, user3};

    sut_.addNewUser(user1, user1_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());
    sut_.addNewUser(user2, user2_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());
    sut_.addNewUser(user3, user3_pswd);
    ASSERT_EQ(warningIndicator_->property("validationWarning").toString(), QString());
    ASSERT_EQ(warningIndicator_->property("registrationWarning").toString(), QString());

    currentStateProvider_->setCurrentLoggedUser(user1);

    ASSERT_TRUE(sut_.addGroup("new_awesome_dummy_group", usersToAddToGroup));
    ASSERT_EQ(warningIndicator_->property("groupCreationWarning").toString(), QString());

    sut_.deleteGroup();
    ASSERT_EQ(warningIndicator_->property("groupOperationsWarning").toString(), QString("Communication problem with database."));

    currentStateProvider_->setCurrentGroup("new_awesome_dummy_group");

    sut_.deleteGroup();
    ASSERT_EQ(warningIndicator_->property("groupOperationsWarning").toString(), QString());
}

TEST_F(ControllerShould, addUserToGroup)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QVariantList usersInGroup{user1, user2};
    QString groupName{"new_awesome_dummy_group"};

    sut_.addNewUser(user1, user1_pswd);
    sut_.addNewUser(user2, user2_pswd);
    currentStateProvider_->setCurrentLoggedUser(user1);
    ASSERT_TRUE(sut_.addGroup(groupName, usersInGroup));

    currentStateProvider_->setCurrentGroup(groupName);
    sut_.addUserToGroup(user3);
    ASSERT_EQ(warningIndicator_->property("groupOperationsWarning").toString(), QString(user3 + " does not exist"));

    sut_.addNewUser(user3, user3_pswd);
    sut_.addUserToGroup(user3);
    ASSERT_EQ(warningIndicator_->property("groupOperationsWarning").toString(), QString());
}

TEST_F(ControllerShould, addNewBill)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QVariantList usersInGroup{user1, user2};
    QString groupName{"new_awesome_dummy_group"};
    double user3Debt{21.37};

    sut_.addNewUser(user1, user1_pswd);
    sut_.addNewUser(user2, user2_pswd);
    currentStateProvider_->setCurrentLoggedUser(user1);
    ASSERT_TRUE(sut_.addGroup(groupName, usersInGroup));
    currentStateProvider_->setCurrentGroup(groupName);
    sut_.addNewUser(user3, user3_pswd);
    sut_.addUserToGroup(user3);

    sut_.addBill("");

    QSignalSpy spy(groupModel_, SIGNAL(userTotalBalanceChanged()));
    sut_.registerUserBill(user3, user3Debt);
    sut_.addBill("money_for_");

    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(groupModel_->property("userTotalBalance").toDouble(), user3Debt);

    currentStateProvider_->setCurrentLoggedUser(user3);
    ASSERT_EQ(groupModel_->property("userTotalBalance").toDouble(), -user3Debt);
}

TEST_F(ControllerShould, addNewBillEqually)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QVariantList usersInGroup{user1, user2, user3};
    QString groupName{"new_awesome_dummy_group"};
    double debt{40};


    sut_.addNewUser(user1, user1_pswd);
    sut_.addNewUser(user2, user2_pswd);
    sut_.addNewUser(user2, user3_pswd);
    currentStateProvider_->setCurrentLoggedUser(user1);
    ASSERT_TRUE(sut_.addGroup(groupName, usersInGroup));
    currentStateProvider_->setCurrentGroup(groupName);

    QSignalSpy spy(groupModel_, SIGNAL(userTotalBalanceChanged()));
    sut_.addBillEqually("new_bill", QString("40"));

    ASSERT_EQ(spy.count(), 2);
    ASSERT_EQ(groupModel_->property("userTotalBalance").toDouble(), debt*2/3);

    currentStateProvider_->setCurrentLoggedUser(user2);
    ASSERT_EQ(groupModel_->property("userTotalBalance").toDouble(), -debt/3);
}

TEST_F(ControllerShould, addFriend)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};

    sut_.addNewUser(user1, user1_pswd);
    ASSERT_TRUE(sut_.logInUser(user1, user1_pswd));
    currentStateProvider_->setCurrentLoggedUser(user1);

    sut_.addNewUser(user3, user3_pswd);

    ASSERT_FALSE(sut_.addFriend(user1));
    ASSERT_FALSE(sut_.addFriend("USER_THAT_NOT_EXIST"));

    ASSERT_TRUE(sut_.addFriend(user3));

    ASSERT_FALSE(sut_.addFriend(user3));
}

TEST_F(ControllerShould, removeFriend)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};

    sut_.addNewUser(user1, user1_pswd);
    ASSERT_TRUE(sut_.logInUser(user1, user1_pswd));
    currentStateProvider_->setCurrentLoggedUser(user1);

    sut_.addNewUser(user3, user3_pswd);

    ASSERT_TRUE(sut_.addFriend(user3));

    sut_.removeFriend(user3);
}

TEST_F(ControllerShould, provideProperBalanceWithFriend)
{
    QString user1{"user1"};
    QString user1_pswd{"user1_pswd"};
    QString user2{"user2"};
    QString user2_pswd{"user2_pswd"};
    QString user3{"user3"};
    QString user3_pswd{"user3_pswd"};
    QString groupName1{"group1"};
    QString groupName2{"group2"};

    sut_.addNewUser(user1, user1_pswd);
    sut_.addNewUser(user2, user2_pswd);
    sut_.addNewUser(user3, user3_pswd);
    ASSERT_TRUE(sut_.logInUser(user1, user1_pswd));
    currentStateProvider_->setCurrentLoggedUser(user1);

    ASSERT_TRUE(sut_.addFriend(user3));

    ASSERT_TRUE(sut_.addGroup(groupName1, {user1, user2, user3}));
    currentStateProvider_->setCurrentGroup(groupName1);

    ASSERT_EQ(friendsModel_->getBalanceWithFriend(user3), 0);

    sut_.addBillEqually("DUMMY_DESCRIPTION", "30");
    ASSERT_EQ(friendsModel_->getBalanceWithFriend(user3), 10);

    ASSERT_TRUE(sut_.addGroup(groupName2, {user1, user3}));
    currentStateProvider_->setCurrentGroup(groupName2);

    sut_.registerUserBill(user3, -3);
    sut_.addBill("SEC_DUMMY_DESCRIPTION");

    ASSERT_EQ(friendsModel_->getBalanceWithFriend(user3), 7);
}
