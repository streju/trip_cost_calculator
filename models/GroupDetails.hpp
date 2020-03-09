#ifndef GROUPDETAILS_HPP
#define GROUPDETAILS_HPP

#include <QObject>

#include "BaseQSqlQueryModel.hpp"
#include "CurrentStateProvider.hpp"

struct UserBill
{
    UserBill(QString userName, double value) : userName(userName), value(value) {}
    QString getUserName() const {return userName;}
    double getValue() const {return value;}
private:
    QString userName;
    double value;
};

class GroupDetails : public BaseQSqlQueryModel
{
public:
    GroupDetails(const CurrentStateProviderPtr& currentStateProvider);
    void addDebt(const QString& description);
    void addDebtEqually(const QString& description, double totalValue);
    void registerUserBill(const UserBill& bill);
    void clearDebts(const QString& user);
    void refreshModel();
    void refreshQuery();
private:
    Q_OBJECT

    void insertBillToDb(const QString& description);

    std::vector<UserBill> newUsersBills_;
    CurrentStateProviderPtr currentStateProvider_;
};

#endif // GROUPDETAILS_HPP
