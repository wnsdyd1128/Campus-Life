#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "CampusLifeGame.hpp"
#include "ConsoleUI.hpp"
#include "LogManager.hpp"
#include "PersistenceResult.hpp"
#include "SaveManager.hpp"
#include "TemporaryDirectory.hpp"

class PersistenceTest : public ::testing::Test {
protected:
    TemporaryDirectory temporary;
};

TEST_F(PersistenceTest, LatestSequentialSaveRestoresAllGameFieldsAndCourseNames) {
    const std::string names[]{ "Object \"Oriented\" Programming", "Data Structure", "Computer Architecture" };
    Student student{ "Ara \"Kim\"", 25000, 25, names, 3 };
    student.getCourse(0) += 100;
    student.getCourse(0).setProgress(37);
    student.getCourse(1) += 100;
    student.getCourse(1) += 100;
    student.getCourse(1).setProgress(55);
    CampusLifeGame original{ student };
    original.planStudy(0, 0);
    original.simulateWeek();
    SaveManager saves{ temporary.path() / "saves" / "save.txt" };
    ASSERT_EQ(saves.save(original), PersistenceResult::Ok);

    original.planSleep(0);
    original.simulateWeek();
    ASSERT_EQ(saves.save(original), PersistenceResult::Ok);
    CampusLifeGame restored{ Student{ "Different", 1, 1 } };
    restored.planWork(2);
    ASSERT_EQ(saves.load(restored), PersistenceResult::Ok);

    EXPECT_EQ(restored.getWeek(), 3);
    EXPECT_TRUE(restored.getCurrentBurnout());
    EXPECT_FALSE(restored.getNextBurnout());
    EXPECT_EQ(restored.getStudent().getName(), "Ara \"Kim\"");
    EXPECT_EQ(restored.getStudent().getMoney(), original.getStudent().getMoney());
    EXPECT_EQ(restored.getStudent().getEnergy(), original.getStudent().getEnergy());
    EXPECT_EQ(restored.getStudent().getCourseCount(), 3);
    for (int index = 0; index < 3; ++index) {
        EXPECT_EQ(restored.getStudent().getCourse(index).getName(), names[index]);
        EXPECT_EQ(restored.getStudent().getCourse(index).getGrade(),
                  original.getStudent().getCourse(index).getGrade());
        EXPECT_EQ(restored.getStudent().getCourse(index).getProgress(),
                  original.getStudent().getCourse(index).getProgress());
    }
    EXPECT_EQ(restored.getPlan()[2].activity, nullptr);
}

TEST_F(PersistenceTest, ReservedBurnoutSurvivesSavingAndAppliesOnResume) {
    CampusLifeGame original{ Student{ "Ara", 50000, 25 } };
    original.planStudy(0, 0);
    original.simulateWeek();
    SaveManager saves{ temporary.path() / "save.txt" };
    ASSERT_EQ(saves.save(original), PersistenceResult::Ok);
    CampusLifeGame restored{ Student{} };
    ASSERT_EQ(saves.load(restored), PersistenceResult::Ok);
    ASSERT_TRUE(restored.getNextBurnout());
    restored.planStudy(0, 0);

    restored.simulateWeek();

    EXPECT_TRUE(restored.getCurrentBurnout());
    EXPECT_EQ(restored.getStudent().getCourse(0).getProgress(), 60);
}

TEST_F(PersistenceTest, MissingSaveDoesNotChangeCurrentGameOrPlan) {
    SaveManager saves{ temporary.path() / "missing.txt" };
    CampusLifeGame game{ Student{ "Keep", 1234, 44 } };
    game.planWork(0);
    const Activity* oldAction{ game.getPlan()[0].activity };

    EXPECT_EQ(saves.load(game), PersistenceResult::NotFound);

    EXPECT_EQ(game.getStudent().getName(), "Keep");
    EXPECT_EQ(game.getStudent().getMoney(), 1234);
    EXPECT_EQ(game.getStudent().getEnergy(), 44);
    EXPECT_EQ(game.getWeek(), 1);
    EXPECT_EQ(game.getPlan()[0].activity, oldAction);
}

