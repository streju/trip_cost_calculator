#ifndef REGISTRATIONSERVICE_HPP
#define REGISTRATIONSERVICE_HPP

#include <QString>

#include "WarningIndicator.hpp"
#include "ValidationBase.hpp"

class RegistrationService : public ValidationBase
{
public:
    explicit RegistrationService(WarningIndicator* const warningIndicator);

    bool validateUsername(const QString &username);
    bool validatePassword(const QString &password);
    void addUser(const QString& username, const QString& pswd);
    bool deleteUser(const QString& username);
private:
    WarningIndicator* const warningIndicator_;
};

using RegistrationServicePtr = std::shared_ptr<RegistrationService>;

#endif // REGISTRATIONSERVICE_HPP
