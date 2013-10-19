#include "environment.cpp"

#include <cassert>
#include <cstdlib>
#include <curses.h>

#include "util.hpp"

using namespace std;

// Very basic testing..
int main(int argc, char *argv[]) {
    options_t options;

    // Initialise
    initscr();
    clear();
    Pacman *p = new Pacman(options);

    // Screen variables
    int width, height;
    getmaxyx(stdscr, height, width);
    if (width < 60 || height < 20) {
        endwin();
        cout << "Please increase your terminal size and run again." << endl;
        return 1;
    }

    p->printCurses();
    move(0, 20);
    addstr("Observation: ");
    printw("%d", p->getObservation());
    move(1, 20);
    addstr("Reward: ");
    printw("%d", p->getReward());
    move(2, 20);
    addstr("Move using (wasd). q to quit.");

    char input;
    while (!p->isFinished()) {
        input = getch();
        clear();
        switch (input) {
            case 'w' : p->performAction((action_t) 2); break;
            case 'a' : p->performAction((action_t) 0); break;
            case 's' : p->performAction((action_t) 3); break;
            case 'd' : p->performAction((action_t) 1); break;
            case 'q' : endwin(); return 0;
            default : break;
        };
        p->printCurses();
        move(0, 20);
        addstr("Observation: ");
        printw("%d", p->getObservation());
        move(1, 20);
        addstr("Reward: ");
        printw("%d", p->getReward());
        if (p->powerActive()) {
            move(2, 20);
            addstr("Power pill active.");
        }
    }

    clear();
    p->printCurses();
    addstr("Game over. Press any key to quit...");
    move(1, 20);
    addstr("Reward: ");
    printw("%d", p->getReward());
    input = getch();

    endwin();
    return 0;
}
