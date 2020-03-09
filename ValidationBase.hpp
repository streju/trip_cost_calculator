#ifndef VALIDATIONBASE_HPP
#define VALIDATIONBASE_HPP

#include <QCryptographicHash>
#include <QSqlQuery>
#include <QString>

class ValidationBase
{
public:
    bool isLoginAlreadyUsed(const QString& newLogin)
    {
        QSqlQuery query;
        query.exec("SELECT 1 FROM users WHERE login ='" + newLogin + "';");
        return query.next();
    }
protected:
    ValidationBase() {}
    QString hashPassword(const QString& toHash)
    {
        return QString(QCryptographicHash::hash(toHash.toUtf8(), QCryptographicHash::Sha1).toHex());
    }
};

#endif // VALIDATIONBASE_HPP
