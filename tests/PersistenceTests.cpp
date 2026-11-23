#include <filesystem>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "CampusLifeGame.hpp"
#include "ConsoleUI.hpp"
#include "LogManager.hpp"
#include "SaveManager.hpp"
#include "TemporaryDirectory.hpp"

class PersistenceTest : public ::testing::Test {
protected:
    TemporaryDirectory temporary;
};

TEST_F(PersistenceTest, ThreeSlotsAreIndependentWhenOneIsOverwritten) {
    const auto path{ temporary.path() / "saves" / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame first{ Student{ "First", 1000, 50 } };
    CampusLifeGame second{ Student{ "Second", 2000, 60 } };
    CampusLifeGame third{ Student{ "Third", 3000, 70 } };
    ASSERT_NO_THROW(saves.save(1, first));
    ASSERT_NO_THROW(saves.save(2, second));
    ASSERT_NO_THROW(saves.save(3, third));
    const auto size{ std::filesystem::file_size(path) };
    ASSERT_EQ(size % 3, 0);
    std::ifstream beforeFile{ path, std::ios::binary };
    const std::string before{ std::istreambuf_iterator<char>{beforeFile}, {} };

    second.planSleep(0);
    second.simulateWeek();
    ASSERT_NO_THROW(saves.save(2, second));
    EXPECT_EQ(std::filesystem::file_size(path), size);
    std::ifstream afterFile{ path, std::ios::binary };
    const std::string after{ std::istreambuf_iterator<char>{afterFile}, {} };
    EXPECT_EQ(before.substr(0, size / 3), after.substr(0, size / 3));
    EXPECT_EQ(before.substr(2 * size / 3), after.substr(2 * size / 3));
    CampusLifeGame restored{ Student{} };
    ASSERT_TRUE(saves.load(1, restored));
    EXPECT_EQ(restored.getStudent().getName(), "First");
    ASSERT_TRUE(saves.load(2, restored));
    EXPECT_EQ(restored.getStudent().getName(), "Second");
    EXPECT_EQ(restored.getWeek(), 2);
    ASSERT_TRUE(saves.load(3, restored));
    EXPECT_EQ(restored.getStudent().getName(), "Third");
}

TEST_F(PersistenceTest, SaveRestoresCoursesBurnoutAndClearsOldPlan) {
    const std::string names[]{ "Object-Oriented Programming 2", "Data Structure", "Computer Architecture" };
    Student student{ "Ara Kim", 25000, 25, names, 3 };
    student.getCourse(0) += 100;
    student.getCourse(0).setProgress(37);
    student.getCourse(1) += 100;
    student.getCourse(1) += 100;
    student.getCourse(1).setProgress(55);
    CampusLifeGame original{ student };
    original.planStudy(0, 0);
    original.simulateWeek();
    SaveManager saves{ temporary.path() / "save.dat" };
    ASSERT_NO_THROW(saves.save(3, original));
    CampusLifeGame restored{ Student{ "Different", 1, 1 } };
    restored.planWork(2);
    ASSERT_TRUE(saves.load(3, restored));
    EXPECT_EQ(restored.getWeek(), 2);
    EXPECT_TRUE(restored.getNextBurnout());
    EXPECT_EQ(restored.getStudent().getName(), "Ara Kim");
    EXPECT_EQ(restored.getStudent().getMoney(), original.getStudent().getMoney());
    EXPECT_EQ(restored.getStudent().getEnergy(), original.getStudent().getEnergy());
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(restored.getStudent().getCourse(i).getName(), names[i]);
        EXPECT_EQ(restored.getStudent().getCourse(i).getGrade(), original.getStudent().getCourse(i).getGrade());
        EXPECT_EQ(restored.getStudent().getCourse(i).getProgress(), original.getStudent().getCourse(i).getProgress());
    }
    EXPECT_EQ(restored.getPlan()[2].activity, nullptr);
    restored.planStudy(0, 0);
    restored.simulateWeek();
    EXPECT_TRUE(restored.getCurrentBurnout());
}

TEST_F(PersistenceTest, MissingEmptyAndInvalidSlotsDoNotChangeGame) {
    SaveManager saves{ temporary.path() / "save.dat" };
    CampusLifeGame game{ Student{ "Keep", 1234, 44 } };
    game.planWork(0);
    const Activity* oldAction{ game.getPlan()[0].activity };
    EXPECT_FALSE(saves.load(1, game));
    EXPECT_THROW(saves.save(0, game), InvalidSaveSlotException);
    EXPECT_THROW(saves.load(4, game), InvalidSaveSlotException);
    ASSERT_NO_THROW(saves.save(1, game));
    EXPECT_FALSE(saves.load(2, game));
    EXPECT_EQ(game.getStudent().getName(), "Keep");
    EXPECT_EQ(game.getPlan()[0].activity, oldAction);
}

