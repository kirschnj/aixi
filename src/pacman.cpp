#include "environment.hpp"

#include <cassert>

#include "util.hpp"

// Wall locations
const bool Pacman::maze[size][size] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    };


/* Utilities */
void Pacman::printWorld(void) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            switch (world[i][j]) {
                case e_empty    : std::cout << " "; break;
                case e_wall     : std::cout << "W"; break;
                case e_pacman   : std::cout << "P"; break;
                case e_ghost    : std::cout << "G"; break;
                case e_food     : std::cout << "."; break;
                default         : std::cout << "x"; break;
            }
        }
        std::cout << std::endl;
    }
}

void Pacman::updateWorldPositions(void) {
    world[pacman.row][pacman.col] = e_pacman;
    for (int i = 0; i < numGhosts; i++) {
        world[ghosts[i].row][ghosts[i].col] = e_ghost;
    }
}

Pacman::Pacman(options_t &options) {
    // TODO: options
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            world[i][j] = maze[i][j];
        }
    }
    // initial locations
    pacman.row = 12;
    pacman.col = 9;

    ghosts[0].row = 7;
    ghosts[0].col = 9;
    ghosts[1].row = 7;
    ghosts[1].col = 10;
    ghosts[2].row = 8;
    ghosts[2].col = 9;
    ghosts[3].row = 8;
    ghosts[3].col = 10;

    updateWorldPositions();

    // TODO: Set up the initial observation
    m_observation = 0;
    m_reward = 0;
}

void Pacman::performAction(action_t action) {

}
