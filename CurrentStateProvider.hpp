#ifndef CURRENTSTATEPROVIDER_HPP
#define CURRENTSTATEPROVIDER_HPP

#include <memory>

#include <QObject>
#include <QString>

class CurrentStateProvider : public QObject
{
public:
    explicit CurrentStateProvider(QObject *parent) : QObject(parent) {}
    void setCurrentGroup(const QString& group) {currentGroup_ = group; emit currentGroupChange(group);}
    void setCurrentLoggedUser(const QString& user) {currentLoggedUser_ = user; emit currentLoggedUserChange(user);}
    QString getCurrentGroup() const {return currentGroup_;}
    QString getCurrentLoggedUser() const {return currentLoggedUser_;}
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

using CurrentStateProviderPtr = CurrentStateProvider* const;

#endif // CURRENTSTATEPROVIDER_HPP
