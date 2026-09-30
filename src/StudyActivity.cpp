#include "StudyActivity.hpp"

#include "Student.hpp"

StudyActivity::StudyActivity(int courseIndex)
    : Activity{ "Study" }, courseIndex{ courseIndex } {
}

bool StudyActivity::execute(Student& student) const {
    return student.study(courseIndex);
}