TEST_F(PersistenceTest, TruncatedFileIsRejectedAndNotOverwritten) {
    const auto path{ temporary.path() / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame game{ Student{ "Keep", 1234, 44 } };
    ASSERT_NO_THROW(saves.save(1, game));
    std::filesystem::resize_file(path, std::filesystem::file_size(path) - 1);
    const auto size{ std::filesystem::file_size(path) };
    EXPECT_THROW(saves.load(1, game), CorruptedSaveDataException);
    EXPECT_THROW(saves.save(2, game), CorruptedSaveDataException);
    EXPECT_EQ(std::filesystem::file_size(path), size);
    EXPECT_EQ(game.getStudent().getName(), "Keep");
}

TEST_F(PersistenceTest, CorruptSlotDoesNotPreventReadingOtherSlots) {
    const auto path{ temporary.path() / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame first{ Student{ "First", 1000, 50 } };
    CampusLifeGame third{ Student{ "Third", 3000, 70 } };
    ASSERT_NO_THROW(saves.save(1, first));
    ASSERT_NO_THROW(saves.save(3, third));
    const auto slotSize{ std::filesystem::file_size(path) / 3 };
    { std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary };
      file.seekp(static_cast<std::streamoff>(slotSize));
      const std::string corrupt(static_cast<std::size_t>(slotSize), '\xff');
      file.write(corrupt.data(), static_cast<std::streamsize>(corrupt.size())); }
    CampusLifeGame target{ Student{ "Keep", 1234, 44 } };
    EXPECT_THROW(saves.load(2, target), CorruptedSaveDataException);
    EXPECT_EQ(target.getStudent().getName(), "Keep");
    ASSERT_TRUE(saves.load(1, target));
    EXPECT_EQ(target.getStudent().getName(), "First");
    ASSERT_TRUE(saves.load(3, target));
    EXPECT_EQ(target.getStudent().getName(), "Third");
}

TEST_F(PersistenceTest, InvalidRecordFieldsAreRejectedWithoutChangingGame) {
    const auto path{ temporary.path() / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame original{ Student{ "Valid", 1000, 50 } };
    CampusLifeGame target{ Student{ "Keep", 1234, 44 } };
    target.planWork(0);
    const Activity* action{ target.getPlan()[0].activity };
    struct Mutation { std::streamoff position; std::int32_t value; };
    // The version-1 record uses 32-bit fields followed by fixed-size names.
    const Mutation mutations[]{ {0, 0}, {4, 0}, {8, -1}, {12, 101}, {16, 2}, {120, 4}, {124, 100} };
    for (const Mutation& mutation : mutations) {
        ASSERT_NO_THROW(saves.save(1, original));
        {
            std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary };
            file.seekp(mutation.position);
            file.write(reinterpret_cast<const char*>(&mutation.value), sizeof(mutation.value));
        }
        EXPECT_THROW(saves.load(1, target), CorruptedSaveDataException);
        EXPECT_EQ(target.getStudent().getName(), "Keep");
        EXPECT_EQ(target.getPlan()[0].activity, action);
    }
}

TEST_F(PersistenceTest, InvalidCourseGradeDoesNotChangeCourse) {
    Course course{ "Data" };
    course.setProgress(50);
    EXPECT_FALSE(course.restoreState("Z", 0));
    EXPECT_FALSE(course.restoreState("A+", 1));
    EXPECT_EQ(course.getGrade(), "C");
    EXPECT_EQ(course.getProgress(), 50);
}

TEST_F(PersistenceTest, AllCourseGradesRoundTripThroughBinarySlots) {
    const std::string grades[]{ "C", "B", "A", "A+" };
    SaveManager saves{ temporary.path() / "save.dat" };
    for (const std::string& grade : grades) {
        Student student;
        ASSERT_TRUE(student.getCourse(0).restoreState(grade, grade == "A+" ? 0 : 37));
        CampusLifeGame original{ student };
        ASSERT_NO_THROW(saves.save(1, original));
        CampusLifeGame restored{ Student{} };
        ASSERT_TRUE(saves.load(1, restored));
        EXPECT_EQ(restored.getStudent().getCourse(0).getGrade(), grade);
        EXPECT_EQ(restored.getStudent().getCourse(0).getProgress(), grade == "A+" ? 0 : 37);
    }
}

TEST_F(PersistenceTest, TooLongCourseNameIsRejectedWithoutChangingExistingSlot) {
    const auto path{ temporary.path() / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame original{ Student{ "Keep", 1234, 44 } };
    ASSERT_NO_THROW(saves.save(1, original));
    const std::string names[]{ std::string(32, 'x'), "Data", "Architecture" };
    CampusLifeGame oversized{ Student{ "Other", 50000, 100, names, 3 } };
    EXPECT_THROW(saves.save(1, oversized), GameException);
    CampusLifeGame restored{ Student{} };
    ASSERT_TRUE(saves.load(1, restored));
    EXPECT_EQ(restored.getStudent().getName(), "Keep");
}

TEST_F(PersistenceTest, FailedSaveOrLogDoesNotChangeStudentState) {
    const auto blocker{ temporary.path() / "not-a-directory" };
    { std::ofstream out{ blocker }; out << "keep"; }
    CampusLifeGame game{ Student{ "Ara", 1234, 44 } };
    EXPECT_THROW((SaveManager{ blocker / "save.dat" }.save(1, game)), FileOpenException);
    EXPECT_EQ((LogManager{ blocker / "play.log" }.append(1, game)), PersistenceResult::IoError);
    EXPECT_EQ(game.getStudent().getMoney(), 1234);
    EXPECT_EQ(game.getStudent().getEnergy(), 44);
    std::ifstream existing{ blocker };
    std::string contents;
    ASSERT_TRUE(static_cast<bool>(std::getline(existing, contents)));
    EXPECT_EQ(contents, "keep");
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
}

TEST_F(PersistenceTest, ConsoleSavesAndLoadsSelectedSlot) {
    std::istringstream input{ "1\n3\n3\n3\n3\n3\ny\n5\n2\n0\n2\n2\n0\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };
    ui.run();
    const std::string text{ output.str() };
    EXPECT_NE(text.find("Game saved"), std::string::npos);
    EXPECT_NE(text.find("Game loaded"), std::string::npos);
    EXPECT_NE(text.find("2. [Week 2]"), std::string::npos);
    const auto firstWeek2 = text.find("===== WEEK 2 =====");
    ASSERT_NE(firstWeek2, std::string::npos);
    EXPECT_NE(text.find("===== WEEK 2 =====", firstWeek2 + 1), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleReportsEmptySlotAndCanCancel) {
    std::istringstream input{ "2\n1\n2\n0\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };
    ui.run();
    EXPECT_NE(output.str().find("EMPTY"), std::string::npos);
    EXPECT_NE(output.str().find("No saved game"), std::string::npos);
    EXPECT_EQ(output.str().find("===== WEEK 1 ====="), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleReturnsToMainMenuWhenSaveFileCannotBeOpened) {
    std::filesystem::create_directory(temporary.path() / "save.dat");
    std::istringstream input{ "2\n1\n0\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.run();

    const std::string text{ output.str() };
    const auto failure{ text.find("Load failed:") };
    ASSERT_NE(failure, std::string::npos);
    EXPECT_NE(text.find("save.dat", failure), std::string::npos);
    EXPECT_NE(text.find("===== WEEK 1 =====", failure), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleRejectsCorruptSlotAndCanThenLoadAnotherSlot) {
    const auto path{ temporary.path() / "save.dat" };
    SaveManager saves{ path };
    CampusLifeGame saved{ Student{ "Ara", 1234, 44 } };
    ASSERT_NO_THROW(saves.save(2, saved));
    {
        std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary };
        file.put('\1');
        ASSERT_TRUE(file.good());
    }
    std::istringstream input{ "2\n1\n2\n2\n0\n0\n" };
    std::ostringstream output;
    ConsoleUI ui{ input, output, temporary.path() };

    ui.run();

    const std::string text{ output.str() };
    EXPECT_NE(text.find("1. INVALID DATA"), std::string::npos);
    const auto failure{ text.find("Load failed: invalid save data in slot 1") };
    ASSERT_NE(failure, std::string::npos);
    EXPECT_NE(text.find("Game loaded.", failure), std::string::npos);
    EXPECT_NE(text.find("Money: 1234", failure), std::string::npos);
}

TEST_F(PersistenceTest, ConsoleResumesPlanningWhenSaveFileCannotBeOpened) {
    std::filesystem::create_directory(temporary.path() / "save.dat");
    std::istringstream input{ "5\n3\n3\n3\n3\n3\ny\n" };
    std::ostringstream output;
    CampusLifeGame game{ Student{} };
    ConsoleUI ui{ input, output, temporary.path() };

    ui.play(game);

    const std::string text{ output.str() };
    EXPECT_NE(text.find("Save failed: could not access save file: save.dat"), std::string::npos);
    EXPECT_EQ(game.getWeek(), 2);
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
