#pragma once

#include <string>

#include "Course.hpp"

class Student {
private:
    static constexpr int COURSE_COUNT{ 3 };

    std::string name;
    int money;
    int energy;
    Course courses[COURSE_COUNT];

public:
    Student();
    Student(const std::string& name, int money, int energy);
    ~Student();

    Student& setMoney(int value);
    Student& setEnergy(int value);

    std::string getName() const;
    int getMoney() const;
    int getEnergy() const;
    static int getCourseCount();

    Course& getCourse(int index);
    const Course& getCourse(int index) const;

    bool study(int courseIndex);
    bool work();
    void sleep();
    bool eatChicken();

    void printStatus() const;
};
