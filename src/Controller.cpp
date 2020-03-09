#include "Controller.hpp"

#include <QCoreApplication>
#include <QDebug>

Controller::Controller(const UserValidationPtr& userValidation, GroupDetails* const groupDetails, FriendsModelPtr friendsModel,
    GroupActivitiesModelPtr singleGroupActivitiesModel, GroupModelPtr groupModel, CurrencyServicePtr currencyService,
    WarningIndicatorPtr warningIndicator, CurrentStateProviderPtr currentStateProvider,
    RegistrationServicePtr&& registrationService, QObject *parent)
    : QObject(parent),
    userValidation_(userValidation),
    groupDetails_(groupDetails),
    friendsModel_(friendsModel),
    singleGroupActivitiesModel_(singleGroupActivitiesModel),
    groupModel_(groupModel),
    warningIndicator_(warningIndicator),
    currentStateProvider_(currentStateProvider),
    currencyService_(currencyService),
    registrationService_(std::move(registrationService))
{
}

void Controller::registerUserBill(const QString& user, double value)
{
    groupDetails_->registerUserBill({user, value});
}

void Controller::addBill(const QString& description)
{
    groupDetails_->addDebt(description);
    singleGroupActivitiesModel_->refreshModel();
    groupModel_->refreshModel();
}

void Controller::addBillEqually(const QString& description, const QString& value)
{
    groupDetails_->addDebtEqually(description, value.toDouble());
    singleGroupActivitiesModel_->refreshModel();
    groupModel_->refreshModel();
}

void Controller::clearDebt(const QString& user)
{
    groupDetails_->clearDebts(user);
    singleGroupActivitiesModel_->refreshModel();
    groupModel_->refreshModel();
}

bool Controller::validateNewUserPassword(const QString &password)
{
    return registrationService_->validatePassword(password);
}

bool Controller::validateEnteredUsername(const QString &enteredUsername, Warning::Type type)
{
    switch (type)
    {
        case WarningType::GROUP_CREATION:
        case WarningType::GROUP_OPERATIONS:
        case WarningType::VALIDATION:
            return userValidation_->validateUsername(enteredUsername, type);
        case WarningType::REGISTRATION:
            return registrationService_->validateUsername(enteredUsername);
        case WarningType::NEW_BILL_CREATION:
        case WarningType::NETWORK:
            qWarning() << "Username validation not needed during adding new bill in group/network connection. Should never happen.";
            return true;
    }

    qWarning() << "Unsupported WarningType!";
    return false;
}

bool Controller::validateEnteredPassword(const QString &enteredUsername, const QString &password)
{
    return userValidation_->validatePassword(enteredUsername, password);
}

bool Controller::logInUser(const QString& username, const QString &password)
{
    if(validateEnteredUsername(username, WarningType::VALIDATION) &&
        validateEnteredPassword(username, password))
    {
        warningIndicator_->setWarning("", WarningType::VALIDATION);
        return true;
    }
    return false;
}

void Controller::addNewUser(const QString &username, const QString &password)
{
    if (validateEnteredUsername(username, WarningType::REGISTRATION))
    {
        registrationService_->addUser(username, password);
    }
}

bool Controller::deleteUser(const QString& username)
{
    bool result = registrationService_->deleteUser(username);
    friendsModel_->deleteUser(username);
    groupModel_->deleteUser(username);
    return result;
}

bool Controller::addFriend(const QString& userToAdd)
{
    return friendsModel_->addFriend(userToAdd);
}

bool Controller::removeFriend(const QString& userToRemove)
{
    return friendsModel_->removeFriend(userToRemove);
}

void Controller::endApp()
{
    QCoreApplication::quit();
}

bool Controller::addGroup(const QString &groupName, QVariantList usersToAddToGroup)
{
    return groupModel_->addGroup(groupName, usersToAddToGroup);
}

bool Controller::validateGroupName(const QString &groupName)
{
    return groupModel_->validateGroupName(groupName);
}

void Controller::deleteGroup()
{
    groupModel_->deleteGroup();
    groupDetails_->refreshModel();
    singleGroupActivitiesModel_->refreshModel();
}

void Controller::addUserToGroup(const QString& username)
{
    if (validateEnteredUsername(username, WarningType::GROUP_OPERATIONS))
    {
        groupModel_->assignUserToGroup(currentStateProvider_->getCurrentGroup(), username);
        groupDetails_->refreshModel();
    }
}

void Controller::updateCurrencyRates()
{
    currencyService_->getCurrenciesRelatedToCurrent();
}
