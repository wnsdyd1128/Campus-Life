#include "Student.hpp"

#include <iostream>

namespace {
const std::string DEFAULT_COURSE_NAMES[]{
    "Object-Oriented Programming 2",
    "Data Structure",
    "Computer Architecture"
};
}

Student::Student()
    : Student{ "Noname", 50000, 100 } {
}

Student::Student(const std::string& name, int money, int energy)
    : Student{ name, money, energy,
               DEFAULT_COURSE_NAMES, DEFAULT_COURSE_COUNT } {
}

Student::Student(const std::string& name, int money, int energy,
                 const std::string courseNames[], int courseCount)
    : name{ name },
      money{ 50000 },
      energy{ 100 },
      courseCount{ courseCount > 0 ? courseCount : 0 },
      courses{ this->courseCount > 0 ? new Course[this->courseCount] : nullptr } {
    setMoney(money).setEnergy(energy);

    for (int index = 0; index < this->courseCount; ++index) {
        courses[index] = Course{ courseNames[index] };
    }
}

Student::~Student() {
    delete[] courses;
}

Student::Student(const Student& other)
    : name{ other.name },
      money{ other.money },
      energy{ other.energy },
      courseCount{ other.courseCount },
      courses{ courseCount > 0 ? new Course[courseCount] : nullptr } {
    for (int index = 0; index < courseCount; ++index) {
        courses[index] = other.courses[index];
    }
}

Student& Student::operator=(const Student& other) {
    if (this == &other) {
        return *this;
    }

    Course* copiedCourses =
        other.courseCount > 0 ? new Course[other.courseCount] : nullptr;

    for (int index = 0; index < other.courseCount; ++index) {
        copiedCourses[index] = other.courses[index];
    }

    delete[] courses;

    name = other.name;
    money = other.money;
    energy = other.energy;
    courseCount = other.courseCount;
    courses = copiedCourses;

    return *this;
}

Student& Student::setMoney(int value) {
    if (value >= 0) {
        this->money = value;
    }
    return *this;
}

Student& Student::setEnergy(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    this->energy = value;
    return *this;
}

std::string Student::getName() const { return name; }
int Student::getMoney() const { return money; }
int Student::getEnergy() const { return energy; }
int Student::getCourseCount() const { return courseCount; }

Course& Student::getCourse(int index) { return courses[index]; }
const Course& Student::getCourse(int index) const { return courses[index]; }

bool Student::study(int courseIndex) {
    if (courseIndex < 0 || courseIndex >= courseCount || energy < 25) {
        return false;
    }

    energy -= 25;
    courses[courseIndex].study();
    return true;
}

bool Student::work() {
    if (energy < 40) return false;
    energy -= 40;
    money += 40000;
    return true;
}

void Student::sleep() {
    energy += 10;
    if (energy > 100) energy = 100;
}

bool Student::eatChicken() {
    if (money < 25000) return false;
    money -= 25000;
    energy += 30;
    if (energy > 100) energy = 100;
    return true;
}

void Student::printStatus() const {
    std::cout << "Name   : " << name << std::endl;
    std::cout << "Money  : " << money << " won" << std::endl;
    std::cout << "Energy : " << energy << " / 100" << std::endl;

    std::cout << "Courses" << std::endl;
    for (int index = 0; index < courseCount; ++index) {
        std::cout << "  ";
        courses[index].printInfo();
    }
}
