#include <string>

#include <gtest/gtest.h>

#include "Student.hpp"

TEST(StudentOwnershipTest, CopyConstructorCreatesIndependentCourses) {
    Student original{ "Biryong", 50000, 100 };
    Student copied{ original };

    EXPECT_NE(&copied.getCourse(0), &original.getCourse(0));
    ASSERT_TRUE(copied.study(0));
    EXPECT_EQ(copied.getCourse(0).getProgress(), 40);
    EXPECT_EQ(original.getCourse(0).getProgress(), 0);
}

TEST(StudentOwnershipTest, CopyAssignmentCreatesIndependentCourses) {
    Student original{ "Biryong", 50000, 100 };
    ASSERT_TRUE(original.study(0));

    Student assigned;
    assigned = original;

    EXPECT_NE(&assigned.getCourse(0), &original.getCourse(0));
    ASSERT_TRUE(original.study(0));
    EXPECT_EQ(assigned.getCourse(0).getProgress(), 40);
    EXPECT_EQ(original.getCourse(0).getProgress(), 80);
}

TEST(StudentOwnershipTest, CopySelfAssignmentPreservesState) {
    Student student{ "Biryong", 50000, 100 };
    ASSERT_TRUE(student.study(0));

    student = student;

    EXPECT_EQ(student.getCourse(0).getProgress(), 40);
}

TEST(StudentOwnershipTest, CustomCourseListUsesRequestedSizeAndNames) {
    const std::string courseNames[]{ "Algorithms", "Operating Systems" };
    Student custom{ "Ara", 50000, 100, courseNames, 2 };

    EXPECT_EQ(custom.getCourseCount(), 2);
    EXPECT_EQ(custom.getCourse(0).getName(), "Algorithms");
    EXPECT_EQ(custom.getCourse(1).getName(), "Operating Systems");
}
