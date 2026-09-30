#include <type_traits>

#include <gtest/gtest.h>

#include "Activity.hpp"
#include "ChickenActivity.hpp"
#include "PartTimeJobActivity.hpp"
#include "SleepActivity.hpp"
#include "Student.hpp"
#include "StudyActivity.hpp"

TEST(ActivityInheritanceTest, FourActionsInheritCommonNames) {
    static_assert(std::is_base_of_v<Activity, StudyActivity>);
    static_assert(std::is_base_of_v<Activity, PartTimeJobActivity>);
    static_assert(std::is_base_of_v<Activity, SleepActivity>);
    static_assert(std::is_base_of_v<Activity, ChickenActivity>);

    EXPECT_EQ(StudyActivity{ 0 }.getName(), "Study");
    EXPECT_EQ(PartTimeJobActivity{}.getName(), "Part-time job");
    EXPECT_EQ(SleepActivity{}.getName(), "Sleep");
    EXPECT_EQ(ChickenActivity{}.getName(), "Eat chicken");
}

TEST(ActivityInheritanceTest, StudyChangesOnlyTheSelectedCourse) {
    Student student{ "Ara", 50000, 100 };
    StudyActivity study{ 1 };

    ASSERT_TRUE(study.execute(student));

    EXPECT_EQ(student.getEnergy(), 75);
    EXPECT_EQ(student.getCourse(0).getProgress(), 0);
    EXPECT_EQ(student.getCourse(1).getProgress(), 40);
}

TEST(ActivityInheritanceTest, StudyFailureLeavesStudentUnchanged) {
    Student student{ "Ara", 50000, 20 };

    EXPECT_FALSE(StudyActivity{ 0 }.execute(student));
    EXPECT_FALSE(StudyActivity{ 99 }.execute(student));
    EXPECT_EQ(student.getEnergy(), 20);
    EXPECT_EQ(student.getCourse(0).getProgress(), 0);
}

TEST(ActivityInheritanceTest, PartTimeJobEarnsMoneyOnlyWithEnoughEnergy) {
    PartTimeJobActivity work;
    Student capable{ "Ara", 50000, 40 };
    Student tired{ "Bo", 50000, 39 };

    EXPECT_TRUE(work.execute(capable));
    EXPECT_EQ(capable.getEnergy(), 0);
    EXPECT_EQ(capable.getMoney(), 90000);

    EXPECT_FALSE(work.execute(tired));
    EXPECT_EQ(tired.getEnergy(), 39);
    EXPECT_EQ(tired.getMoney(), 50000);
}

TEST(ActivityInheritanceTest, SleepRestoresEnergyWithoutExceedingMaximum) {
    Student student{ "Ara", 50000, 95 };

    EXPECT_TRUE(SleepActivity{}.execute(student));

    EXPECT_EQ(student.getEnergy(), 100);
    EXPECT_EQ(student.getMoney(), 50000);
}

TEST(ActivityInheritanceTest, ChickenCostsMoneyOnlyWhenAffordable) {
    ChickenActivity chicken;
    Student buyer{ "Ara", 25000, 80 };
    Student shortOnMoney{ "Bo", 24999, 80 };

    EXPECT_TRUE(chicken.execute(buyer));
    EXPECT_EQ(buyer.getMoney(), 0);
    EXPECT_EQ(buyer.getEnergy(), 100);

    EXPECT_FALSE(chicken.execute(shortOnMoney));
    EXPECT_EQ(shortOnMoney.getMoney(), 24999);
    EXPECT_EQ(shortOnMoney.getEnergy(), 80);
}
