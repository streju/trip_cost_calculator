#include <gtest.h>
#include <QSignalSpy>

#include "CurrentStateProvider.hpp"

TEST(CurrentStateProviderTest, shouldProperlySetCurrentGroup)
{
    std::unique_ptr<CurrentStateProvider> sut = std::make_unique<CurrentStateProvider>();
    QString currentGroup{"DUMMY_GROUP"};

    ASSERT_TRUE(sut->getCurrentGroup().isEmpty());

    QSignalSpy spy(sut.get(), SIGNAL(currentGroupChange(const QString&)));
    sut->setCurrentGroup(currentGroup);

    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(sut->property("groupname").toString(), currentGroup);
    ASSERT_EQ(sut->getCurrentGroup(), currentGroup);
}

TEST(CurrentStateProviderTest, shouldProperlySetCurrentLoggedUser)
{
    std::unique_ptr<CurrentStateProvider> sut = std::make_unique<CurrentStateProvider>();
    QString currentUser{"DUMMY_USERDUMMY_"};

    ASSERT_TRUE(sut->getCurrentLoggedUser().isEmpty());

    QSignalSpy spy(sut.get(), SIGNAL(currentLoggedUserChange(const QString&)));
    sut->setCurrentLoggedUser(currentUser);

    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(spy.takeFirst().at(0).toString(), currentUser);
    ASSERT_EQ(sut->property("username").toString(), currentUser);
    ASSERT_EQ(sut->getCurrentLoggedUser(), currentUser);
}
