#include "StudyActivity.hpp"

#include "Student.hpp"

std::string StudyActivity::getName() const {
    return "Study";
}

bool StudyActivity::execute(Student& student, bool burnout, int courseIndex) const {
    return student.study(courseIndex, burnout);
}
