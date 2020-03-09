#ifndef NETWORKHELPER_HPP
#define NETWORKHELPER_HPP

#include <QObject>
#include <QNetworkAccessManager>

#include "Interfaces/IWarningIndicator.hpp"

class INetworkHelper
{
public:
    virtual void makeRequest(const QString) = 0;
};

class NetworkHelper : public QObject, public INetworkHelper
{
public:
    NetworkHelper(const std::shared_ptr<QNetworkAccessManager>& netManager, WarningIndicatorPtr warningIndicator,
        QObject* parent = nullptr);
    void makeRequest(const QString urlStr) override;
signals:
    void networkDataReady(QByteArray);
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    Q_OBJECT

    std::shared_ptr<QNetworkAccessManager> netManager_;
    WarningIndicatorPtr warningIndicator_;
};

#endif // NETWORKHELPER_HPP
