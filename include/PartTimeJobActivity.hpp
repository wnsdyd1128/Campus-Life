#pragma once

#include "Activity.hpp"

class Student;

/**
 * @brief Earns money through a part-time job when energy permits.
 */
class PartTimeJobActivity : public Activity {
public:
    /**
     * @brief Supplies the part-time job action's display name.
     *
     * @return The part-time job action label.
     */
    std::string getName() const override;

    /**
     * @brief Spends energy and earns money for the student.
     *
     * @param student Student whose energy and money are changed.
     * @param burnout Unused by this action.
     * @param courseIndex Unused by this action.
     * @return True on success; false when energy is insufficient.
     */
    bool execute(Student& student, bool burnout = false,
                 int courseIndex = -1) const override;
};
