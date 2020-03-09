#ifndef APPPROPERTIES_HPP
#define APPPROPERTIES_HPP

#include <map>
#include <memory>

#include <QObject>
#include <QString>

#include "models/ActivitiesModel.hpp"
#include "Controller.hpp"
#include "Currency.hpp"
#include "CurrentStateProvider.hpp"
#include "models/FriendsModel.hpp"
#include "models/GroupDetails.hpp"
#include "models/GroupModel.hpp"
#include "SqlConnectionHelper.hpp"
#include "UserValidation.hpp"
#include "WarningIndicator.hpp"

struct AppProperties
{
    AppProperties()
    {
        Warning::init();
        CurrencyType::init();

        const auto warningIndicator = new WarningIndicator();
        const auto userValidation = std::make_shared<UserValidation>(warningIndicator);
        const auto currentStateProvider = new CurrentStateProvider(nullptr);
        const auto currencyService = new CurrencyService(warningIndicator, currentStateProvider);
        const auto groupDetails = new GroupDetails(currentStateProvider);
        const auto activitiesModel = new ActivitiesModel(currentStateProvider);
        const auto singleGroupActivitiesModel = new GroupActivitiesModel(currentStateProvider);
        const auto friendsModel = new FriendsModel(userValidation, currentStateProvider);
        const auto groupModel = new GroupModel(warningIndicator, currentStateProvider);
        const auto controller = new Controller(userValidation, groupDetails, friendsModel,
            singleGroupActivitiesModel, groupModel, currencyService, warningIndicator, currentStateProvider);

        properties_.insert(std::make_pair(QString("groupDetailsModel"), groupDetails));
        properties_.insert(std::make_pair(QString("activitiesModel"), activitiesModel));
        properties_.insert(std::make_pair(QString("singleGroupActivitiesModel"), singleGroupActivitiesModel));
        properties_.insert(std::make_pair(QString("friendsModel"), friendsModel));
        properties_.insert(std::make_pair(QString("groupModel"), groupModel));
        properties_.insert(std::make_pair(QString("warningIndicator"), warningIndicator));
        properties_.insert(std::make_pair(QString("stateProvider"), currentStateProvider));
        properties_.insert(std::make_pair(QString("currencyService"), currencyService));
        properties_.insert(std::make_pair(QString("backendController"), controller));
    }

    ~AppProperties()
    {
        for (const auto& property : properties_)
        {
            delete property.second;
        }
    }
    std::map<QString, QObject*> properties_;
};

#endif // APPPROPERTIES_HPP
