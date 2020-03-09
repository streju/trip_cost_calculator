#ifndef CURRENCYSERVICE_HPP
#define CURRENCYSERVICE_HPP

#include "Currency.hpp"
#include "Interfaces/ICurrencyService.hpp"
#include "Interfaces/ICurrentStateProvider.hpp"
#include "Interfaces/IWarningIndicator.hpp"
#include "NetworkHelper.hpp"

class CurrencyService : public QObject, public ICurrencyService
{
public:
    CurrencyService(const std::shared_ptr<NetworkHelper>& networkHelper, WarningIndicatorPtr warningIndicator);

    static CurrencyService* create(WarningIndicatorPtr warningIndicator);

    void getCurrenciesRelatedToCurrent() override;
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

    std::shared_ptr<NetworkHelper> networkHelper_;
    WarningIndicatorPtr warningIndicator_;

    std::map<Currency, double> rates_;
};

#endif // CURRENCYSERVICE_HPP
