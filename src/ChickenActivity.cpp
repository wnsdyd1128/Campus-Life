#include "ChickenActivity.hpp"

#include "Student.hpp"

ChickenActivity::ChickenActivity()
    : Activity{ "Eat chicken" } {
}

bool ChickenActivity::execute(Student& student) const {
    return student.eatChicken();
}
