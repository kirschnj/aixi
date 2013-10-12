#include "environment.cpp"

#include <cassert>

#include "util.hpp"

int main(int argc, char *argv[]) {
    std::cout << "Pacman testing." << std::endl;

    options_t options;
    Pacman *p = new Pacman(options);

    p->printWorld();
}

