#pragma once

#include <string>

#include "ChickenActivity.hpp"
#include "PartTimeJobActivity.hpp"
#include "SleepActivity.hpp"
#include "Student.hpp"
#include "StudyActivity.hpp"
#include "WeeklyPlan.hpp"

/** @brief Captures visible student state around one day's action. */
struct DayResult {
    int energyBefore{};
    int energyAfter{};
    int moneyBefore{};
    int moneyAfter{};
    bool hasCourse{ false };
    std::string gradeBefore;
    std::string gradeAfter;
    int progressBefore{};
    int progressAfter{};
};

/**
 * @brief Records weekday outcomes and weekend recovery for one simulation.
 */
struct WeekResult {
    /** False means the action failed or the day had no selected action. */
    bool succeeded[WeeklyPlan::DAY_COUNT]{};
    /** State before and after each weekday action. */
    DayResult days[WeeklyPlan::DAY_COUNT];
    /** Energy immediately before weekend recovery. */
    int weekendEnergyBefore{};
    /** Energy after weekend recovery, capped at 100. */
    int weekendEnergyAfter{};
};

/**
 * @brief Owns a student, reusable actions, and the plan that borrows those actions.
 */
class CampusLifeGame {
public:
    /**
     * @brief Starts at week one with an independent copy of the student.
     *
     * @param student Initial student state to copy.
     */
    explicit CampusLifeGame(const Student& student);

    /** @brief Prevents copying a plan whose pointers refer to this game's actions. */
    CampusLifeGame(const CampusLifeGame&) = delete;
    /** @brief Prevents replacing a game with pointers to another game's actions. */
    CampusLifeGame& operator=(const CampusLifeGame&) = delete;

    /**
     * @brief Provides read-only access to the game's student.
     *
     * @return The student owned by this game.
     */
    const Student& getStudent() const;

    /**
     * @brief Provides read-only access to the selected actions.
     *
     * @return The plan whose action pointers remain valid for this game's lifetime.
     */
    const WeeklyPlan& getPlan() const;

    /**
     * @brief Returns the next week to simulate.
     *
     * @return One-based week number.
     */
    int getWeek() const;

    /**
     * @brief Reports whether the last simulation applied burnout study points.
     *
     * @return True if the most recently simulated week had burnout.
     */
    bool getCurrentBurnout() const;

    /**
     * @brief Reports whether the next simulation will apply burnout.
     *
     * @return True if a successful action reached zero energy this week.
     */
    bool getNextBurnout() const;

    /**
     * @brief Selects study without changing the student.
     *
     * @param day Zero-based weekday index in 0..4.
     * @param courseIndex Course index checked when the action executes.
     */
    void planStudy(int day, int courseIndex);

    /**
     * @brief Selects a part-time job without changing the student.
     *
     * @param day Zero-based weekday index in 0..4.
     */
    void planWork(int day);

    /**
     * @brief Selects sleep without changing the student.
     *
     * @param day Zero-based weekday index in 0..4.
     */
    void planSleep(int day);

    /**
     * @brief Selects chicken without changing the student.
     *
     * @param day Zero-based weekday index in 0..4.
     */
    void planChicken(int day);

    /**
     * @brief Executes Monday through Friday and applies 35 weekend energy.
     *
     * Failed or empty days do not stop later actions. Only successful actions
     * reaching zero energy reserve burnout, which recovery does not cancel.
     * The plan remains available for the next week.
     *
     * @return Success flags for the five weekdays.
     */
    WeekResult simulateWeek();

private:
    Student student;
    StudyActivity study;
    PartTimeJobActivity work;
    SleepActivity sleep;
    ChickenActivity chicken;
    WeeklyPlan plan;
    int week{ 1 };
    bool currentBurnout{ false };
    bool nextBurnout{ false };
};
