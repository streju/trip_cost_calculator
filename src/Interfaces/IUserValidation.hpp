#ifndef IUSERVALIDATION_HPP
#define IUSERVALIDATION_HPP

#include <memory>
#include <QString>

#include "IWarningIndicator.hpp"

class IUserValidation
{
public:
    virtual bool validatePassword(const QString& user, const QString& password) = 0;
    virtual bool validateUsername(const QString& username, WarningType type) = 0;
    virtual ~IUserValidation() = default;
};

using UserValidationPtr = std::shared_ptr<IUserValidation>;

#endif // IUSERVALIDATION_HPP
