#ifndef USERVALIDATION_HPP
#define USERVALIDATION_HPP

#include <memory>

#include "WarningIndicator.hpp"
#include "ValidationBase.hpp"

class UserValidation : public ValidationBase
{
public:
    explicit UserValidation(WarningIndicator* const warningIndicator);

    bool validatePassword(const QString& user, const QString& password);
    bool validateUsername(const QString& username, WarningType type);
private:
    bool isValidPassword(const QString& user, const QString& pswd);

    WarningIndicator* const warningIndicator_;
};

using UserValidationPtr = std::shared_ptr<UserValidation>;

#endif // USERVALIDATION_HPP
