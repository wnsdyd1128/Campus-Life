#pragma once

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

    void study();
    void printInfo() const;
};
