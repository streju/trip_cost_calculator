#ifndef WARNINGINDICATORMOCK_HPP
#define WARNINGINDICATORMOCK_HPP

#include <gmock.h>

#include "../src/Interfaces/IWarningIndicator.hpp"

class WarningIndicatorMock : public IWarningIndicator
{
public:
    MOCK_METHOD(void, setWarning, (const QString&, Warning::Type));
};

#endif // WARNINGINDICATORMOCK_HPP

