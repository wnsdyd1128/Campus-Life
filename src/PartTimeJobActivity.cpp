#include "PartTimeJobActivity.hpp"

#include "Student.hpp"

PartTimeJobActivity::PartTimeJobActivity()
    : Activity{ "Part-time job" } {
}

bool PartTimeJobActivity::execute(Student& student) const {
    return student.work();
}
