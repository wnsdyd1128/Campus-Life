#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "Course.hpp"
#include "Student.hpp"
#include "WeeklyPlan.hpp"

TEST(CourseOperatorTest, PlusEqualsReturnsCourseForChaining) {
    Course course{ "Algorithms" };

    Course& result = ((course += 10) += 30);

    EXPECT_EQ(&result, &course);
    EXPECT_EQ(course.getProgress(), 40);
}

TEST(CourseOperatorTest, PlusEqualsAdvancesOneGradeAtHundredPoints) {
    Course course{ "Algorithms", 80 };

    course += 40;

    EXPECT_EQ(course.getGrade(), "B");
    EXPECT_EQ(course.getProgress(), 0);
}

TEST(StreamOperatorTest, CourseOutputContainsGradeAndProgress) {
    Course course{ "Algorithms" };
    course += 40;
    std::ostringstream output;

    output << course;

    EXPECT_EQ(output.str(), "Algorithms : C [40/100]");
}

TEST(StreamOperatorTest, StudentOutputContainsResourcesAndCourses) {
    Student student{ "Ara", 50000, 100 };
    std::ostringstream output;

    output << student;

    EXPECT_NE(output.str().find("Name   : Ara"), std::string::npos);
    EXPECT_NE(output.str().find("Money  : 50000 won"), std::string::npos);
    EXPECT_NE(output.str().find("Object-Oriented Programming 2 : C [0/100]"),
              std::string::npos);
}

TEST(WeeklyPlanOperatorTest, SubscriptStoresAndReadsFiveActions) {
    WeeklyPlan plan;
    plan[0] = { "Study", 0 };
    plan[1] = { "Work", -1 };
    plan[2] = { "Sleep", -1 };
    plan[3] = { "Chicken", -1 };
    plan[4] = { "Study", 2 };
    const WeeklyPlan& savedPlan{ plan };

    EXPECT_EQ(savedPlan[0].action, "Study");
    EXPECT_EQ(savedPlan[0].courseIndex, 0);
    EXPECT_EQ(savedPlan[1].action, "Work");
    EXPECT_EQ(savedPlan[2].action, "Sleep");
    EXPECT_EQ(savedPlan[3].action, "Chicken");
    EXPECT_EQ(savedPlan[4].courseIndex, 2);
}

TEST(WeeklyPlanOperatorTest, ClearResetsEveryDay) {
    WeeklyPlan plan;
    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        plan[day] = { "Study", day % 3 };
    }

    plan.clear();

    for (int day = 0; day < WeeklyPlan::DAY_COUNT; ++day) {
        EXPECT_EQ(plan[day].action, "None");
        EXPECT_EQ(plan[day].courseIndex, -1);
    }
}