TEST_F(PersistenceTest, TruncatedSaveIsRejectedWithoutChangingCurrentGame) {
    const auto path{ temporary.path() / "save.txt" };
    SaveManager saves{ path };
    CampusLifeGame source{ Student{} };
    ASSERT_EQ(saves.save(source), PersistenceResult::Ok);
    { std::ofstream out{ path, std::ios::app }; out << "CAMPUS_LIFE_SAVE 1\n"; }
    CampusLifeGame target{ Student{ "Keep", 1234, 44 } };
    target.planWork(0);
    const Activity* oldAction{ target.getPlan()[0].activity };

    EXPECT_EQ(saves.load(target), PersistenceResult::InvalidData);

    EXPECT_EQ(target.getStudent().getName(), "Keep");
    EXPECT_EQ(target.getStudent().getMoney(), 1234);
    EXPECT_EQ(target.getPlan()[0].activity, oldAction);
}

TEST_F(PersistenceTest, InvalidGradeOrResourceValuesAreRejected) {
    const auto path{ temporary.path() / "bad.txt" };
    { std::ofstream out{ path }; out << "CAMPUS_LIFE_SAVE 1\n1 0 0 -5 101 \"Ara\" 3\n"
                                     "\"A\" \"Z\" 12\n\"B\" \"C\" 0\n\"C\" \"C\" 0\nEND\n"; }
    SaveManager saves{ path };
    CampusLifeGame target{ Student{ "Keep", 1234, 44 } };

    EXPECT_EQ(saves.load(target), PersistenceResult::InvalidData);

    EXPECT_EQ(target.getStudent().getMoney(), 1234);
    EXPECT_EQ(target.getStudent().getEnergy(), 44);
}

TEST_F(PersistenceTest, EachInvalidSaveFieldIsRejectedWithoutChangingGame) {
    const std::vector<std::string> badRecords{
        "CAMPUS_LIFE_SAVE 2\n1 0 0 1 50 \"A\" 3\n\"a\" \"C\" 0\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n0 0 0 1 50 \"A\" 3\n\"a\" \"C\" 0\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n1 0 2 1 50 \"A\" 3\n\"a\" \"C\" 0\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n1 0 0 1 50 \"A\" 2\n\"a\" \"C\" 0\n\"b\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n1 0 0 1 50 \"A\" 3\n\"a\" \"Z\" 0\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n1 0 0 1 50 \"A\" 3\n\"a\" \"C\" 100\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n",
        "CAMPUS_LIFE_SAVE 1\n1 0 0 1 50 \"A\" 3\n\"a\" \"A+\" 1\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND\n"
    };
    const auto path{ temporary.path() / "invalid.txt" };
    SaveManager saves{ path };
    CampusLifeGame game{ Student{ "Keep", 1234, 44 } };

    for (const std::string& record : badRecords) {
        { std::ofstream out{ path }; out << record; }
        EXPECT_EQ(saves.load(game), PersistenceResult::InvalidData);
        EXPECT_EQ(game.getStudent().getName(), "Keep");
        EXPECT_EQ(game.getStudent().getMoney(), 1234);
        EXPECT_EQ(game.getWeek(), 1);
    }
}

TEST_F(PersistenceTest, SavingAfterValidRecordWithoutFinalNewlineRemainsReadable) {
    const auto path{ temporary.path() / "save.txt" };
    { std::ofstream out{ path }; out << "CAMPUS_LIFE_SAVE 1\n1 0 0 1 50 \"A\" 3\n"
                                     "\"a\" \"C\" 0\n\"b\" \"C\" 0\n\"c\" \"C\" 0\nEND"; }
    SaveManager saves{ path };
    CampusLifeGame original{ Student{ "Ara", 50000, 100 } };
    CampusLifeGame restored{ Student{} };

    ASSERT_EQ(saves.save(original), PersistenceResult::Ok);
    ASSERT_EQ(saves.load(restored), PersistenceResult::Ok);

    EXPECT_EQ(restored.getStudent().getName(), "Ara");
}

