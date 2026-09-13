#include <iostream>

#include "Student.hpp"

int main() {
    Student student{ "Biryong", 50000, 100 };

    std::cout << "===== CAMPUS LIFE : START =====" << std::endl;
    student.printStatus();

    std::cout << std::endl << "===== TODAY'S ACTIONS =====" << std::endl;

    if (student.study(0)) {
        std::cout << "Study: OOP2" << std::endl;
    }

    if (student.study(1)) {
        std::cout << "Study: Data Structure" << std::endl;
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

    const Student& currentStudent{ student };
    std::cout << std::endl << "Registered courses: "
              << Student::getCourseCount() << std::endl;
    currentStudent.getCourse(0).printInfo();

    return 0;
}
