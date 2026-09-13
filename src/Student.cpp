#include "Student.hpp"

#include <iostream>

Student::Student()
    : name{ "Noname" }, money{ 50000 }, energy{ 100 } {
}

Student::Student(const std::string& name, int money, int energy)
    : name{ name }, money{ 50000 }, energy{ 100 } {
    setMoney(money);
    setEnergy(energy);
}

void Student::setMoney(int value) {
    if (value >= 0) {
        money = value;
    }
}

void Student::setEnergy(int value) {
    if (value < 0) value = 0;
    if (value > 100) value = 100;
    energy = value;
}

std::string Student::getName() const { return name; }
int Student::getMoney() const { return money; }
int Student::getEnergy() const { return energy; }

bool Student::study() {
    if (energy < 25) return false;
    energy -= 25;
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
}
