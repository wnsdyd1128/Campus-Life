#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Earns money through a part-time job when energy permits.
 */
class PartTimeJobActivity : public Activity {
public:
    /**
     * @brief Creates the part-time job action with its display name.
     */
    PartTimeJobActivity();

    /**
     * @brief Spends energy and earns money for the student.
     *
     * @param student Student whose energy and money are changed.
     * @return True on success; false when energy is insufficient.
     */
    bool execute(Student& student) const;
};
