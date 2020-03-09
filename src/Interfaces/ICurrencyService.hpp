#ifndef ICURRENCYSERVICE_HPP
#define ICURRENCYSERVICE_HPP

class ICurrencyService
{
public:
    virtual void getCurrenciesRelatedToCurrent() = 0;
    virtual ~ICurrencyService() = default;
};

using CurrencyServicePtr = ICurrencyService* const;

#endif // ICURRENCYSERVICE_HPP
