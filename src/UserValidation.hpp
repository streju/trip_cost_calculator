#ifndef USERVALIDATION_HPP
#define USERVALIDATION_HPP

#include <memory>

#include "Interfaces/IQSqlQueryCreator.hpp"
#include "Interfaces/IUserValidation.hpp"
#include "Interfaces/IWarningIndicator.hpp"
#include "ValidationBase.hpp"

class UserValidation : public IUserValidation, public ValidationBase
{
public:
    UserValidation(WarningIndicatorPtr warningIndicator, const QSqlQueryCreatorPtr& queryCreator);

    bool validatePassword(const QString& user, const QString& password) override;
    bool validateUsername(const QString& username, WarningType type) override;
private:
    bool isValidPassword(const QString& user, const QString& pswd);

    WarningIndicatorPtr warningIndicator_;
};

#endif // USERVALIDATION_HPP
