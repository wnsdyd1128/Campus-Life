#pragma once

#include <string>

class Student;

/**
 * @brief Defines the common interface for concrete campus-life actions.
 */
class Activity {
public:
    /**
     * @brief Allows derived actions to be destroyed through a base pointer.
     */
    virtual ~Activity() = default;

    /**
     * @brief Returns the action's display name.
     *
     * @return Display name provided by the concrete action.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Executes an action without taking ownership of the student.
     *
     * @param student Student whose resources or course state may change.
     * @param burnout Whether study points are halved for the current week.
     * @param courseIndex Course to study; ignored by other actions.
     * @return True on success; false leaves the student's state unchanged.
     */
    virtual bool execute(Student& student, bool burnout = false,
                         int courseIndex = -1) const = 0;
};
