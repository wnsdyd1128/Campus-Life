#include <type_traits>

#include <gtest/gtest.h>

#include "Activity.hpp"
#include "CampusLifeGame.hpp"
#include "ChickenActivity.hpp"
#include "PartTimeJobActivity.hpp"
#include "SleepActivity.hpp"
#include "Student.hpp"
#include "StudyActivity.hpp"

static_assert(std::is_abstract_v<Activity>);
static_assert(std::has_virtual_destructor_v<Activity>);
static_assert(!std::is_abstract_v<StudyActivity>);
static_assert(!std::is_abstract_v<PartTimeJobActivity>);
static_assert(!std::is_abstract_v<SleepActivity>);
static_assert(!std::is_abstract_v<ChickenActivity>);

namespace {
class DestructionProbe : public Activity {
public:
    explicit DestructionProbe(bool& destroyed) : destroyed{ destroyed } {}
    ~DestructionProbe() override { destroyed = true; }
    std::string getName() const override { return "Probe"; }
    bool execute(Student&, bool, int) const override { return true; }

private:
    bool& destroyed;
};
}

TEST(ActivityPolymorphismTest, BasePointersDispatchEachConcreteAction) {
    Student student{ "Ara", 50000, 100 };
    StudyActivity study;
    PartTimeJobActivity work;
    SleepActivity sleep;
    ChickenActivity chicken;
    const Activity* actions[]{ &study, &work, &sleep, &chicken };

    EXPECT_EQ(actions[0]->getName(), "Study");
    ASSERT_TRUE(actions[0]->execute(student, false, 1));
    EXPECT_EQ(student.getCourse(1).getProgress(), 40);
    EXPECT_EQ(student.getEnergy(), 75);

    EXPECT_EQ(actions[1]->getName(), "Part-time job");
    ASSERT_TRUE(actions[1]->execute(student));
    EXPECT_EQ(student.getMoney(), 90000);
    EXPECT_EQ(student.getEnergy(), 35);

    EXPECT_EQ(actions[2]->getName(), "Sleep");
    ASSERT_TRUE(actions[2]->execute(student));
    EXPECT_EQ(student.getEnergy(), 45);

    EXPECT_EQ(actions[3]->getName(), "Eat chicken");
    ASSERT_TRUE(actions[3]->execute(student));
    EXPECT_EQ(student.getMoney(), 65000);
    EXPECT_EQ(student.getEnergy(), 75);
}

TEST(ActivityPolymorphismTest, DeletingThroughBaseRunsDerivedDestructor) {
    bool destroyed{ false };
    Activity* activity = new DestructionProbe{ destroyed };

    delete activity;

    EXPECT_TRUE(destroyed);
}

TEST(ActivityPolymorphismTest, BurnoutHalvesStudyPointsWithIntegerRounding) {
    StudyActivity study;
    const Activity& activity{ study };
    const int expectedPoints[]{ 20, 12, 10 };

    for (int grade = 0; grade < 3; ++grade) {
        Student student;
        for (int promotion = 0; promotion < grade; ++promotion) {
            student.getCourse(0) += 100;
        }

        ASSERT_TRUE(activity.execute(student, true, 0));

        EXPECT_EQ(student.getCourse(0).getProgress(), expectedPoints[grade]);
        EXPECT_EQ(student.getEnergy(), 75);
    }
}

TEST(ActivityPolymorphismTest, InvalidStudyCourseLeavesEnergyAndProgressUnchanged) {
    Student student;
    StudyActivity study;
    const Activity& activity{ study };

    EXPECT_FALSE(activity.execute(student, false, -1));
    EXPECT_FALSE(activity.execute(student, false, student.getCourseCount()));
    EXPECT_EQ(student.getEnergy(), 100);
    EXPECT_EQ(student.getCourse(0).getProgress(), 0);
}

TEST(ActivityPolymorphismTest, BurnoutStudyCanAdvanceTheGrade) {
    Student student;
    student.getCourse(0) += 100;
    student.getCourse(0).setProgress(90);
    StudyActivity study;

    ASSERT_TRUE(study.execute(student, true, 0));

    EXPECT_EQ(student.getCourse(0).getGrade(), "A");
    EXPECT_EQ(student.getCourse(0).getProgress(), 0);
}

TEST(WeekSimulationTest, PlanningSharesOneStudyActionWithoutChangingStudent) {
    CampusLifeGame game{ Student{ "Ara", 50000, 100 } };

    game.planStudy(0, 0);
    game.planStudy(1, 1);

    ASSERT_NE(game.getPlan()[0].activity, nullptr);
    EXPECT_EQ(game.getPlan()[0].activity, game.getPlan()[1].activity);
    EXPECT_EQ(game.getPlan()[0].courseIndex, 0);
    EXPECT_EQ(game.getPlan()[1].courseIndex, 1);
    EXPECT_EQ(game.getStudent().getEnergy(), 100);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 0);
}

TEST(WeekSimulationTest, MixedPlanRunsInOrderBeforeWeekendRecovery) {
    CampusLifeGame game{ Student{ "Ara", 50000, 100 } };
    game.planStudy(0, 0);
    game.planStudy(1, 1);
    game.planChicken(2);
    game.planWork(3);
    game.planSleep(4);

    const WeekResult result{ game.simulateWeek() };

    for (bool succeeded : result.succeeded) {
        EXPECT_TRUE(succeeded);
    }
    EXPECT_EQ(game.getStudent().getMoney(), 65000);
    EXPECT_EQ(game.getStudent().getEnergy(), 85);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 40);
    EXPECT_EQ(game.getStudent().getCourse(1).getProgress(), 40);
    EXPECT_EQ(game.getWeek(), 2);
    EXPECT_FALSE(game.getNextBurnout());
}

