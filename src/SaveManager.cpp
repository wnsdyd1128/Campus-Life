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
    if (slot < 1 || slot > SaveManager::SLOT_COUNT) throw InvalidSaveSlotException{ slot };
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

bool hasSaveFile(const std::filesystem::path& path) {
    std::error_code error;
    const bool exists{ std::filesystem::exists(path, error) };
    if (error) throw FileOpenException{ path, "could not access save file" };
    if (!exists) return false;
    if (!std::filesystem::is_regular_file(path, error)) {
        throw FileOpenException{ path, "could not access save file" };
    }
    const auto size{ std::filesystem::file_size(path, error) };
    if (error) throw FileOpenException{ path, "could not get save file size" };
    if (size != sizeof(SaveRecord) * SaveManager::SLOT_COUNT) {
        throw CorruptedSaveDataException{ "invalid save file size" };
    }
    return true;
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

void SaveManager::save(int slot, const CampusLifeGame& game) const {
    const std::streamoff position{ offset(slot) };
    const Student& student{ game.getStudent() };
    if (game.getWeek() < 1 || game.getWeek() > 16 || student.getCourseCount() != COURSE_COUNT ||
        student.getMoney() < 0 || student.getEnergy() < 0 || student.getEnergy() > 100) {
        throw GameException{ "invalid game state for saving" };
    }
    SaveRecord record{};
    record.marker = RECORD_MARKER;
    record.week = game.getWeek();
    record.money = student.getMoney();
    record.energy = student.getEnergy();
    record.currentBurnout = game.getCurrentBurnout();
    record.nextBurnout = game.getNextBurnout();
    if (!copyName(record.studentName, sizeof(record.studentName), student.getName())) {
        throw GameException{ "student name does not fit in a save record" };
    }
    for (int i = 0; i < COURSE_COUNT; ++i) {
        const Course& course{ student.getCourse(i) };
        if (!copyName(record.courses[i].name, sizeof(record.courses[i].name), course.getName())) {
            throw GameException{ "course name does not fit in a save record" };
        }
        record.courses[i].grade = gradeCode(course.getGrade());
        record.courses[i].progress = course.getProgress();
    }
    if (!validRecord(record)) throw GameException{ "invalid game state for saving" };

    if (!hasSaveFile(path)) {
        std::error_code error;
        if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), error);
        if (error) throw FileOpenException{ path, "could not create save directory" };
        std::ofstream output{ path, std::ios::binary };
        if (!output) throw FileOpenException{ path, "could not create save file" };
        const SaveRecord empty{};
        for (int i = 0; i < SLOT_COUNT; ++i) {
            output.write(reinterpret_cast<const char*>(&empty), sizeof(empty));
        }
        output.close();
        if (!output) throw FileOpenException{ path, "could not initialize save file" };
    }
    std::fstream file{ path, std::ios::in | std::ios::out | std::ios::binary };
    if (!file) throw FileOpenException{ path, "could not open save file" };
    file.seekp(position, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&record), sizeof(record));
    file.close();
    if (!file) throw FileOpenException{ path, "could not write save slot" };
}

bool SaveManager::load(int slot, CampusLifeGame& game) const {
    const std::streamoff position{ offset(slot) };
    if (!hasSaveFile(path)) return false;
    std::ifstream file{ path, std::ios::binary };
    if (!file) throw FileOpenException{ path, "could not open save file" };
    if (!file.seekg(position, std::ios::beg)) {
        throw FileOpenException{ path, "could not seek to save slot" };
    }
    SaveRecord record{};
    const auto corrupt = [slot] {
        return CorruptedSaveDataException{ "invalid save data in slot " + std::to_string(slot) };
    };
    if (!file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
        if (file.bad() || !file.eof()) throw FileOpenException{ path, "could not read save slot" };
        throw corrupt();
    }
    if (record.marker == 0) {
        const char* bytes{ reinterpret_cast<const char*>(&record) };
        if (std::all_of(bytes, bytes + sizeof(record), [](char value) { return value == 0; })) {
            return false;
        }
    }
    if (!validRecord(record)) throw corrupt();
    std::array<std::string, COURSE_COUNT> names;
    for (int i = 0; i < COURSE_COUNT; ++i) names[i] = record.courses[i].name;
    Student restored{ record.studentName, record.money, record.energy, names.data(), COURSE_COUNT };
    for (int i = 0; i < COURSE_COUNT; ++i) {
        if (!restored.getCourse(i).restoreState(GRADE_NAMES[record.courses[i].grade],
                                               record.courses[i].progress)) throw corrupt();
    }
    if (!game.restoreState(restored, record.week, record.currentBurnout != 0, record.nextBurnout != 0)) {
        throw corrupt();
    }
    return true;
}
