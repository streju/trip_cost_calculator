#include "WarningIndicator.hpp"

WarningIndicator::WarningIndicator(QObject *parent) : QObject(parent)
{
}

void WarningIndicator::setWarning(const QString &warning, WarningType type)
{
    qDebug() << "Going to set warning to " << warning;
    warnings_[type] = warning;

    switch(type)
    {
        case WarningType::VALIDATION:
            emit validationWarningChange();
            return;
        case WarningType::REGISTRATION:
            emit registrationWarningChange();
            return;
        case WarningType::NEW_BILL_CREATION:
            emit newBillCreationWarningChange();
            return;
        case WarningType::GROUP_CREATION:
            emit groupCreationWarningChange();
            return;
        case WarningType::GROUP_OPERATIONS:
            emit groupOperationsWarningChange();
            return;
        case WarningType::NETWORK:
            emit networkWarningChange();
            return;
    }

    qWarning() << "Unsupported warning type!";
}

QString WarningIndicator::getValidationWarning()
{
    return getWarning(WarningType::VALIDATION);
}

QString WarningIndicator::getRegistrationWarning()
{
    return getWarning(WarningType::REGISTRATION);
}

QString WarningIndicator::getNBCreationWarning()
{
    return getWarning(WarningType::NEW_BILL_CREATION);
}

QString WarningIndicator::getGroupCreationWarning()
{
    return getWarning(WarningType::GROUP_CREATION);
}

QString WarningIndicator::getGroupOperationsWarning()
{
    return getWarning(WarningType::GROUP_OPERATIONS);
}

QString WarningIndicator::getNetworkWarning()
{
    return getWarning(WarningType::NETWORK);
}

QString WarningIndicator::getWarning(WarningType type)
{
    if (warnings_.find(type) == warnings_.end())
    {
        return "";
    }
    return warnings_.at(type);
}
