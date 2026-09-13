#include <iostream>

#include "Course.hpp"
#include "Student.hpp"

int main() {
    Student student{ "Biryong", 50000, 100 };

    Course oop2{ "Object-Oriented Programming 2" };
    Course dataStructure{ "Data Structure", 60 };
    Course computerArchitecture{ "Computer Architecture" };

    Course emptyCourse{};
    Course copiedCourse{ oop2 };

    std::cout << "===== CAMPUS LIFE : START =====" << std::endl;
    student.printStatus();
    oop2.printInfo();
    dataStructure.printInfo();
    computerArchitecture.printInfo();

    std::cout << std::endl << "===== TODAY'S ACTIONS =====" << std::endl;

    if (student.study()) {
        std::cout << "Study: OOP2" << std::endl;
        oop2.study();
    }

    if (student.study()) {
        std::cout << "Study: Data Structure" << std::endl;
        dataStructure.study();
    }

    if (student.eatChicken()) {
        std::cout << "Eat chicken" << std::endl;
    }

    if (student.work()) {
        std::cout << "Part-time job" << std::endl;
    }

    student.sleep();
    std::cout << "Sleep" << std::endl;

    std::cout << std::endl << "===== CAMPUS LIFE : RESULT =====" << std::endl;
    student.printStatus();
    oop2.printInfo();
    dataStructure.printInfo();
    computerArchitecture.printInfo();

    std::cout << std::endl << "[Copy constructor check]" << std::endl;
    copiedCourse.printInfo();

    return 0;
}
