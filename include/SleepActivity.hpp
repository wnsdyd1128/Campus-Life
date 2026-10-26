#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Restores a student's energy through sleep.
 */
class SleepActivity : public Activity {
public:
    /**
     * @brief Supplies the sleep action's display name.
     *
     * @return The sleep action label.
     */
    std::string getName() const override;

    /**
     * @brief Restores energy up to the student's maximum.
     *
     * @param student Student whose energy is restored.
     * @param burnout Unused by this action.
     * @param courseIndex Unused by this action.
     * @return Always true.
     */
    bool execute(Student& student, bool burnout = false,
                 int courseIndex = -1) const override;
};
