#include "environment.hpp"

#include <cassert>
#include <cmath>

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

// Check if a particular entity is at a particular position p
bool Pacman::entityAt(int row, int col, const int ent) {
    assert(row >= 0 && row < size);
    assert(col >= 0 && col < size);
    int actual = world[row][col];
    if (actual == ent) return true;
    else if (ent == e_ghost) return actual == e_gf;
    else if (ent == e_food) return actual == e_gf;
    return false;
}

// Check if a particular entity exists within "range" units away from p 
bool Pacman::entityScan(point &p, int range, const int ent) {
    // Start with square of length "range" centered on p
    for (int i = p.row - range; i < p.row + range; i++) {
        for (int j = p.col - range; j < p.col + range; j++) {
            // Only check locations within the maze
            if (i < 0 || i >= size) continue;
            if (j < 0 || j >= size) continue;
            // Discard corners (outside "range" in Manhattan distance)
            if (std::abs(i - p.row) + std::abs(j - p.col) > range) continue;

            if (entityAt(i, j, ent)) return true;
        }
    }
    return false;
}


/** Check if an entity is in line of sight
    @param p Current position
    @param dir Cardinal direction for sight
    @param ent The entity to check for
    @return boolean, True if the entity can be seen
*/
bool Pacman::lineOfSight(point &p, action_t dir, const int ent) {
    assert(dir < m_num_actions);
    switch (dir) {
        case m_move_left :
            for (int i = p.col; i >= 0; --i) {
                if (world[p.row][i] == e_wall) return false;
                if (entityAt(p.row, i, ent)) return true;
            }
            break;
        case m_move_right :
            for (int i = p.col; i < size; ++i) {
                if (world[p.row][i] == e_wall) return false;
                if (entityAt(p.row, i, ent)) return true;
            }
            break;
        case m_move_up :
            for (int i = p.row; i >= 0; --i) {
                if (world[i][p.col] == e_wall) return false;
                if (entityAt(i, p.col, ent)) return true;
            }
            break;
        case m_move_down :
            for (int i = p.row; i < size; ++i) {
                if (world[i][p.col] == e_wall) return false;
                if (entityAt(i, p.col, ent)) return true;
            }
            break;
        default : return 0;
    };
    return 0;
}

// Output a percept corresponding to pacman's current observation
percept_t Pacman::genObservation(void) {
    // Hardcoded observation size
    bool bits[16];

    int curRow = pacman.row;
    int curCol = pacman.col;

    // 4 bits for wall configuration
    // Left
    bits[0] = (curCol - 1 < 0) ? 1 : maze[curRow][curCol - 1];
    // Right
    bits[1] = (curCol + 1 == size) ? 1 : maze[curRow][curCol + 1];
    // Up
    bits[2] = (curRow - 1 < 0) ? 1 : maze[curRow - 1][curCol];
    // Down
    bits[3] = (curRow + 1 == size) ? 1 : maze[curRow + 1][curCol];

    // 4 bits for ghost visibility (line of sight)
    bits[4] = lineOfSight(pacman, m_move_left, e_ghost);
    bits[5] = lineOfSight(pacman, m_move_right, e_ghost);
    bits[6] = lineOfSight(pacman, m_move_up, e_ghost);
    bits[7] = lineOfSight(pacman, m_move_down, e_ghost);

    // 3 bits for food "smell" (Manhattan Distance)
    // Does food exist within 2 units?
    bits[8] = entityScan(pacman, 2, e_food);
    // Does food exist within 3 units?
    bits[9] = entityScan(pacman, 3, e_food);
    // Does food exist within 4 units?
    bits[10] = entityScan(pacman, 4, e_food);

    // 4 bits for food visibility (line of sight)
    bits[11] = lineOfSight(pacman, m_move_left, e_food);
    bits[12] = lineOfSight(pacman, m_move_right, e_food);
    bits[13] = lineOfSight(pacman, m_move_up, e_food);
    bits[14] = lineOfSight(pacman, m_move_down, e_food);

    // 1 bit for power pellet
    bits[15] = power;

    // TODO: DEBUG
    for (int i = 0; i < 16; i++) {
        std::cout << bits[i];
    }
    std::cout << std::endl;

    // Convert bits into an unsigned int (percept_t)
    return boolToInt(bits, 16);
}


// Generate a reward, after pacman moves to a new square
// Must be called before updating world positions.
int Pacman::genReward(void) {
    int value = world[pacman.row][pacman.col];
    int reward = 0;

    switch (value) {
        case e_wall : reward -= m_reward_wall;
        case e_food : reward += m_reward_food;
        case e_ghost : reward -= m_reward_ghost;
        case e_gf : reward = reward + m_reward_food - m_reward_ghost;
        default : break;
    }
    return reward;
}

/* Implementations required by Environment */
Pacman::Pacman(options_t &options) {
    // TODO: options
    numFood = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int entity = maze[i][j];
            // Place food in empty locations with 0.5 probability
            if (entity == 0) {
                entity = rand01() < 0.5 ? (int) e_food : 0;
                ++numFood;
            }
            world[i][j] = entity;
        }
    }
    // initial locations
    pacman.row = 12;
    pacman.col = 9;
    // Start without effects of power pellet
    power = 0;

    ghosts[0].row = 7;
    ghosts[0].col = 9;
    ghosts[1].row = 7;
    ghosts[1].col = 10;
    ghosts[2].row = 8;
    ghosts[2].col = 9;
    ghosts[3].row = 8;
    ghosts[3].col = 10;

    updateWorldPositions();

    m_observation = genObservation();
    m_reward = 0;
}

void Pacman::performAction(action_t action) {
    assert(action < 4);
    // Cumulative reward
    int reward = m_reward_init;

    // Move pacman (hold position if wall is in the way)
    switch (action) {
        case m_move_left :
            if (!maze[pacman.row][pacman.col - 1]) {
                world[pacman.row][pacman.col] = e_empty;
                --pacman.col;
                reward += getReward();
            } else reward -= m_reward_wall;
            break;
        case m_move_right :
            if (!maze[pacman.row][pacman.col + 1]) {
                world[pacman.row][pacman.col] = e_empty;
                --pacman.col;
                reward += getReward();
            } else reward -= m_reward_wall;
            break;
        case m_move_up :
            if (!maze[pacman.row - 1][pacman.col]) {
                world[pacman.row][pacman.col] = e_empty;
                --pacman.col;
                reward += getReward();
            } else reward -= m_reward_wall;
            break;
        case m_move_down :
            if (!maze[pacman.row + 1][pacman.col]) {
                world[pacman.row][pacman.col] = e_empty;
                --pacman.col;
                reward += getReward();
            } else reward -= m_reward_wall;
            break;
        default : break;
    };
    // Penalty for moving
    reward -= m_reward_move;
    
    // Check if all pellets have been eaten
    if (numFood = 0) {
        reward += m_reward_win;
        // TODO: reset world
        return;
    }

    // TODO: make ghosts move...


    updateWorldPositions();

}
