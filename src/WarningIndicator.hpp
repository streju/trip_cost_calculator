#ifndef WARNINGINDICATOR_HPP
#define WARNINGINDICATOR_HPP

#include <QDebug>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QString>

#include "Interfaces/IWarningIndicator.hpp"


class WarningIndicator : public QObject, public IWarningIndicator
{
public:
    WarningIndicator(QObject *parent = nullptr);
    void setWarning(const QString& warning, Warning::Type type) override;
signals:
    void validationWarningChange();
    void registrationWarningChange();
    void newBillCreationWarningChange();
    void groupCreationWarningChange();
    void groupOperationsWarningChange();
    void networkWarningChange();
private:
    Q_OBJECT
    Q_PROPERTY(QString validationWarning READ getValidationWarning NOTIFY validationWarningChange)
    Q_PROPERTY(QString registrationWarning READ getRegistrationWarning NOTIFY registrationWarningChange)
    Q_PROPERTY(QString newBillCreationWarning READ getNBCreationWarning NOTIFY newBillCreationWarningChange)
    Q_PROPERTY(QString groupCreationWarning READ getGroupCreationWarning NOTIFY groupCreationWarningChange)
    Q_PROPERTY(QString groupOperationsWarning READ getGroupOperationsWarning NOTIFY groupOperationsWarningChange)
    Q_PROPERTY(QString networkWarning READ getNetworkWarning NOTIFY networkWarningChange)

    QString getValidationWarning();
    QString getRegistrationWarning();
    QString getNBCreationWarning();
    QString getGroupCreationWarning();
    QString getGroupOperationsWarning();
    QString getNetworkWarning();
    QString getWarning(Warning::Type type);

    std::map<Warning::Type, QString> warnings_;
};

#endif // WARNINGINDICATOR_HPP
