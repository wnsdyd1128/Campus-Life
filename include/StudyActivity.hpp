#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Studies one selected course owned by a student.
 */
class StudyActivity : public Activity {
public:
    /**
     * @brief Selects the course to study when this action executes.
     *
     * @param courseIndex Zero-based index in the student's course list.
     */
    explicit StudyActivity(int courseIndex);

    /**
     * @brief Applies one study session to the selected course.
     *
     * @param student Student whose energy and course are changed.
     * @return True on success; false for an invalid course or insufficient energy.
     */
    bool execute(Student& student) const;

private:
    int courseIndex;
};
