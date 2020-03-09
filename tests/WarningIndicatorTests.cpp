#include <gtest.h>
#include <QSignalSpy>
#include <tuple>

#include "WarningIndicator.hpp"

using TestParams = std::tuple<const char*, const char *, WarningType>;

class WarningIndicatorTest : public testing::Test, public ::testing::WithParamInterface<TestParams>
{
public:
    WarningIndicatorTest() {}
protected:
    std::unique_ptr<WarningIndicator> sut_{std::make_unique<WarningIndicator>()};
    QString warning_{"DUMMY_WARNING"};
};

TEST_P(WarningIndicatorTest, shouldProperlySetWarnings)
{
    const auto propertyName = std::get<0>(GetParam());
    const auto signal = std::get<1>(GetParam());
    const auto warningType = std::get<2>(GetParam());

    QSignalSpy spy(sut_.get(), signal);
    sut_->setWarning(warning_, warningType);

    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(sut_->property(propertyName).toString(), warning_);
}

INSTANTIATE_TEST_SUITE_P(WarningIndicatorTestWithParams, WarningIndicatorTest,
    testing::Values(
        std::make_tuple("validationWarning", SIGNAL(validationWarningChange()), Warning::Type::VALIDATION),
        std::make_tuple("registrationWarning", SIGNAL(registrationWarningChange()), WarningType::REGISTRATION),
        std::make_tuple("newBillCreationWarning", SIGNAL(newBillCreationWarningChange()), WarningType::NEW_BILL_CREATION),
        std::make_tuple("groupCreationWarning", SIGNAL(groupCreationWarningChange()), WarningType::GROUP_CREATION),
        std::make_tuple("groupOperationsWarning", SIGNAL(groupOperationsWarningChange()), WarningType::GROUP_OPERATIONS),
        std::make_tuple("networkWarning", SIGNAL(networkWarningChange()), WarningType::NETWORK)));
