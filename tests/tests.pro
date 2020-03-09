QT += testlib quick sql network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../src \
    googletest/googletest \
    googletest/googlemock \
    googletest/googletest/include \
    googletest/googletest/include/gtest \
    googletest/googlemock/include \
    googletest/googlemock/include/gmock

LIBS += -Lgoogletest/googletest_build/lib -lgtest

SOURCES +=  \
    googletest/googletest/src/gtest-all.cc \
    googletest/googlemock/src/gmock-all.cc \
    ../src/models/ActivitiesModel.cpp \
    ../src/Controller.cpp \
    ../src/CurrencyService.cpp \
    ../src/models/FriendsModel.cpp \
    ../src/models/GroupDetails.cpp \
    ../src/models/GroupModel.cpp \
    ../src/NetworkHelper.cpp \
    ../src/RegistrationService.cpp \
    ../src/SqlConnectionHelper.cpp \
    ../src/UserValidation.cpp \
    ../src/WarningIndicator.cpp \
    WarningIndicatorTests.cpp \
    UserValidationTest.cpp \
    RegistrationServiceTest.cpp \
    CurrentStateProviderTest.cpp \
    GroupModelTest.cpp \
    GroupDetailsTest.cpp \
    FriendsModelTest.cpp \
    ControllerTest.cpp \
    main.cpp

HEADERS += \
    ../src/models/ActivitiesModel.hpp \
    ../src/AppProperties.hpp \
    ../src/models/BaseQSqlQueryModel.hpp \
    ../src/Controller.hpp \
    ../src/Currency.hpp \
    ../src/CurrencyService.hpp \
    ../src/CurrentStateProvider.hpp \
    ../src/models/FriendsModel.hpp \
    ../src/models/GroupDetails.hpp \
    ../src/models/GroupModel.hpp \
    ../src/NetworkHelper.hpp \
    ../src/RegistrationService.hpp \
    ../src/UserValidation.hpp \
    ../src/ValidationBase.hpp \
    ../src/WarningIndicator.hpp


