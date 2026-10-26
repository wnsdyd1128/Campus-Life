#include "PartTimeJobActivity.hpp"

#include "Student.hpp"

std::string PartTimeJobActivity::getName() const {
    return "Part-time job";
}

bool PartTimeJobActivity::execute(Student& student, bool, int) const {
    return student.work();
}
