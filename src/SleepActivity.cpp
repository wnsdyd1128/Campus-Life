#include "SleepActivity.hpp"

#include "Student.hpp"

std::string SleepActivity::getName() const {
    return "Sleep";
}

bool SleepActivity::execute(Student& student, bool, int) const {
    student.sleep();
    return true;
}
