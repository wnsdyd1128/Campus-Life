#include "SleepActivity.hpp"

#include "Student.hpp"

SleepActivity::SleepActivity()
    : Activity{ "Sleep" } {
}

bool SleepActivity::execute(Student& student) const {
    student.sleep();
    return true;
}
