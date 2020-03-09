#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include <set>

#include <QString>
#include <QObject>

#include "BaseQSqlQueryModel.hpp"
#include "Interfaces/ICurrentStateProvider.hpp"
#include "Interfaces/IQSqlQueryCreator.hpp"
#include "Interfaces/IWarningIndicator.hpp"

class GroupModel : public BaseQSqlQueryModel
{
public:
    GroupModel(WarningIndicatorPtr warningIndicator, const CurrentStateProviderPtr& currentStateProvider,
        const QSqlQueryCreatorPtr& queryCreator);
    void refreshQuery();

    bool addGroup(const QString& groupName, QVariantList usersToAddToGroup);
    bool validateGroupName(const QString& groupName) const;
    void refreshModel();
    void deleteUser(const QString& user);
    void deleteGroup();
    void assignUserToGroup(const QString groupName, const QString user);
signals:
    void userTotalBalanceChanged();
private:
    double getUserTotalBalance();
    void updateUserTotalBalance();
    bool assignUsersToGroup(const QString& groupName, const std::set<QString>& groupUsers) const;
    bool insertInitialActivity(const QString& groupName);

    Q_OBJECT
    Q_PROPERTY(double userTotalBalance READ getUserTotalBalance NOTIFY userTotalBalanceChanged)

    WarningIndicatorPtr warningIndicator_;
    CurrentStateProviderPtr currentStateProvider_;
    QSqlQueryCreatorPtr queryCreator_;
    double userTotalBalance_;
};

using GroupModelPtr = GroupModel* const;

#endif // GROUPMODEL_HPP
