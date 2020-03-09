#include "CurrencyService.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>

CurrencyService::CurrencyService(WarningIndicatorPtr warningIndicator, const CurrentStateProviderPtr& currentStateProvider)
    : warningIndicator_(warningIndicator), currentStateProvider_(currentStateProvider)
{
    connect(&networkHelper_, SIGNAL(networkDataReady(QByteArray)), this, SLOT(handleNetworkReply(const QByteArray&)));
}

void CurrencyService::getCurrenciesRelatedToCurrent()
{
    networkHelper_.makeRequest("https://api.exchangeratesapi.io/latest?base=PLN");
}

void CurrencyService::handleNetworkReply(const QByteArray& result)
{
    if (result.isEmpty())
    {
        qDebug() << "Empty reply";
        warningIndicator_->setWarning("Unable to get currency rate", WarningType::GROUP_OPERATIONS);
        return;
    }

    auto jsonDocument = QJsonDocument::fromJson(result);
    qDebug() << "USD " << jsonDocument["rates"]["USD"].toDouble();
    qDebug() << "EUR " << jsonDocument["rates"]["EUR"].toDouble();
    qDebug() << "GBP " << jsonDocument["rates"]["GBP"].toDouble();

    rates_[Currency::USD] = 1/jsonDocument["rates"]["USD"].toDouble();
    rates_[Currency::EUR] = 1/jsonDocument["rates"]["EUR"].toDouble();
    rates_[Currency::GBP] = 1/jsonDocument["rates"]["GBP"].toDouble();

    emit usdRateChanged();
    emit eurRateChanged();
    emit gbpRateChanged();
}

double CurrencyService::getUsdRate()
{
    return rates_[Currency::USD];
}
double CurrencyService::getEurRate()
{
    return rates_[Currency::EUR];
}
double CurrencyService::getGbpRate()
{
    return rates_[Currency::GBP];
}
