#include "WeeklyPlan.hpp"

PlannedAction& WeeklyPlan::operator[](int day) {
    return days[day];
}

const PlannedAction& WeeklyPlan::operator[](int day) const {
    return days[day];
}

void WeeklyPlan::clear() {
    for (int day = 0; day < DAY_COUNT; ++day) {
        days[day] = PlannedAction{};
    }
}
