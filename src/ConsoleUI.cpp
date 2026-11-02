#include "ConsoleUI.hpp"

#include <cctype>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

#include "Activity.hpp"
#include "CampusLifeGame.hpp"

namespace {
constexpr int SEMESTER_WEEKS{ 16 };
const char* const DAY_NAMES[]{ "MON", "TUE", "WED", "THU", "FRI" };
const char* const ACTION_NAMES[]{ "", "Study", "Part-time job", "Sleep", "Eat chicken" };

bool allCoursesComplete(const Student& student) {
    if (student.getCourseCount() != 3) return false;
    for (int i = 0; i < student.getCourseCount(); ++i) {
        if (student.getCourse(i).getGrade() != "A+") return false;
    }
    return true;
}

void printStudent(std::ostream& output, const Student& student) {
    output << "Money: " << student.getMoney() << "\nEnergy: "
           << student.getEnergy() << " / 100\n";
    for (int i = 0; i < student.getCourseCount(); ++i) {
        output << "  " << student.getCourse(i) << '\n';
    }
}
}

ConsoleUI::ConsoleUI(std::istream& input, std::ostream& output)
    : input{ input }, output{ output } {
}

bool ConsoleUI::readInt(const std::string& prompt, int min, int max, int& value) {
    std::string line;
    while (true) {
        output << prompt;
        if (!std::getline(input, line)) return false;
        std::istringstream parsed{ line };
        int candidate{};
        char extra{};
        if ((parsed >> candidate) && !(parsed >> extra) &&
            candidate >= min && candidate <= max) {
            value = candidate;
            return true;
        }
        output << "Invalid input. Enter a number from " << min << " to " << max << ".\n";
    }
}

bool ConsoleUI::readConfirmation(bool& confirmed) {
    std::string line;
    while (true) {
        output << "Execute plan? (Y/N, 0: menu) >> ";
        if (!std::getline(input, line)) return false;
        std::istringstream parsed{ line };
        char choice{}, extra{};
        if ((parsed >> choice) && !(parsed >> extra)) {
            const char normalized = static_cast<char>(
                std::tolower(static_cast<unsigned char>(choice)));
            if (normalized == '0') return false;
            if (normalized == 'y' || normalized == 'n') {
                confirmed = normalized == 'y';
                return true;
            }
        }
        output << "Invalid input. Enter Y or N.\n";
    }
}

void ConsoleUI::run() {
    while (true) {
        output << "\n===== CAMPUS LIFE =====\n1. New game\n0. Exit\n";
        int selection{};
        if (!readInt("Select >> ", 0, 1, selection) || selection == 0) return;
        CampusLifeGame game{ Student{ "Biryong", 50000, 100 } };
        play(game);
    }
}

void ConsoleUI::play(CampusLifeGame& game) {
    while (game.getWeek() <= SEMESTER_WEEKS) {
        output << "\n===== WEEK " << game.getWeek() << " =====\n";
        printStudent(output, game.getStudent());
        output << "Study: " << (game.getNextBurnout() ? "x0.5 (BURNOUT)" : "x1.0") << '\n';

        int choices[WeeklyPlan::DAY_COUNT]{};
        int courses[WeeklyPlan::DAY_COUNT]{};
        bool confirmed{ false };
        while (!confirmed) {
            for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
                output << "\n[" << DAY_NAMES[day] << " plan]\n"
                       << "1. Study\n2. Part-time job\n3. Sleep\n4. Eat chicken\n0. Main menu\n";
                if (!readInt("Select >> ", 0, 4, choices[day]) || choices[day] == 0) return;
                if (choices[day] == 1) {
                    const Student& student{ game.getStudent() };
                    output << "Choose a course:\n";
                    for (int i = 0; i < student.getCourseCount(); ++i) {
                        output << i + 1 << ". " << student.getCourse(i) << '\n';
                    }
                    int course{};
                    if (!readInt("Course (0: menu) >> ", 0, student.getCourseCount(), course) ||
                        course == 0) return;
                    courses[day] = course - 1;
                }
            }

            output << "\n===== WEEK " << game.getWeek() << " PLAN =====\n";
            for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
                output << DAY_NAMES[day] << ": " << ACTION_NAMES[choices[day]];
                if (choices[day] == 1) {
                    output << " - " << game.getStudent().getCourse(courses[day]).getName();
                }
                output << '\n';
            }
            if (!readConfirmation(confirmed)) return;
            if (!confirmed) output << "Plan not confirmed. Enter all five days again.\n";
        }

        for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
            if (choices[day] == 1) game.planStudy(day, courses[day]);
            else if (choices[day] == 2) game.planWork(day);
            else if (choices[day] == 3) game.planSleep(day);
            else game.planChicken(day);
        }
        const int week{ game.getWeek() };
        const WeekResult result{ game.simulateWeek() };
        const WeeklyPlan& plan{ game.getPlan() };
        for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
            const DayResult& daily{ result.days[day] };
            output << "\n[" << DAY_NAMES[day] << "] " << plan[day].activity->getName()
                   << (result.succeeded[day] ? " - Success\n" : " - Failed\n");
            output << "Energy: " << daily.energyBefore << " -> " << daily.energyAfter << '\n';
            output << "Money: " << daily.moneyBefore << " -> " << daily.moneyAfter << '\n';
            if (daily.hasCourse) {
                output << game.getStudent().getCourse(plan[day].courseIndex).getName() << ": "
                       << daily.gradeBefore << " [" << daily.progressBefore << "] -> "
                       << daily.gradeAfter << " [" << daily.progressAfter << "]\n";
                if (daily.gradeBefore != daily.gradeAfter) output << "Grade advanced!\n";
            }
            if (!result.succeeded[day]) {
                if (choices[day] == 1) output << "Need energy: 25\n";
                else if (choices[day] == 2) output << "Need energy: 40\n";
                else if (choices[day] == 4) output << "Need money: 25000\n";
            } else if (daily.energyAfter == 0) {
                output << "Next week burnout reserved.\n";
            }
        }
        output << "\n===== WEEK " << week << " RESULT =====\nWeekend Energy: "
               << result.weekendEnergyBefore << " -> " << result.weekendEnergyAfter << '\n';
        printStudent(output, game.getStudent());
        output << "Next week study: " << (game.getNextBurnout() ? "x0.5 (BURNOUT)" : "x1.0") << '\n';

        if (allCoursesComplete(game.getStudent())) {
            output << "\n===== SCHOLARSHIP: CAMPUS LIFE CLEAR! =====\n";
            return;
        }
        if (week == SEMESTER_WEEKS) {
            output << "\n===== SEMESTER RESULT =====\nScholarship requirements not met.\n";
            printStudent(output, game.getStudent());
            output << "Incomplete courses:\n";
            for (int i = 0; i < game.getStudent().getCourseCount(); ++i) {
                if (game.getStudent().getCourse(i).getGrade() != "A+") {
                    output << "  " << game.getStudent().getCourse(i).getName() << '\n';
                }
            }
            return;
        }
    }
}
