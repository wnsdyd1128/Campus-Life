#include "Course.hpp"

#include <iostream>

Course::Course()
    : name{ "None" }, grade{ "C" }, progress{ 0 } {
}

Course::Course(const std::string& name)
    : name{ name }, grade{ "C" }, progress{ 0 } {
}

Course::Course(const std::string& name, int progress)
    : name{ name }, grade{ "C" }, progress{ 0 } {
    setProgress(progress);
}

Course::Course(const Course& other)
    : name{ other.name }, grade{ other.grade }, progress{ other.progress } {
}

void Course::setProgress(int value) {
    if (value >= 0 && value < 100) {
        progress = value;
    }
}

std::string Course::getName() const { return name; }
std::string Course::getGrade() const { return grade; }
int Course::getProgress() const { return progress; }

Course& Course::operator+=(int point) {
    if (point <= 0 || grade == "A+") {
        return *this;
    }

    progress += point;
    if (progress >= 100) {
        progress = 0;
        if (grade == "C") grade = "B";
        else if (grade == "B") grade = "A";
        else if (grade == "A") grade = "A+";
    }

    return *this;
}

void Course::study() {
    int point = 0;
    if (grade == "C") point = 40;
    else if (grade == "B") point = 25;
    else if (grade == "A") point = 20;

    *this += point;
}

void Course::printInfo() const {
    std::cout << *this << std::endl;
}

std::ostream& operator<<(std::ostream& out, const Course& course) {
    out << course.getName() << " : " << course.getGrade();
    if (course.getGrade() != "A+") {
        out << " [" << course.getProgress() << "/100]";
    }
    return out;
}
