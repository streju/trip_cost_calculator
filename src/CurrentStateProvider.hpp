#ifndef CURRENTSTATEPROVIDER_HPP
#define CURRENTSTATEPROVIDER_HPP

#include <memory>

#include <QObject>
#include <QString>

#include "Interfaces/ICurrentStateProvider.hpp"

class CurrentStateProvider : public QObject, public ICurrentStateProvider
{
public:
    explicit CurrentStateProvider(QObject *parent = nullptr) : QObject(parent) {}
    void setCurrentGroup(const QString& group) override {currentGroup_ = group; emit currentGroupChange(group);}
    void setCurrentLoggedUser(const QString& user) override {currentLoggedUser_ = user; emit currentLoggedUserChange(user);}
    QString getCurrentGroup() const override {return currentGroup_;}
    QString getCurrentLoggedUser() const override {return currentLoggedUser_;}
signals:
    void currentLoggedUserChange(const QString& username);
    void currentGroupChange(const QString& groupname);
private:
    Q_OBJECT
    Q_PROPERTY(QString username READ getCurrentLoggedUser WRITE setCurrentLoggedUser NOTIFY currentLoggedUserChange)
    Q_PROPERTY(QString groupname READ getCurrentGroup WRITE setCurrentGroup NOTIFY currentGroupChange)

    QString currentLoggedUser_;
    QString currentGroup_;
};

#endif // CURRENTSTATEPROVIDER_HPP
