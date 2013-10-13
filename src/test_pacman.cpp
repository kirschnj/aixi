#include "environment.cpp"

#include <cassert>

#include "util.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Pacman testing." << std::endl;
    options_t options;

    // Test bool to int conversion
    const int n = 4;
    bool a[n] = {true, true, false, true};
    std::cout << "boolean to int: " << boolToInt(a, n) << std::endl;

    Pacman *p = new Pacman(options);
    p->printWorld();
    std::cout << "Observation: " << p->getObservation() << std::endl;

    return 0;
}

