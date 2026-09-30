#include "Activity.hpp"

Activity::Activity(const std::string& name)
    : name{ name } {
}

std::string Activity::getName() const {
    return name;
}
