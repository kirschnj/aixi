#include "environment.cpp"

#include <cassert>
#include <cstdlib>

#include "util.hpp"

using namespace std;

// Very basic testing..
int main(int argc, char *argv[]) {
    system("clear");
    options_t options;

    // Test bool to int conversion
    /*
    const int n = 4;
    bool a[n] = {true, true, false, true};
    std::cout << "boolean to int: " << boolToInt(a, n) << std::endl;
    */

    Pacman *p = new Pacman(options);
    cout << "Pacman initial world." << endl;
    p->printWorld();
    cout << "Observation: " << p->getObservation() << endl;
    cout << "Reward: " << p->getReward() << endl;

    char input;
    while (!p->isFinished()) {
        cout << "Make a move (wasd): ";
        cin >> input;
        cout << input << endl;
        system("clear");
        switch (input) {
            case 'w' : p->performAction((action_t) 2); break;
            case 'a' : p->performAction((action_t) 0); break;
            case 's' : p->performAction((action_t) 3); break;
            case 'd' : p->performAction((action_t) 1); break;
            default : p->performAction((action_t) 0); break;
        };
        p->printWorld();
        cout << "Observation: " << p->getObservation() << endl;
        cout << "Reward: " << p->getReward() << endl;
    }

    return 0;
}