TEST(WeekSimulationTest, FailedActionsConsumeTheirDayAndLaterDaysStillRun) {
    CampusLifeGame game{ Student{ "Ara", 24999, 24 } };
    game.planStudy(0, 0);
    game.planChicken(1);
    game.planSleep(2);
    game.planStudy(3, 0);
    game.planWork(4);

    const WeekResult result{ game.simulateWeek() };

    EXPECT_FALSE(result.succeeded[0]);
    EXPECT_FALSE(result.succeeded[1]);
    EXPECT_TRUE(result.succeeded[2]);
    EXPECT_TRUE(result.succeeded[3]);
    EXPECT_FALSE(result.succeeded[4]);
    EXPECT_EQ(game.getStudent().getMoney(), 24999);
    EXPECT_EQ(game.getStudent().getEnergy(), 44);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 40);
    EXPECT_FALSE(game.getNextBurnout());
}

TEST(WeekSimulationTest, RecoveryDoesNotCancelBurnoutReservedAtZeroEnergy) {
    CampusLifeGame game{ Student{ "Ara", 50000, 25 } };
    game.planStudy(0, 0);
    for (int day = 1; day < WeeklyPlan::DAY_COUNT; ++day) {
        game.planSleep(day);
    }

    game.simulateWeek();

    EXPECT_EQ(game.getStudent().getEnergy(), 75);
    EXPECT_FALSE(game.getCurrentBurnout());
    EXPECT_TRUE(game.getNextBurnout());
}

TEST(WeekSimulationTest, BurnoutAppliesOnlyToTheFollowingWeekUnlessReservedAgain) {
    CampusLifeGame game{ Student{ "Ara", 50000, 25 } };
    game.planStudy(0, 0);
    for (int day = 1; day < WeeklyPlan::DAY_COUNT; ++day) {
        game.planSleep(day);
    }
    game.simulateWeek();

    game.simulateWeek();

    EXPECT_TRUE(game.getCurrentBurnout());
    EXPECT_FALSE(game.getNextBurnout());
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 60);

    game.simulateWeek();

    EXPECT_FALSE(game.getCurrentBurnout());
    EXPECT_EQ(game.getStudent().getCourse(0).getGrade(), "B");
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 0);
}

TEST(WeekSimulationTest, RepeatedBurnoutKeepsHalfPointsWithoutStacking) {
    CampusLifeGame game{ Student{ "Ara", 50000, 25 } };
    game.planStudy(0, 0);
    game.simulateWeek();
    game.planSleep(0);
    game.planStudy(1, 0);
    game.planSleep(2);
    game.planSleep(3);
    game.planWork(4);

    game.simulateWeek();
    ASSERT_TRUE(game.getNextBurnout());
    game.simulateWeek();

    EXPECT_TRUE(game.getCurrentBurnout());
    EXPECT_TRUE(game.getNextBurnout());
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 80);
    EXPECT_EQ(game.getStudent().getEnergy(), 35);
}

TEST(WeekSimulationTest, EmptyDaysDoNotReserveBurnoutAtZeroEnergy) {
    CampusLifeGame game{ Student{ "Ara", 50000, 0 } };

    const WeekResult result{ game.simulateWeek() };

    for (bool succeeded : result.succeeded) {
        EXPECT_FALSE(succeeded);
    }
    EXPECT_EQ(game.getStudent().getEnergy(), 35);
    EXPECT_FALSE(game.getNextBurnout());
}

TEST(WeekSimulationTest, FailedStudyAtZeroEnergyDoesNotReserveBurnout) {
    CampusLifeGame game{ Student{ "Ara", 50000, 0 } };
    game.planStudy(0, 0);

    const WeekResult result{ game.simulateWeek() };

    EXPECT_FALSE(result.succeeded[0]);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 0);
    EXPECT_EQ(game.getStudent().getEnergy(), 35);
    EXPECT_FALSE(game.getNextBurnout());
}

TEST(WeekSimulationTest, WeekendRecoveryDoesNotExceedMaximumEnergy) {
    CampusLifeGame game{ Student{ "Ara", 50000, 95 } };

    game.simulateWeek();

    EXPECT_EQ(game.getStudent().getEnergy(), 100);
}

TEST(WeekSimulationTest, FinishingAllCoursesStillRunsRemainingDaysAndWeekend) {
    Student student;
    for (int course = 0; course < student.getCourseCount(); ++course) {
        student.getCourse(course) += 100;
        student.getCourse(course) += 100;
        if (course != 0) {
            student.getCourse(course) += 100;
        }
    }
    student.getCourse(0).setProgress(80);
    CampusLifeGame game{ student };
    game.planStudy(0, 0);
    game.planWork(1);
    game.planSleep(2);
    game.planChicken(3);
    game.planWork(4);

    const WeekResult result{ game.simulateWeek() };

    for (bool succeeded : result.succeeded) {
        EXPECT_TRUE(succeeded);
    }
    EXPECT_EQ(game.getStudent().getCourse(0).getGrade(), "A+");
    EXPECT_EQ(game.getStudent().getMoney(), 105000);
    EXPECT_EQ(game.getStudent().getEnergy(), 70);
}
