#pragma once

#include <string>

#include "Course.hpp"

class Student {
private:
    static constexpr int DEFAULT_COURSE_COUNT{ 3 };

    std::string name;
    int money;
    int energy;
    int courseCount;
    Course* courses;

public:
    /**
     * @brief Creates a student who owns the three default courses.
     */
    Student();

    /**
     * @brief Creates a student who owns the three default courses.
     *
     * @param name   Student name.
     * @param money  Initial money; negative values keep the default amount.
     * @param energy Initial energy, clamped to the range 0..100.
     */
    Student(const std::string& name, int money, int energy);

    /**
     * @brief Creates a student who owns a dynamically allocated course array.
     *
     * @param name        Student name.
     * @param money       Initial money; negative values keep the default amount.
     * @param energy      Initial energy, clamped to the range 0..100.
     * @param courseNames Non-null array of names when @p courseCount is positive.
     * @param courseCount Number of courses to copy; non-positive values create an empty list.
     */
    Student(const std::string& name, int money, int energy,
            const std::string courseNames[], int courseCount);

    /**
     * @brief Releases the course array owned by this student.
     */
    ~Student();

    /**
     * @brief Creates an independent deep copy of another student.
     *
     * @param other Student whose state and courses are copied.
     */
    Student(const Student& other);

    /**
     * @brief Replaces this student with an independent deep copy.
     *
     * @param other Student whose state and courses are copied.
     * @return This student after assignment.
     */
    Student& operator=(const Student& other);

    Student& setMoney(int value);
    Student& setEnergy(int value);

    std::string getName() const;
    int getMoney() const;
    int getEnergy() const;

    /**
     * @brief Returns the number of courses owned by this student.
     *
     * @return Current course count.
     */
    int getCourseCount() const;

    Course& getCourse(int index);
    const Course& getCourse(int index) const;

    bool study(int courseIndex);
    bool work();
    void sleep();
    bool eatChicken();

    void printStatus() const;
};
