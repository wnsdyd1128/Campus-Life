#include "ChickenActivity.hpp"

#include "Student.hpp"

std::string ChickenActivity::getName() const {
    return "Eat chicken";
}

bool ChickenActivity::execute(Student& student, bool, int) const {
    return student.eatChicken();
}
