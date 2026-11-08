#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "CampusLifeGame.hpp"
#include "ConsoleUI.hpp"
#include "TemporaryDirectory.hpp"

class ConsoleUITest : public ::testing::Test {
protected:
    TemporaryDirectory temporary;
};

TEST_F(ConsoleUITest, InvalidMenuValuesAndTrailingCharactersAreRetried) {
    std::istringstream input{ "abc\n2x\n99999999999999999999\n9\n3\n3\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 2);
    EXPECT_NE(output.str().find("Invalid input"), std::string::npos);
    EXPECT_NE(output.str().find("WEEK 1 RESULT"), std::string::npos);
}

TEST_F(ConsoleUITest, RejectedPlanIsReenteredBeforeAnyActionRuns) {
    std::istringstream input{ "3\n3\n3\n3\n3\nn\n1\n2\n3\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 2);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 0);
    EXPECT_EQ(game.getStudent().getCourse(1).getProgress(), 40);
    EXPECT_NE(output.str().find("Plan not confirmed"), std::string::npos);
}

TEST_F(ConsoleUITest, EndOfInputDuringPlanningDoesNotSimulateWeek) {
    std::istringstream input{ "1\n1\n3\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 1);
    EXPECT_EQ(game.getStudent().getEnergy(), 100);
    EXPECT_EQ(game.getStudent().getCourse(0).getProgress(), 0);
    EXPECT_EQ(game.getPlan()[0].activity, nullptr);
}

TEST_F(ConsoleUITest, RejectedPlanIsDiscardedEvenIfInputEndsImmediately) {
    std::istringstream input{ "3\n3\n3\n3\n3\nn\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 1);
    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        EXPECT_EQ(game.getPlan()[day].activity, nullptr);
    }
}

TEST_F(ConsoleUITest, ConfirmationRejectsExtraCharactersAndAcceptsUppercase) {
    std::istringstream input{ "3\n3\n3\n3\n3\nY extra\nY\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 2);
    EXPECT_NE(output.str().find("Invalid input. Enter Y or N"), std::string::npos);
}

TEST_F(ConsoleUITest, DailyStudyResultsShowEachDaysOwnProgress) {
    std::istringstream input{ "1\n1\n1\n1\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_NE(output.str().find("C [0] -> C [40]"), std::string::npos);
    EXPECT_NE(output.str().find("C [40] -> C [80]"), std::string::npos);
    EXPECT_NE(output.str().find("Weekend Energy: 80 -> 100"), std::string::npos);
}

TEST_F(ConsoleUITest, FailedActionsShowRequiredAndAvailableResources) {
    std::istringstream input{ "1\n1\n2\n4\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{ "Ara", 10000, 0 } };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_NE(output.str().find("Need energy: 25"), std::string::npos);
    EXPECT_NE(output.str().find("Need energy: 40"), std::string::npos);
    EXPECT_NE(output.str().find("Need money: 25000"), std::string::npos);
    EXPECT_NE(output.str().find("Money: 10000"), std::string::npos);
    EXPECT_EQ(game.getStudent().getEnergy(), 55);
    EXPECT_FALSE(game.getNextBurnout());
}

TEST_F(ConsoleUITest, NextWeekShowsBurnoutBeforePlanning) {
    std::istringstream input{ "1\n1\n3\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{ "Ara", 50000, 25 } };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_TRUE(game.getNextBurnout());
    EXPECT_NE(output.str().find("Study: x0.5"), std::string::npos);
    EXPECT_NE(output.str().find("Energy: 25 -> 0"), std::string::npos);
    EXPECT_NE(output.str().find("Next week burnout"), std::string::npos);
}

TEST_F(ConsoleUITest, SemesterEndsAfterSixteenCompleteWeeks) {
    std::string choices;
    for (int week = 0; week < 16; ++week) {
        choices += "3\n3\n3\n3\n3\ny\n";
    }
    std::istringstream input{ choices };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 17);
    EXPECT_NE(output.str().find("SEMESTER RESULT"), std::string::npos);
    EXPECT_NE(output.str().find("WEEK 16 RESULT"), std::string::npos);
    EXPECT_EQ(output.str().find("WEEK 17 PLAN"), std::string::npos);
}

TEST_F(ConsoleUITest, ScholarshipAppearsAfterAllFiveDaysAndWeekend) {
    Student student;
    for (int course = 0; course < student.getCourseCount(); ++course) {
        student.getCourse(course) += 100;
        student.getCourse(course) += 100;
        if (course != 0) student.getCourse(course) += 100;
    }
    student.getCourse(0).setProgress(80);
    CampusLifeGame game{ student };
    std::istringstream input{ "1\n1\n2\n3\n4\n2\ny\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    EXPECT_EQ(game.getStudent().getMoney(), 105000);
    EXPECT_EQ(game.getStudent().getEnergy(), 70);
    EXPECT_NE(output.str().find("FRI"), std::string::npos);
    EXPECT_NE(output.str().find("Weekend Energy"), std::string::npos);
    EXPECT_NE(output.str().find("SCHOLARSHIP"), std::string::npos);
}

TEST_F(ConsoleUITest, MainMenuCanExitWithoutStartingGame) {
    std::istringstream input{ "0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.run();

    EXPECT_NE(output.str().find("CAMPUS LIFE"), std::string::npos);
    EXPECT_EQ(output.str().find("WEEK 1 PLAN"), std::string::npos);
}
