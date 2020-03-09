QT += quick sql network

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src

SOURCES += \
        src/models/ActivitiesModel.cpp \
        src/Controller.cpp \
        src/CurrencyService.cpp \
        src/models/FriendsModel.cpp \
        src/models/GroupDetails.cpp \
        src/models/GroupModel.cpp \
        src/NetworkHelper.cpp \
        src/RegistrationService.cpp \
        src/SqlConnectionHelper.cpp \
        src/UserValidation.cpp \
        src/WarningIndicator.cpp \
        src/main.cpp

RESOURCES += src/qml.qrc \
    src/media.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/models/ActivitiesModel.hpp \
    src/AppProperties.hpp \
    src/models/BaseQSqlQueryModel.hpp \
    src/Controller.hpp \
    src/Currency.hpp \
    src/CurrencyService.hpp \
    src/CurrentStateProvider.hpp \
    src/models/FriendsModel.hpp \
    src/models/GroupDetails.hpp \
    src/models/GroupModel.hpp \
    src/NetworkHelper.hpp \
    src/RegistrationService.hpp \
    src/UserValidation.hpp \
    src/ValidationBase.hpp \
    src/WarningIndicator.hpp \
    src/QSqlQueryCreator.hpp \
    src/QSqlQueryWrapper.hpp \
    src/Interfaces/IWarningIndicator.hpp

DISTFILES +=

QMAKE_CXXFLAGS+=-std=c++17
