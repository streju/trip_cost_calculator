#include "NetworkHelper.hpp"

#include <QNetworkReply>
#include <QEventLoop>
#include <QtNetwork>

NetworkHelper::NetworkHelper(WarningIndicatorPtr warningIndicator, QObject* parent)
    : QObject(parent), warningIndicator_(warningIndicator)
{
    connect(netManager_.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void NetworkHelper::makeRequest(const QString urlStr)
{
    netManager_->get(QNetworkRequest(QUrl(urlStr)));
}

void NetworkHelper::replyFinished(QNetworkReply *reply)
{
    if (!reply->bytesAvailable())
    {
        qDebug() << "Not connected to internet";
        return;
    }
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network Failure " << reply->errorString();
        warningIndicator_->setWarning(reply->errorString(), WarningType::NETWORK);
        return;
    }
    const auto response = reply->readAll();
    reply->abort();
    reply->close();
    reply->deleteLater();
    emit(networkDataReady(response));
}
