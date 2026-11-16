#include "SaveManager.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include "CampusLifeGame.hpp"

namespace {
constexpr int COURSE_COUNT{ 3 };
constexpr std::int32_t RECORD_MARKER{ 0x434c5331 };
const char* const GRADE_NAMES[]{ "C", "B", "A", "A+" };

struct CourseRecord {
    char name[32]{};
    std::int32_t grade{};
    std::int32_t progress{};
};

struct SaveRecord {
    std::int32_t marker{};
    std::int32_t week{};
    std::int32_t money{};
    std::int32_t energy{};
    std::int32_t currentBurnout{};
    std::int32_t nextBurnout{};
    char studentName[64]{};
    CourseRecord courses[COURSE_COUNT]{};
};

static_assert(std::is_trivially_copyable_v<SaveRecord>);
static_assert(sizeof(SaveRecord) == 208, "Unexpected binary save record layout");

std::streamoff offset(int slot) {
    return static_cast<std::streamoff>(slot - 1) * sizeof(SaveRecord);
}

bool copyName(char* destination, std::size_t size, const std::string& name) {
    if (name.size() >= size || name.find('\0') != std::string::npos) return false;
    std::copy(name.begin(), name.end(), destination);
    return true;
}

bool terminated(const char* name, std::size_t size) {
    return std::find(name, name + size, '\0') != name + size;
}

std::int32_t gradeCode(const std::string& grade) {
    for (std::int32_t code = 0; code < 4; ++code) {
        if (grade == GRADE_NAMES[code]) return code;
    }
    return -1;
}

PersistenceResult inspectFile(const std::filesystem::path& path) {
    std::error_code error;
    const bool exists{ std::filesystem::exists(path, error) };
    if (error) return PersistenceResult::IoError;
    if (!exists) return PersistenceResult::NotFound;
    const auto size{ std::filesystem::file_size(path, error) };
    if (error) return PersistenceResult::IoError;
    return size == sizeof(SaveRecord) * SaveManager::SLOT_COUNT
        ? PersistenceResult::Ok : PersistenceResult::InvalidData;
}

bool validRecord(const SaveRecord& record) {
    if (record.marker != RECORD_MARKER || record.week < 1 || record.week > 16 ||
        record.money < 0 || record.energy < 0 || record.energy > 100 ||
        record.currentBurnout < 0 || record.currentBurnout > 1 ||
        record.nextBurnout < 0 || record.nextBurnout > 1 ||
        !terminated(record.studentName, sizeof(record.studentName))) return false;
    for (const CourseRecord& course : record.courses) {
        if (!terminated(course.name, sizeof(course.name)) || course.grade < 0 || course.grade > 3 ||
            course.progress < 0 || course.progress >= 100 ||
            (course.grade == 3 && course.progress != 0)) return false;
    }
    return true;
}
}

SaveManager::SaveManager(std::filesystem::path path)
    : path{ std::move(path) } {
}

PersistenceResult SaveManager::save(int slot, const CampusLifeGame& game) const {
    if (slot < 1 || slot > SLOT_COUNT) return PersistenceResult::InvalidData;
    const Student& student{ game.getStudent() };
    if (game.getWeek() < 1 || game.getWeek() > 16 || student.getCourseCount() != COURSE_COUNT ||
        student.getMoney() < 0 || student.getEnergy() < 0 || student.getEnergy() > 100) {
        return PersistenceResult::InvalidData;
    }
    SaveRecord record{};
    record.marker = RECORD_MARKER;
    record.week = game.getWeek();
    record.money = student.getMoney();
    record.energy = student.getEnergy();
    record.currentBurnout = game.getCurrentBurnout();
    record.nextBurnout = game.getNextBurnout();
    if (!copyName(record.studentName, sizeof(record.studentName), student.getName())) return PersistenceResult::InvalidData;
    for (int i = 0; i < COURSE_COUNT; ++i) {
        const Course& course{ student.getCourse(i) };
        if (!copyName(record.courses[i].name, sizeof(record.courses[i].name), course.getName())) return PersistenceResult::InvalidData;
        record.courses[i].grade = gradeCode(course.getGrade());
        record.courses[i].progress = course.getProgress();
    }
    if (!validRecord(record)) return PersistenceResult::InvalidData;

    const PersistenceResult status{ inspectFile(path) };
    if (status != PersistenceResult::Ok && status != PersistenceResult::NotFound) return status;
    if (status == PersistenceResult::NotFound) {
        std::error_code error;
        if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), error);
        if (error) return PersistenceResult::IoError;
        std::ofstream output{ path, std::ios::binary };
        if (!output) return PersistenceResult::IoError;
        const SaveRecord empty{};
        for (int i = 0; i < SLOT_COUNT; ++i) {
            output.write(reinterpret_cast<const char*>(&empty), sizeof(empty));
        }
        output.close();
        if (!output) return PersistenceResult::IoError;
    }
    std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary };
    if (!file) return PersistenceResult::IoError;
    file.seekp(offset(slot), std::ios::beg);
    file.write(reinterpret_cast<const char*>(&record), sizeof(record));
    file.close();
    return file ? PersistenceResult::Ok : PersistenceResult::IoError;
}

PersistenceResult SaveManager::load(int slot, CampusLifeGame& game) const {
    if (slot < 1 || slot > SLOT_COUNT) return PersistenceResult::InvalidData;
    const PersistenceResult status{ inspectFile(path) };
    if (status != PersistenceResult::Ok) return status;
    std::ifstream file{ path, std::ios::binary };
    if (!file) return PersistenceResult::IoError;
    file.seekg(offset(slot), std::ios::beg);
    SaveRecord record{};
    if (!file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        return file.bad() ? PersistenceResult::IoError : PersistenceResult::InvalidData;
    }
    if (record.marker == 0) {
        const char* bytes{ reinterpret_cast<const char*>(&record) };
        return std::all_of(bytes, bytes + sizeof(record), [](char value) { return value == 0; })
            ? PersistenceResult::NotFound : PersistenceResult::InvalidData;
    }
    if (!validRecord(record)) return PersistenceResult::InvalidData;
    std::array<std::string, COURSE_COUNT> names;
    for (int i = 0; i < COURSE_COUNT; ++i) names[i] = record.courses[i].name;
    Student restored{ record.studentName, record.money, record.energy, names.data(), COURSE_COUNT };
    for (int i = 0; i < COURSE_COUNT; ++i) {
        if (!restored.getCourse(i).restoreState(GRADE_NAMES[record.courses[i].grade],
                                               record.courses[i].progress)) return PersistenceResult::InvalidData;
    }
    if (!game.restoreState(restored, record.week, record.currentBurnout != 0, record.nextBurnout != 0)) {
        return PersistenceResult::InvalidData;
    }
    return PersistenceResult::Ok;
}
