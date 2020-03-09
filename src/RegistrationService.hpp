#ifndef REGISTRATIONSERVICE_HPP
#define REGISTRATIONSERVICE_HPP

#include <QString>

#include "Interfaces/IQSqlQueryCreator.hpp"
#include "Interfaces/IRegistrationService.hpp"
#include "Interfaces/IWarningIndicator.hpp"
#include "ValidationBase.hpp"

class RegistrationService : public IRegistrationService, public ValidationBase
{
public:
    explicit RegistrationService(IWarningIndicator* const warningIndicator, const QSqlQueryCreatorPtr& queryCreator);

    bool validateUsername(const QString &username) override;
    bool validatePassword(const QString &password) override;
    void addUser(const QString& username, const QString& pswd) override;
    bool deleteUser(const QString& username) override;
private:
    IWarningIndicator* const warningIndicator_;
};

#endif // REGISTRATIONSERVICE_HPP
