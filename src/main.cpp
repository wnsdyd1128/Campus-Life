#include <iostream>

#include "ChickenActivity.hpp"
#include "PartTimeJobActivity.hpp"
#include "SleepActivity.hpp"
#include "Student.hpp"
#include "StudyActivity.hpp"
#include "WeeklyPlan.hpp"

int main() {
    Student student{ "Biryong", 50000, 100 };
    StudyActivity studyOop{ 0 };
    StudyActivity studyDataStructure{ 1 };
    ChickenActivity chicken;
    PartTimeJobActivity work;
    SleepActivity sleep;
    WeeklyPlan plan;
    plan[0] = { "Study", 0 };
    plan[1] = { "Study", 1 };
    plan[2] = { "Chicken", -1 };
    plan[3] = { "Work", -1 };
    plan[4] = { "Sleep", -1 };

    std::cout << "===== CAMPUS LIFE : START =====" << std::endl;
    std::cout << student << std::endl;

    const char* dayNames[WeeklyPlan::DAY_COUNT]{
        "MON", "TUE", "WED", "THU", "FRI"
    };
    const WeeklyPlan& savedPlan{ plan };

    std::cout << std::endl << "===== WEEKLY PLAN =====" << std::endl;
    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        std::cout << dayNames[day] << " : " << savedPlan[day].action;
        if (savedPlan[day].courseIndex >= 0) {
            std::cout << " - "
                      << student.getCourse(savedPlan[day].courseIndex).getName();
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "===== TODAY'S ACTIONS =====" << std::endl;

    if (studyOop.execute(student)) {
        std::cout << studyOop.getName() << ": OOP2" << std::endl;
    }

    if (studyDataStructure.execute(student)) {
        std::cout << studyDataStructure.getName() << ": Data Structure" << std::endl;
    }

    if (chicken.execute(student)) {
        std::cout << chicken.getName() << std::endl;
    }

    if (work.execute(student)) {
        std::cout << work.getName() << std::endl;
    }

    sleep.execute(student);
    std::cout << sleep.getName() << std::endl;

    std::cout << std::endl << "===== CAMPUS LIFE : RESULT =====" << std::endl;
    std::cout << student << std::endl;

    const Student& currentStudent{ student };
    std::cout << std::endl << "Registered courses: "
              << currentStudent.getCourseCount() << std::endl;
    std::cout << currentStudent.getCourse(0) << std::endl;

    return 0;
}
