#ifndef VALIDATIONBASE_HPP
#define VALIDATIONBASE_HPP

#include <QCryptographicHash>
#include <QSqlQuery>
#include <QString>

#include "Interfaces/IQSqlQueryCreator.hpp"

class ValidationBase
{
public:
    bool isLoginAlreadyUsed(const QString& newLogin)
    {
        const auto query = queryCreator_->create();
        query->exec("SELECT 1 FROM users WHERE login ='" + newLogin + "';");
        return query->next();
    }
protected:
    ValidationBase(const QSqlQueryCreatorPtr& queryCreator) : queryCreator_(queryCreator) {}
    QString hashPassword(const QString& toHash)
    {
        return QString(QCryptographicHash::hash(toHash.toUtf8(), QCryptographicHash::Sha1).toHex());
    }
    QSqlQueryCreatorPtr queryCreator_;
};

#endif // VALIDATIONBASE_HPP
