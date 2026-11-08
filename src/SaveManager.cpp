#include "SaveManager.hpp"

#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>

#include "CampusLifeGame.hpp"

namespace {
constexpr int COURSE_COUNT{ 3 };
constexpr int FORMAT_VERSION{ 1 };

struct CourseRecord {
    std::string name;
    std::string grade;
    int progress{};
};

struct SaveRecord {
    int week{};
    int currentBurnout{};
    int nextBurnout{};
    int money{};
    int energy{};
    std::string name;
    std::array<CourseRecord, COURSE_COUNT> courses;
};

bool validCourse(const CourseRecord& course) {
    return (course.grade == "C" || course.grade == "B" || course.grade == "A" ||
            course.grade == "A+") && course.progress >= 0 && course.progress < 100 &&
           (course.grade != "A+" || course.progress == 0);
}

PersistenceResult readLatest(const std::filesystem::path& path, SaveRecord& latest) {
    std::error_code error;
    const bool exists{ std::filesystem::exists(path, error) };
    if (error) return PersistenceResult::IoError;
    if (!exists) return PersistenceResult::NotFound;
    std::ifstream input{ path };
    if (!input) return PersistenceResult::IoError;

    bool found{ false };
    while (true) {
        input >> std::ws;
        if (input.bad()) return PersistenceResult::IoError;
        if (input.eof()) break;
        std::string marker;
        int version{}, count{};
        SaveRecord record;
        if (!(input >> marker >> version) || marker != "CAMPUS_LIFE_SAVE" ||
            version != FORMAT_VERSION ||
            !(input >> record.week >> record.currentBurnout >> record.nextBurnout >>
              record.money >> record.energy >> std::quoted(record.name) >> count)) {
            return input.bad() ? PersistenceResult::IoError : PersistenceResult::InvalidData;
        }
        if (record.week < 1 || record.week > 16 || count != COURSE_COUNT ||
            record.money < 0 || record.energy < 0 || record.energy > 100 ||
            record.currentBurnout < 0 || record.currentBurnout > 1 ||
            record.nextBurnout < 0 || record.nextBurnout > 1) {
            return PersistenceResult::InvalidData;
        }
        for (CourseRecord& course : record.courses) {
            if (!(input >> std::quoted(course.name) >> std::quoted(course.grade) >> course.progress)) {
                return input.bad() ? PersistenceResult::IoError : PersistenceResult::InvalidData;
            }
            if (!validCourse(course)) return PersistenceResult::InvalidData;
        }
        if (!(input >> marker) || marker != "END") {
            return input.bad() ? PersistenceResult::IoError : PersistenceResult::InvalidData;
        }
        latest = std::move(record);
        found = true;
    }
    return found ? PersistenceResult::Ok : PersistenceResult::InvalidData;
}
}

SaveManager::SaveManager(std::filesystem::path path)
    : path{ std::move(path) } {
}

PersistenceResult SaveManager::save(const CampusLifeGame& game) const {
    const Student& student{ game.getStudent() };
    if (game.getWeek() < 1 || game.getWeek() > 16 || student.getCourseCount() != COURSE_COUNT) {
        return PersistenceResult::InvalidData;
    }
    std::ostringstream record;
    record << "CAMPUS_LIFE_SAVE " << FORMAT_VERSION << '\n'
           << game.getWeek() << ' ' << game.getCurrentBurnout() << ' '
           << game.getNextBurnout() << ' ' << student.getMoney() << ' '
           << student.getEnergy() << ' ' << std::quoted(student.getName()) << ' '
           << COURSE_COUNT << '\n';
    for (int i = 0; i < COURSE_COUNT; ++i) {
        const Course& course{ student.getCourse(i) };
        if (!validCourse({ course.getName(), course.getGrade(), course.getProgress() })) {
            return PersistenceResult::InvalidData;
        }
        record << std::quoted(course.getName()) << ' ' << std::quoted(course.getGrade())
               << ' ' << course.getProgress() << '\n';
    }
    record << "END\n";
    if (!record) return PersistenceResult::IoError;

    SaveRecord previous;
    const PersistenceResult existing{ readLatest(path, previous) };
    if (existing != PersistenceResult::Ok && existing != PersistenceResult::NotFound) return existing;
    std::error_code error;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), error);
    if (error) return PersistenceResult::IoError;
    std::ofstream output{ path, std::ios::app };
    if (!output) return PersistenceResult::IoError;
    if (existing == PersistenceResult::Ok) output << '\n';
    output << record.str();
    output.close();
    return output ? PersistenceResult::Ok : PersistenceResult::IoError;
}

PersistenceResult SaveManager::load(CampusLifeGame& game) const {
    SaveRecord latest;
    const PersistenceResult result{ readLatest(path, latest) };
    if (result != PersistenceResult::Ok) return result;
    std::array<std::string, COURSE_COUNT> names;
    for (int i = 0; i < COURSE_COUNT; ++i) names[i] = latest.courses[i].name;
    Student restored{ latest.name, latest.money, latest.energy, names.data(), COURSE_COUNT };
    for (int i = 0; i < COURSE_COUNT; ++i) {
        if (!restored.getCourse(i).restoreState(latest.courses[i].grade, latest.courses[i].progress)) {
            return PersistenceResult::InvalidData;
        }
    }
    if (!game.restoreState(restored, latest.week,
                           latest.currentBurnout != 0, latest.nextBurnout != 0)) {
        return PersistenceResult::InvalidData;
    }
    return PersistenceResult::Ok;
}
