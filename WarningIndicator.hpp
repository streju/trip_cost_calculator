#ifndef WARNINGINDICATOR_HPP
#define WARNINGINDICATOR_HPP

#include <QDebug>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QString>

class Warning : public QObject {
    Q_OBJECT
public:
    enum class Type : quint8 {
        VALIDATION,
        REGISTRATION,
        NEW_BILL_CREATION,
        GROUP_CREATION,
        GROUP_OPERATIONS,
        NETWORK
    };
    Q_ENUM(Type)

    static void init()
    {
        qRegisterMetaType<Warning::Type>("Warning::Type");
        qmlRegisterType<Warning>("Warnings", 1, 0, "WarningType");
    }
};

class WarningIndicator : public QObject
{
public:
    WarningIndicator(QObject *parent = nullptr);
    void setWarning(const QString& warning, Warning::Type type);
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

using WarningIndicatorPtr = WarningIndicator* const;
using WarningType = Warning::Type;

#endif // WARNINGINDICATOR_HPP
