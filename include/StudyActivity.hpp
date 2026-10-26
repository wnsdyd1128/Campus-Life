#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Studies the course selected in the daily plan.
 */
class StudyActivity : public Activity {
public:
    /**
     * @brief Supplies the study action's display name.
     *
     * @return The study action label.
     */
    std::string getName() const override;

    /**
     * @brief Applies one study session to the selected course.
     *
     * @param student Student whose energy and course are changed.
     * @param burnout Whether to apply half study points using integer division.
     * @param courseIndex Zero-based index in the student's course list.
     * @return True on success; false for an invalid course or insufficient energy.
     */
    bool execute(Student& student, bool burnout = false,
                 int courseIndex = -1) const override;
};
