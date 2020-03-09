#ifndef IREGISTRATIONSERVICE_HPP
#define IREGISTRATIONSERVICE_HPP

#include <memory>
#include <QString>

class IRegistrationService
{
public:
    virtual bool validateUsername(const QString &username) = 0;
    virtual bool validatePassword(const QString &password) = 0;
    virtual void addUser(const QString& username, const QString& pswd) = 0;
    virtual bool deleteUser(const QString& username) = 0;
    virtual ~IRegistrationService() = default;
};

using RegistrationServicePtr = std::unique_ptr<IRegistrationService>;

#endif // IREGISTRATIONSERVICE_HPP
