#pragma once

#include <string>

/**
 * @brief Holds the name shared by all campus-life actions.
 */
class Activity {
public:
    /**
     * @brief Creates a named action for a derived class.
     *
     * @param name Name displayed for this action.
     */
    explicit Activity(const std::string& name);

    /**
     * @brief Returns the action's display name.
     *
     * @return Display name supplied to the base constructor.
     */
    std::string getName() const;

private:
    std::string name;
};
