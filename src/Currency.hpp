#ifndef CURRENCY_HPP
#define CURRENCY_HPP

#include <QObject>
#include <QQmlApplicationEngine>

class CurrencyType : public QObject {
    Q_OBJECT
public:
    enum class Type : quint8 {
        USD,
        EUR,
        GBP
    };
    Q_ENUM(Type)

    static void init()
    {
        qRegisterMetaType<CurrencyType::Type>("Currency::Type");
        qmlRegisterType<CurrencyType>("Currency", 1, 0, "CurrencyType");
    }
};

using Currency = CurrencyType::Type;

#endif // CURRENCY_HPP
