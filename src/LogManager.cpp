#include "LogManager.hpp"

#include <fstream>
#include <iomanip>
#include <system_error>
#include <utility>

#include "CampusLifeGame.hpp"

LogManager::LogManager(std::filesystem::path path)
    : path{ std::move(path) } {
}

PersistenceResult LogManager::append(int completedWeek, const CampusLifeGame& game) const {
    std::error_code error;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), error);
    if (error) return PersistenceResult::IoError;
    std::ofstream output{ path, std::ios::app };
    if (!output) return PersistenceResult::IoError;
    const Student& student{ game.getStudent() };
    output << "[Week " << completedWeek << "] Money=" << student.getMoney()
           << " Energy=" << student.getEnergy();
    for (int i = 0; i < student.getCourseCount(); ++i) {
        const Course& course{ student.getCourse(i) };
        output << ' ' << std::quoted(course.getName()) << '=' << course.getGrade()
               << ':' << course.getProgress();
    }
    output << " currentBurnout=" << game.getCurrentBurnout()
           << " nextBurnout=" << game.getNextBurnout() << '\n';
    output.close();
    return output ? PersistenceResult::Ok : PersistenceResult::IoError;
}
