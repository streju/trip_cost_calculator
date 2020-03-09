#ifndef CURRENCYSERVICE_HPP
#define CURRENCYSERVICE_HPP

#include "Currency.hpp"
#include "CurrentStateProvider.hpp"
#include "NetworkHelper.hpp"
#include "WarningIndicator.hpp"

class CurrencyService : public QObject
{
public:
    CurrencyService(WarningIndicatorPtr warningIndicator, const CurrentStateProviderPtr& currentStateProvider);

    void getCurrenciesRelatedToCurrent();
signals:
    void usdRateChanged();
    void eurRateChanged();
    void gbpRateChanged();
private slots:
    void handleNetworkReply(const QByteArray& result);

private:

    double getUsdRate();
    double getEurRate();
    double getGbpRate();
    Q_OBJECT

    Q_PROPERTY(double usdRate READ getUsdRate NOTIFY usdRateChanged)
    Q_PROPERTY(double eurRate READ getEurRate NOTIFY eurRateChanged)
    Q_PROPERTY(double gbpRate READ getGbpRate NOTIFY gbpRateChanged)

    WarningIndicatorPtr warningIndicator_;
    CurrentStateProviderPtr currentStateProvider_;
    NetworkHelper networkHelper_{warningIndicator_};

    std::map<Currency, double> rates_;
};

using CurrencyServicePtr = CurrencyService* const;

#endif // CURRENCYSERVICE_HPP
