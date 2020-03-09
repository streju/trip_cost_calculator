#include "UserValidation.hpp"

#include <QDebug>
#include <QSqlQuery>

UserValidation::UserValidation(WarningIndicator* const warningIndicator)
    : warningIndicator_(warningIndicator)
{
}

bool UserValidation::isValidPassword(const QString& user, const QString& pswd)
{    
    QSqlQuery query;
    query.prepare("SELECT password, salt FROM users WHERE login=:login");
    query.bindValue(":login", user);
    query.exec();
    query.next();
    const auto fetchedPassword = query.value("password").toString();
    const auto salt = query.value("salt").toString();
    return hashPassword(pswd + salt) == fetchedPassword;
}

bool UserValidation::validatePassword(const QString& user, const QString& password)
{
    if (password.isEmpty())
    {
        warningIndicator_->setWarning("Please entry password", WarningType::VALIDATION);
        return false;
    }
    if (!validateUsername(user, WarningType::VALIDATION))
    {
        return false;
    }
    if (!isValidPassword(user, password))
    {
        qDebug() << "Wrong password!";
        return false;
    }
    return true;
}

bool UserValidation::validateUsername(const QString &username, WarningType type)
{
    if(username.isEmpty())
    {
        warningIndicator_->setWarning("Empty username", type);
        return false;
    }
    else if (!isLoginAlreadyUsed(username))
    {
        warningIndicator_->setWarning(username + " does not exist", type);
        return false;
    }
    return true;
}
