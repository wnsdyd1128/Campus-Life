#include "Student.hpp"

#include <iostream>

Student::Student()
    : name{ "Noname" },
      money{ 50000 },
      energy{ 100 },
      courses{
          Course{ "Object-Oriented Programming 2" },
          Course{ "Data Structure" },
          Course{ "Computer Architecture" }
      } {
}

Student::Student(const std::string& name, int money, int energy)
    : name{ name },
      money{ 50000 },
      energy{ 100 },
      courses{
          Course{ "Object-Oriented Programming 2" },
          Course{ "Data Structure" },
          Course{ "Computer Architecture" }
      } {
    setMoney(money).setEnergy(energy);
}

Student::~Student() = default;

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
int Student::getCourseCount() { return COURSE_COUNT; }

Course& Student::getCourse(int index) { return courses[index]; }
const Course& Student::getCourse(int index) const { return courses[index]; }

bool Student::study(int courseIndex) {
    if (courseIndex < 0 || courseIndex >= COURSE_COUNT || energy < 25) {
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
    for (int index = 0; index < COURSE_COUNT; ++index) {
        std::cout << "  ";
        courses[index].printInfo();
    }
}
