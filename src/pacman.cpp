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
                case e_wall     : std::cout << "\u2588"; break;
                case e_food     : std::cout << "\u2022"; break;
                case e_pacman   : std::cout << "P"; break;
                case e_ghost    : std::cout << "G"; break;
                case e_gf       : std::cout << "G"; break;
                default         : std::cout << "x"; break;
            }
        }
        std::cout << std::endl;
    }
}

// Update entity positions only
void Pacman::updateWorldPositions(void) {
    world[pacman.row][pacman.col] = e_pacman;
    for (int i = 0; i < numGhosts; i++) {
        int y = ghosts[i].row;
        int x = ghosts[i].col;
        world[y][x] = world[y][x] == e_food ? (int) e_gf : e_ghost;
    }
}

percept_t Pacman::getObservation(void) {
    // TODO, check local squares around pacman...
    // I'll do this tomorrow.

}

/* Implementations required by Environment */
Pacman::Pacman(options_t &options) {
    // TODO: options
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int entity = maze[i][j];
            // Place food in empty locations with 0.5 probability
            if (entity == 0) {
                entity = rand01() < 0.5 ? (int) e_food : 0;
            }
            world[i][j] = entity;
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
