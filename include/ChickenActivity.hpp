#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Buys chicken to restore a student's energy.
 */
class ChickenActivity : public Activity {
public:
    /**
     * @brief Supplies the chicken action's display name.
     *
     * @return The chicken action label.
     */
    std::string getName() const override;

    /**
     * @brief Spends money and restores the student's energy.
     *
     * @param student Student whose money and energy are changed.
     * @param burnout Unused by this action.
     * @param courseIndex Unused by this action.
     * @return True on success; false when money is insufficient.
     */
    bool execute(Student& student, bool burnout = false,
                 int courseIndex = -1) const override;
};
