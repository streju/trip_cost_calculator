#ifndef CURRENTSTATEPROVIDERMOCK_HPP
#define CURRENTSTATEPROVIDERMOCK_HPP

#include <gmock.h>

#include "Interfaces/ICurrentStateProvider.hpp"

class CurrentStateProviderMock : public ICurrentStateProvider
{
public:
    MOCK_METHOD(void, setCurrentGroup, (const QString&));
    MOCK_METHOD(void, setCurrentLoggedUser, (const QString&));
    MOCK_METHOD(QString, getCurrentGroup, (), (const));
    MOCK_METHOD(QString, getCurrentLoggedUser, (), (const));
};

#endif // CURRENTSTATEPROVIDERMOCK_HPP

