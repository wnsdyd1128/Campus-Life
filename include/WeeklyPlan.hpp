#pragma once

#include <string>

/**
 * @brief Stores one day's selected action without executing it.
 */
struct PlannedAction {
    /** Selected action label. */
    std::string action{ "None" };

    /** Course index for study actions, or -1 when no course is required. */
    int courseIndex{ -1 };
};

/**
 * @brief Stores five weekday actions before they are executed.
 */
class WeeklyPlan {
public:
    static constexpr int DAY_COUNT{ 5 };

    /**
     * @brief Provides mutable access to one weekday plan.
     *
     * @param day Zero-based weekday index.
     * @return The selected action for the requested day.
     * @pre @p day is in the range 0..4.
     */
    PlannedAction& operator[](int day);

    /**
     * @brief Provides read-only access to one weekday plan.
     *
     * @param day Zero-based weekday index.
     * @return The selected action for the requested day.
     * @pre @p day is in the range 0..4.
     */
    const PlannedAction& operator[](int day) const;

    /**
     * @brief Resets all weekdays without changing student state.
     */
    void clear();

private:
    PlannedAction days[DAY_COUNT];
};
