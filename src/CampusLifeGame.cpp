#include "CampusLifeGame.hpp"

CampusLifeGame::CampusLifeGame(const Student& student)
    : student{ student } {
}

const Student& CampusLifeGame::getStudent() const { return student; }
const WeeklyPlan& CampusLifeGame::getPlan() const { return plan; }
int CampusLifeGame::getWeek() const { return week; }
bool CampusLifeGame::getCurrentBurnout() const { return currentBurnout; }
bool CampusLifeGame::getNextBurnout() const { return nextBurnout; }

bool CampusLifeGame::restoreState(const Student& savedStudent, int savedWeek,
                                  bool savedCurrentBurnout, bool savedNextBurnout) {
    if (savedWeek < 1 || savedWeek > 16 || savedStudent.getCourseCount() != 3 ||
        savedStudent.getMoney() < 0 || savedStudent.getEnergy() < 0 || savedStudent.getEnergy() > 100) {
        return false;
    }
    student = savedStudent;
    week = savedWeek;
    currentBurnout = savedCurrentBurnout;
    nextBurnout = savedNextBurnout;
    plan.clear();
    return true;
}

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
        DayResult& daily{ result.days[day] };
        daily.energyBefore = student.getEnergy();
        daily.moneyBefore = student.getMoney();
        daily.energyAfter = daily.energyBefore;
        daily.moneyAfter = daily.moneyBefore;
        if (selected.activity == nullptr) {
            continue;
        }

        daily.hasCourse = selected.courseIndex >= 0 &&
                          selected.courseIndex < student.getCourseCount();
        if (daily.hasCourse) {
            const Course& course{ student.getCourse(selected.courseIndex) };
            daily.gradeBefore = course.getGrade();
            daily.progressBefore = course.getProgress();
        }
        result.succeeded[day] = selected.activity->execute(
            student, currentBurnout, selected.courseIndex);
        daily.energyAfter = student.getEnergy();
        daily.moneyAfter = student.getMoney();
        if (daily.hasCourse) {
            const Course& course{ student.getCourse(selected.courseIndex) };
            daily.gradeAfter = course.getGrade();
            daily.progressAfter = course.getProgress();
        }
        if (result.succeeded[day] && student.getEnergy() == 0) {
            nextBurnout = true;
        }
    }

    result.weekendEnergyBefore = student.getEnergy();
    student.setEnergy(student.getEnergy() + 35);
    result.weekendEnergyAfter = student.getEnergy();
    ++week;
    return result;
}
