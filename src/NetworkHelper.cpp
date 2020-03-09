#include "NetworkHelper.hpp"

#include <QNetworkReply>
#include <QEventLoop>
#include <QtNetwork>

NetworkHelper::NetworkHelper(const std::shared_ptr<QNetworkAccessManager>& netManager, WarningIndicatorPtr warningIndicator,
    QObject* parent)
    : QObject(parent), netManager_(netManager), warningIndicator_(warningIndicator)
{
    qDebug() << "NetworkHelper C-TOR";

    connect(netManager_.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void NetworkHelper::makeRequest(const QString urlStr)
{
    qDebug() << "makingReq";

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
