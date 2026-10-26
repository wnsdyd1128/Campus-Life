#include "CampusLifeGame.hpp"

CampusLifeGame::CampusLifeGame(const Student& student)
    : student{ student } {
}

const Student& CampusLifeGame::getStudent() const { return student; }
const WeeklyPlan& CampusLifeGame::getPlan() const { return plan; }
int CampusLifeGame::getWeek() const { return week; }
bool CampusLifeGame::getCurrentBurnout() const { return currentBurnout; }
bool CampusLifeGame::getNextBurnout() const { return nextBurnout; }

void CampusLifeGame::planStudy(int day, int courseIndex) {
    plan[day] = { &study, courseIndex };
}

void CampusLifeGame::planWork(int day) {
    plan[day] = { &work, -1 };
}

void CampusLifeGame::planSleep(int day) {
    plan[day] = { &sleep, -1 };
}

void CampusLifeGame::planChicken(int day) {
    plan[day] = { &chicken, -1 };
}

WeekResult CampusLifeGame::simulateWeek() {
    currentBurnout = nextBurnout;
    nextBurnout = false;
    WeekResult result;

    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        const PlannedAction& selected{ plan[day] };
        if (selected.activity == nullptr) {
            continue;
        }

        result.succeeded[day] = selected.activity->execute(
            student, currentBurnout, selected.courseIndex);
        if (result.succeeded[day] && student.getEnergy() == 0) {
            nextBurnout = true;
        }
    }

    student.setEnergy(student.getEnergy() + 35);
    ++week;
    return result;
}
