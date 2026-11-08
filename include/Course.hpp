#pragma once

#include <iostream>
#include <string>

class Course {
private:
    std::string name;
    std::string grade;
    int progress;

public:
    Course();
    explicit Course(const std::string& name);
    Course(const std::string& name, int progress);
    Course(const Course& other);

    void setProgress(int value);

    std::string getName() const;
    std::string getGrade() const;
    int getProgress() const;

    /**
     * @brief Restores a grade and progress only when both satisfy course rules.
     * @param savedGrade Grade C, B, A, or A+.
     * @param savedProgress Progress in 0..99; A+ requires zero.
     * @return True on success; false leaves the course unchanged.
     */
    bool restoreState(const std::string& savedGrade, int savedProgress);

    /**
     * @brief Applies study points while preserving the course grade invariant.
     *
     * Reaching 100 points advances one grade and resets progress to zero.
     * Non-positive points and completed A+ courses are left unchanged.
     *
     * @param point Number of study points to apply.
     * @return This course, allowing chained additions.
     */
    Course& operator+=(int point);

    /**
     * @brief Applies the grade's study points, halved during a burnout week.
     *
     * @param burnout Whether to halve points using integer division.
     */
    void study(bool burnout = false);
    void printInfo() const;
};

/**
 * @brief Writes the course name, grade, and progress to a stream.
 *
 * @param out    Destination stream.
 * @param course Course to format.
 * @return The destination stream.
 */
std::ostream& operator<<(std::ostream& out, const Course& course);
