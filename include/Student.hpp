#pragma once

#include <string>

class Student {
private:
    std::string name;
    int money;
    int energy;

public:
    Student();
    Student(const std::string& name, int money, int energy);

    void setMoney(int value);
    void setEnergy(int value);

    std::string getName() const;
    int getMoney() const;
    int getEnergy() const;

    bool study();
    bool work();
    void sleep();
    bool eatChicken();

    void printStatus() const;
};
