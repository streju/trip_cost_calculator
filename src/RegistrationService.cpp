#include "RegistrationService.hpp"

#include <random>

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

std::string random_string()
{
     std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

     std::random_device rd;
     std::mt19937 generator(rd());

     std::generate(str.begin(), str.end(), generator);
     return str.substr(0, 32);
}

RegistrationService::RegistrationService(IWarningIndicator* const warningIndicator,
    const QSqlQueryCreatorPtr& queryCreator)
    : ValidationBase(queryCreator)
    , warningIndicator_(warningIndicator)
{
}

bool RegistrationService::validateUsername(const QString &username)
{
    if(username.isEmpty())
    {
        warningIndicator_->setWarning("Empty username", WarningType::REGISTRATION);
        return false;
    }
    else if (isLoginAlreadyUsed(username))
    {
        warningIndicator_->setWarning("Username: " + username + " already used", WarningType::REGISTRATION);
        return false;
    }
    warningIndicator_->setWarning("", WarningType::REGISTRATION);
    return true;
}

bool RegistrationService::validatePassword(const QString &password)
{
    if(password.isEmpty())
    {
        warningIndicator_->setWarning("Password cannot be empty", WarningType::REGISTRATION);
        return false;
    }
    warningIndicator_->setWarning("", WarningType::REGISTRATION);
    return true;
}

void RegistrationService::addUser(const QString &username, const QString &pswd)
{
    queryCreator_->create("CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "login TEXT UNIQUE,"
        "password TEXT,"
        "salt TEXT)");

    const auto salt = random_string();
    const auto hashedPasswd = hashPassword(pswd + salt.c_str());

    const auto query = queryCreator_->create();
    query->prepare("INSERT INTO users(login, password, salt) VALUES(:login, :password, :salt);");
    query->bindValue(":login", username);
    query->bindValue(":password", hashedPasswd);
    query->bindValue(":salt", salt.c_str());
    if (query->exec())
    {
            qDebug() << "Added new user: " << username;
            return;
    }
    qDebug() << "fail to add " << query->lastError().text();
}

bool RegistrationService::deleteUser(const QString& username)
{
    const auto query = queryCreator_->create();
    query->prepare("DELETE FROM users WHERE login=:login");
    query->bindValue(":login", username);
    return query->exec();
}
