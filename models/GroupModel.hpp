#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include <set>

#include <QString>
#include <QObject>

#include "BaseQSqlQueryModel.hpp"
#include "CurrentStateProvider.hpp"
#include "WarningIndicator.hpp"

class GroupModel : public BaseQSqlQueryModel
{
public:
    GroupModel(WarningIndicatorPtr warningIndicator, const CurrentStateProviderPtr& currentStateProvider);
    void refreshQuery();

    bool addGroup(const QString& groupName, QVariantList usersToAddToGroup);
    bool validateGroupName(const QString& groupName) const;
    double getUserTotalBalance();
    void updateUserTotalBalance();
    void refreshModel();
    void deleteUser(const QString& user);
    void deleteGroup();
    void assignUserToGroup(const QString groupName, const QString user);
signals:
    void userTotalBalanceChanged();
private:
    bool assignUsersToGroup(const QString& groupName, const std::set<QString>& groupUsers) const;
    bool insertInitialActivity(const QString& groupName);

    Q_OBJECT
    Q_PROPERTY(double userTotalBalance READ getUserTotalBalance NOTIFY userTotalBalanceChanged)

    WarningIndicatorPtr warningIndicator_;
    CurrentStateProviderPtr currentStateProvider_;
    double userTotalBalance_;
};

using GroupModelPtr = GroupModel* const;

#endif // GROUPMODEL_HPP