TEST_F(PersistenceTest, RefusingToAppendToCorruptHistoryPreservesFile) {
    const auto path{ temporary.path() / "save.txt" };
    { std::ofstream out{ path }; out << "invalid history"; }
    SaveManager saves{ path };
    CampusLifeGame game{ Student{} };

    EXPECT_EQ(saves.save(game), PersistenceResult::InvalidData);

    std::ifstream in{ path };
    std::string content;
    std::getline(in, content);
    EXPECT_EQ(content, "invalid history");
}

TEST_F(PersistenceTest, FailedSaveOrLogDoesNotChangeStudentState) {
    const auto blocker{ temporary.path() / "not-a-directory" };
    { std::ofstream out{ blocker }; out << "keep"; }
    CampusLifeGame game{ Student{ "Ara", 1234, 44 } };
    SaveManager saves{ blocker / "save.txt" };
    LogManager logs{ blocker / "play.log" };

    EXPECT_EQ(saves.save(game), PersistenceResult::IoError);
    EXPECT_EQ(logs.append(1, game), PersistenceResult::IoError);
    EXPECT_EQ(game.getStudent().getMoney(), 1234);
    EXPECT_EQ(game.getStudent().getEnergy(), 44);
    std::ifstream in{ blocker };
    std::string content;
    in >> content;
    EXPECT_EQ(content, "keep");
}

TEST_F(PersistenceTest, WeekLogAppendsWithoutReplacingPreviousEntry) {
    const auto path{ temporary.path() / "logs" / "play.log" };
    LogManager logs{ path };
    CampusLifeGame game{ Student{ "Ara", 50000, 25 } };
    game.planStudy(0, 0);
    game.simulateWeek();
    ASSERT_EQ(logs.append(1, game), PersistenceResult::Ok);
    game.planSleep(0);
    game.simulateWeek();
    ASSERT_EQ(logs.append(2, game), PersistenceResult::Ok);

    std::ifstream in{ path };
    std::string first, second, extra;
    ASSERT_TRUE(static_cast<bool>(std::getline(in, first)));
    ASSERT_TRUE(static_cast<bool>(std::getline(in, second)));
    EXPECT_FALSE(static_cast<bool>(std::getline(in, extra)));
    EXPECT_NE(first.find("Week 1"), std::string::npos);
    EXPECT_NE(second.find("Week 2"), std::string::npos);
    EXPECT_NE(first.find("nextBurnout=1"), std::string::npos);
    EXPECT_NE(second.find("currentBurnout=1"), std::string::npos);
    EXPECT_NE(second.find("Object-Oriented Programming 2"), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleSavesThenLoadsAtTheBeginningOfTheSameWeek) {
    std::istringstream input{ "1\n3\n3\n3\n3\n3\ny\n5\n0\n2\n0\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.run();

    const std::string text{ output.str() };
    EXPECT_NE(text.find("Game saved"), std::string::npos);
    EXPECT_NE(text.find("Game loaded"), std::string::npos);
    const auto firstWeek2 = text.find("===== WEEK 2 =====");
    ASSERT_NE(firstWeek2, std::string::npos);
    EXPECT_NE(text.find("===== WEEK 2 =====", firstWeek2 + 1), std::string::npos);
    std::ifstream in{ temporary.path() / "play.log" };
    std::string line;
    ASSERT_TRUE(static_cast<bool>(std::getline(in, line)));
    EXPECT_FALSE(static_cast<bool>(std::getline(in, line)));
}

TEST_F(PersistenceTest, ConsoleReportsMissingSaveAndReturnsToMenu) {
    std::istringstream input{ "2\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.run();

    EXPECT_NE(output.str().find("No saved game"), std::string::npos);
    EXPECT_EQ(output.str().find("===== WEEK 1 ====="), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleContinuesAfterWeeklyLogCannotBeWritten) {
    const auto blocker{ temporary.path() / "not-a-directory" };
    { std::ofstream out{ blocker }; out << "keep"; }
    std::istringstream input{ "3\n3\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, blocker };

    ui.play(game);

    EXPECT_EQ(game.getWeek(), 2);
    EXPECT_NE(output.str().find("Warning: could not append weekly log"), std::string::npos);
    EXPECT_NE(output.str().find("WEEK 1 RESULT"), std::string::npos);
}
