#ifndef NETWORKHELPER_HPP
#define NETWORKHELPER_HPP

#include <QObject>
#include <QNetworkAccessManager>

#include "WarningIndicator.hpp"

class NetworkHelper : public QObject
{
public:
    NetworkHelper(WarningIndicatorPtr warningIndicator, QObject* parent = nullptr);
    void makeRequest(const QString urlStr);
signals:
    void networkDataReady(QByteArray);
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    Q_OBJECT

    WarningIndicatorPtr warningIndicator_;
    std::unique_ptr<QNetworkAccessManager> netManager_{std::make_unique<QNetworkAccessManager>(this)};
};

#endif // NETWORKHELPER_HPP
