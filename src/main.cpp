#include <iostream>

#include "Activity.hpp"
#include "CampusLifeGame.hpp"

int main() {
    CampusLifeGame game{ Student{ "Biryong", 50000, 100 } };
    game.planStudy(0, 0);
    game.planStudy(1, 1);
    game.planChicken(2);
    game.planWork(3);
    game.planSleep(4);

    std::cout << "===== CAMPUS LIFE : START =====" << std::endl;
    std::cout << game.getStudent() << std::endl;

    const char* dayNames[WeeklyPlan::DAY_COUNT]{
        "MON", "TUE", "WED", "THU", "FRI"
    };
    const WeeklyPlan& savedPlan{ game.getPlan() };

    std::cout << std::endl << "===== WEEKLY PLAN =====" << std::endl;
    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        std::cout << dayNames[day] << " : "
                  << savedPlan[day].activity->getName();
        if (savedPlan[day].courseIndex >= 0) {
            std::cout << " - "
                      << game.getStudent().getCourse(savedPlan[day].courseIndex).getName();
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "===== WEEK " << game.getWeek()
              << " ACTION RESULTS =====" << std::endl;
    const WeekResult result{ game.simulateWeek() };
    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        std::cout << dayNames[day] << " : " << savedPlan[day].activity->getName()
                  << (result.succeeded[day] ? " - Success" : " - Failed") << std::endl;
    }

    std::cout << std::endl << "===== CAMPUS LIFE : RESULT =====" << std::endl;
    std::cout << "Weekend recovery: +35 energy (max 100)" << std::endl;
    std::cout << game.getStudent() << std::endl;
    std::cout << "Current burnout: " << game.getCurrentBurnout() << std::endl;
    std::cout << "Next week burnout: " << game.getNextBurnout() << std::endl;

    return 0;
}
