#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Buys chicken to restore a student's energy.
 */
class ChickenActivity : public Activity {
public:
    /**
     * @brief Creates the chicken action with its display name.
     */
    ChickenActivity();

    /**
     * @brief Spends money and restores the student's energy.
     *
     * @param student Student whose money and energy are changed.
     * @return True on success; false when money is insufficient.
     */
    bool execute(Student& student) const;
};
