#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <memory>

#include <QObject>

#include "models/ActivitiesModel.hpp"
#include "CurrencyService.hpp"
#include "Interfaces/ICurrentStateProvider.hpp"
#include "models/FriendsModel.hpp"
#include "models/GroupDetails.hpp"
#include "models/GroupModel.hpp"
#include "Interfaces/IRegistrationService.hpp"
#include "UserValidation.hpp"
#include "Interfaces/IWarningIndicator.hpp"

class Controller : public QObject
{
public:
    Controller(const UserValidationPtr& userValidation, GroupDetails* const groupDetails, FriendsModelPtr friendsModel,
        GroupActivitiesModelPtr singleGroupActivitiesModel, GroupModelPtr groupModel, CurrencyServicePtr currencyService,
        WarningIndicatorPtr warningIndicator, CurrentStateProviderPtr currentStateProvider,
        RegistrationServicePtr&& registrationService, QObject *parent = nullptr);

    Q_INVOKABLE void registerUserBill(const QString& user, double value);
    Q_INVOKABLE void addBill(const QString& description);
    Q_INVOKABLE void addBillEqually(const QString& description, const QString& value);
    Q_INVOKABLE void clearDebt(const QString& user);
    Q_INVOKABLE bool validateEnteredUsername(const QString &enteredUsername, Warning::Type type);
    Q_INVOKABLE bool validateNewUserPassword(const QString &password);
    Q_INVOKABLE bool validateEnteredPassword(const QString &enteredUsername, const QString &password);
    Q_INVOKABLE bool logInUser(const QString& username, const QString &password);
    Q_INVOKABLE void addNewUser(const QString& username, const QString& password);
    Q_INVOKABLE bool deleteUser(const QString& username);
    Q_INVOKABLE bool addFriend(const QString& userToAdd);
    Q_INVOKABLE bool removeFriend(const QString& userToRemove);
    Q_INVOKABLE void endApp();
    Q_INVOKABLE bool addGroup(const QString& groupName, QVariantList usersToAddToGroup);
    Q_INVOKABLE bool validateGroupName(const QString& groupName);
    Q_INVOKABLE void deleteGroup();
    Q_INVOKABLE void addUserToGroup(const QString& username);
    Q_INVOKABLE void updateCurrencyRates();
signals:

private:
    Q_OBJECT

    const UserValidationPtr userValidation_;
    GroupDetails* const groupDetails_;
    FriendsModelPtr friendsModel_;
    GroupActivitiesModelPtr singleGroupActivitiesModel_;
    GroupModelPtr groupModel_;
    WarningIndicatorPtr warningIndicator_;
    CurrentStateProviderPtr currentStateProvider_;
    CurrencyServicePtr currencyService_;
    RegistrationServicePtr registrationService_;
};

#endif // CONTROLLER_HPP
