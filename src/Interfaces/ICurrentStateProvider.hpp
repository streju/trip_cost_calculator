#ifndef ICURRENTSTATEPROVIDER_HPP
#define ICURRENTSTATEPROVIDER_HPP

#include <memory>
#include <QString>

class ICurrentStateProvider
{
public:
    virtual void setCurrentGroup(const QString& group) = 0;
    virtual void setCurrentLoggedUser(const QString& user) = 0;
    virtual QString getCurrentGroup() const = 0;
    virtual QString getCurrentLoggedUser() const = 0;
    virtual ~ICurrentStateProvider() = default;
};

using CurrentStateProviderPtr = ICurrentStateProvider* const;

#endif // ICURRENTSTATEPROVIDER_HPP
