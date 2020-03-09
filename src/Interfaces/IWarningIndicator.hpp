#ifndef IWARNINGINDICATOR_HPP
#define IWARNINGINDICATOR_HPP

#include <QObject>
#include <QQmlApplicationEngine>
#include <QString>

class Warning : public QObject
{
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

class IWarningIndicator
{
public:
    virtual void setWarning(const QString& warning, Warning::Type type) = 0;
    virtual ~IWarningIndicator() = default;
};

using WarningIndicatorPtr = IWarningIndicator* const;
using WarningType = Warning::Type;

#endif // IWARNINGINDICATOR_HPP
