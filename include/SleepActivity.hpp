#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Restores a student's energy through sleep.
 */
class SleepActivity : public Activity {
public:
    /**
     * @brief Creates the sleep action with its display name.
     */
    SleepActivity();

    /**
     * @brief Restores energy up to the student's maximum.
     *
     * @param student Student whose energy is restored.
     * @return Always true.
     */
    bool execute(Student& student) const;
};
