#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <queue>

#include <curses.h>

#include "util.hpp"

using namespace std;

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


// Adjacency list, used in BFS for ghost movement
const vector< vector<int> > Pacman::adjList = Pacman::genAdjList();

// Generate an adjacency list based on maze wall locations
vector< vector<int> > Pacman::genAdjList(void) {
    vector< vector<int> > aList;

    // Locations will be indexed using "(row * size) + col"
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            vector<int> tmpList;
            // Add empty list for walls
            if (!maze[i][j]) {
                // Check all adjacent locations
                if ((j > 0) && !maze[i][j - 1]) {
                    tmpList.push_back((i * size) + j - 1);
                }
                if ((j < size - 1) && !maze[i][j + 1]) {
                    tmpList.push_back((i * size) + j + 1);
                }
                if ((i > 0) && !maze[i - 1][j]) {
                    tmpList.push_back((i - 1) * size + j);
                }
                if ((i < size - 1) && !maze[i + 1][j]) {
                    tmpList.push_back((i + 1) * size + j);
                }
            }
            aList.push_back(tmpList);
        }
    }

    return aList;
}


/** Use BFS to find a shortest path between two points.
    Assumes only movement in the four cardinal directions are possible.
    @param src Start point
    @param dest Destination point
    @return action_t Corresponding to best move from src
*/
action_t Pacman::shortestMove(point &src, point &dest) {
    // Standard queue for unvisited nodes
    queue<int> q;
    // Integer representations of src and dest
    int d = (dest.row * size) + dest.col;
    int s = (src.row * size) + src.col;
    q.push(s);
    // Keep track of visited vertices, also serves to find path
    int parent[adjList.size()];
    // Initialise all elements to -1
    fill_n(parent, adjList.size(), -1);

    // Main BFS loop
    int v, n;
    while (!q.empty()) {
        // Get first element
        v = q.front();
        q.pop();

        // Check neighbours
        vector<int> neighbours = adjList.at(v);
        for (unsigned int i = 0; i < neighbours.size(); i++) {
            n = neighbours.at(i);
            // Update path
            if (parent[n] < 0) {
                parent[n] = v;
                q.push(n);
            }
        }
        // Check if we have reached dest
        if (parent[d] >= 0) break;
    }

    // Ensure that we have found a path to dest
    n = parent[d];
    assert(n >= 0);

    // Get path
    vector<int> path;
    path.push_back(d);
    while (n != s) {
        path.push_back(n);
        n = parent[n];
    }
    path.push_back(s);

    // Get first move from src
    n = path.at(path.size() - 2);
    // Map this to an action
    if (n == (s - 1)) return m_move_left;
    else if (n == (s + 1)) return m_move_right;
    else if (n < s) return m_move_up;
    return m_move_down;
}


// Initial positions.
const Pacman::point Pacman::pacman_init = {12, 9};
const Pacman::point Pacman::g_init[numGhosts] = {{7, 9}, {7, 10},{8, 9}, {8, 10}};


/* Utilities */
// Print world to stdout
void Pacman::printWorld(void) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            switch (world[i][j]) {
                case e_empty    : cout << " "; break;
                case e_wall     : cout << "\u2588"; break;
                case e_food     : cout << "\u2022"; break;
                case e_power    : cout << "+"; break;
                case e_pacman   : cout << "P"; break;
                case e_ghost    : cout << "G"; break;
                case e_gf       : cout << "G"; break;
                case e_gp       : cout << "G"; break;
                default         : cout << "x"; break;
            }
        }
        cout << endl;
    }
}

// Print a world to a curses window
void Pacman::printCurses(void) {
    for (int i = 0; i < size; i++) { addch(ACS_CKBOARD); }
    addch('\n');
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            switch (world[i][j]) {
                case e_empty    : addch(' '); break;
                case e_wall     : addch(ACS_CKBOARD); break;
                case e_food     : addch(ACS_BULLET); break;
                case e_power    : addch('+'); break;
                case e_pacman   : addch('P'); break;
                case e_ghost    : addch('G'); break;
                case e_gf       : addch('G'); break;
                case e_gp       : addch('G'); break;
                default         : addch('x'); break;
            }
        }
        addch('\n');
    }
    for (int i = 0; i < size; i++) { addch(ACS_CKBOARD); }
    printw("\n%d food pellets left.", numFood);
    addch('\n');
}

// Returns whether pacman is under the effects of a power pill
bool Pacman::powerActive(void) {
    return (powerP > 0);
}

// Update entity positions (Pacman and Ghosts)
void Pacman::updateWorldPositions(void) {
    world[pacman.row][pacman.col] = e_pacman;
    // Update ghosts
    for (int i = 0; i < numGhosts; i++) {
        // Check if ghost is idle
        if (ghostState[i] < 0) continue;
        // Otherwise, update position in world
        int r = ghosts[i].row;
        int c = ghosts[i].col;
        if (entityAt(r, c, e_food)) {
            world[r][c] = e_gf;
        } else if (entityAt(r, c, e_power)) {
            world[r][c] = e_gp;
        } else {
            world[r][c] = e_ghost;
        }
    }
}

// Check if a particular entity is at a particular position p
bool Pacman::entityAt(int row, int col, const int ent) {
    assert(row >= 0 && row < size);
    assert(col >= 0 && col < size);
    // Get object at p
    int actual = world[row][col];
    if (actual == ent) return true;
    else if (ent == e_ghost) return (actual == e_gf || actual == e_gp);
    else if (ent == e_food) return (actual == e_gf);
    else if (ent == e_power) return (actual == e_gp);
    return false;
}

// "Eat ghosts" at a particular position p
void Pacman::eatGhosts(point &p) {
    for (int i = 0; i < numGhosts; i++) {
        if ((ghosts[i].row == p.row) && (ghosts[i].col == p.col)) {
            // Ghost is idle for (manHattan dist to init) time steps
            ghostState[i] = -(abs(ghosts[i].row - g_init[i].row)
                    + abs(ghosts[i].col - g_init[i].col));
            // Send ghost back to start position
            ghosts[i] = g_init[i];
        }
    }
}

// Check if a particular entity exists within "range" units from p 
bool Pacman::entityScan(point &p, int range, const int ent) {
    // Start with square of length "range" centered on p
    for (int i = p.row - range; i < p.row + range; i++) {
        for (int j = p.col - range; j < p.col + range; j++) {
            // Only check locations within the maze
            if (i < 0 || i >= size) continue;
            if (j < 0 || j >= size) continue;
            // Discard corners (outside "range" in Manhattan distance)
            if (abs(i - p.row) + abs(j - p.col) > range) continue;
            // Check if the required entitiy is at this point
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
    bits[15] = (powerP > 0);

    // Convert bits into an unsigned int (percept_t)
    return boolToInt(bits, 16);
}


// Generate a reward, after entity position changes.
// Should be called immediately after pacman moves.
// Should also be called after moving ghosts.
int Pacman::genReward(void) {
    int value = world[pacman.row][pacman.col];
    int reward = 0;

    // note e_wall should never occur
    switch (value) {
        case e_wall : reward -= m_reward_wall; break;
        case e_food :
            reward += m_reward_food;
            // delete food (do not add pacman here yet)
            world[pacman.row][pacman.col] = e_empty;
            --numFood;
            break;
        case e_power :
            powerP = init_power_length;
            world[pacman.row][pacman.col] = e_empty;
            break;
        case e_gf :
            // If under power pill effects, eat food
            if (powerP > 0) {
                reward += m_reward_food;
                --numFood;
            }
        case e_gp :
        case e_ghost :
            // If under effects of power pill, eat ghost
            if (powerP > 0) {
                eatGhosts(pacman);
            } else {
                // Ghosts take priority over food and power pellets
                reward -= m_reward_ghost;
                endState = true;
            }
            break;
        default : break;
    }
    return reward;
}


// Move a ghost
void Pacman::moveGhost(int index) {
    int curRow = ghosts[index].row;
    int curCol = ghosts[index].col;

    action_t a;

    // Chase or move randomly based on ghostState 
    if (ghostState[index] > 0) {
        // If we have just finished chasing, set random movement period
        if (--ghostState[index] == 0) ghostWait[index] = g_wait;
        // Chase pacman
        a = shortestMove(ghosts[index], pacman);
    } else {
        // Make a random move
        // Get available movements
        vector<action_t> actions;
        if ((curCol > 0) && !maze[curRow][curCol - 1]) {
            actions.push_back((action_t) m_move_left);
        }
        if ((curCol < size - 1) && !maze[curRow][curCol + 1]) {
            actions.push_back((action_t) m_move_right);
        }
        if ((curRow > 0) && !maze[curRow - 1][curCol]) {
            actions.push_back((action_t) m_move_up);
        }
        if ((curRow < size - 1) && !maze[curRow + 1][curCol]) {
            actions.push_back((action_t) m_move_down);
        }

        // Randomly pick a move
        a = actions.at(randRange((unsigned int) actions.size()));
    } 

    switch (a) {
        case m_move_left    : ghosts[index].col = curCol - 1; break;
        case m_move_right   : ghosts[index].col = curCol + 1; break;
        case m_move_up      : ghosts[index].row = curRow - 1; break;
        case m_move_down    : ghosts[index].row = curRow + 1; break;
        // Don't move
        default : break;
    };

    // Clear previous square
    switch (world[curRow][curCol]) {
        case e_food : world[curRow][curCol] = e_food; break;
        case e_gf : world[curRow][curCol] = e_food; break;
        case e_power : world[curRow][curCol] = e_power; break;
        case e_gp : world[curRow][curCol] = e_power; break;
        default : world[curRow][curCol] = e_empty; break;
    };
}

void Pacman::reset(void) {
    // Start the game
    endState = false;
    numFood = 0;

    // Initial world configuration (walls)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            world[i][j] = maze[i][j];
        }
    }
    // Initial locations for pacman and ghosts
    pacman = (point) pacman_init;
    ghosts[0] = g_init[0];
    ghosts[1] = (point) g_init[1];
    ghosts[2] = (point) g_init[2];
    ghosts[3] = (point) g_init[3];

    // Ghosts start moving randomly
    ghostState[0] = 0;
    ghostState[1] = 0;
    ghostState[2] = 0;
    ghostState[3] = 0;

    // No chasing yet
    ghostWait[0] = 0;
    ghostWait[1] = 0;
    ghostWait[2] = 0;
    ghostWait[3] = 0;

    // Start without effects of power pellet
    powerP = 0;

    // Place power pellets
    world[1][1] = e_power;
    world[1][17] = e_power;
    world[14][1] = e_power;
    world[14][17] = e_power;

    updateWorldPositions();

    // Place food in empty locations with 0.5 probability
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int entity = world[i][j];
            if (entity == e_empty) {
                if (rand01() < 0.5) {
                    ++numFood;
                    world[i][j] = e_food;
                }
            }
        }
    }
}

/* Implementations required by Environment */
Pacman::Pacman(options_t &options) {
    reset();
    m_observation = genObservation();
    m_reward = 0;
}

void Pacman::performAction(action_t action) {
    assert(action < 4);

    // Cumulative reward
    int reward = m_reward_init;

    // Move pacman (hold position if wall is in the way)
    int newrow, newcol;
    switch (action) {
        // Wrap around possible for left/right movements
        case m_move_left :
            newcol = (pacman.col == 0) ? size - 1 : pacman.col - 1;
            if (!maze[pacman.row][newcol]) {
                world[pacman.row][pacman.col] = e_empty;
                pacman.col = newcol;
                reward += genReward();
            } else reward -= m_reward_wall;
            break;
        case m_move_right :
            newcol = (pacman.col == size - 1) ? 0 : pacman.col + 1;
            if (!maze[pacman.row][newcol]) {
                world[pacman.row][pacman.col] = e_empty;
                pacman.col = newcol;
                reward += genReward();
            } else reward -= m_reward_wall;
            break;
        // No wrap-around for up/down movements
        case m_move_up :
            newrow = pacman.row - 1;
            if (newrow >= 0 && !maze[newrow][pacman.col]) {
                world[pacman.row][pacman.col] = e_empty;
                --pacman.row;
                reward += genReward();
            } else reward -= m_reward_wall;
            break;
        case m_move_down :
            newrow = pacman.row + 1;
            if (newrow < size && !maze[newrow][pacman.col]) {
                world[pacman.row][pacman.col] = e_empty;
                ++pacman.row;
                reward += genReward();
            } else reward -= m_reward_wall;
            break;
        default : break;
    };
    // Penalty for moving
    reward -= m_reward_move;
    
    // Check if all pellets have been eaten
    if (numFood == 0) {
        reward += m_reward_win;
        endState = true;
    }

    // If the game has ended, return now.
    if (endState == true) {
        // Update percepts
        reset();
        m_observation = genObservation();
        m_reward = reward;
        return;
    }

    // If under effects of power pellet, move ghosts every 2 "turns"
    // Also, delays ghost revival
    if ((powerP % 2) == 0) {
        for (int i = 0; i < numGhosts; i++) {
            if (ghostState[i] >= 0) {
                // Start chasing if close enough
                if ((ghostState[i] == 0) && (ghostWait[i] == 0)) {
                    int dist = abs(ghosts[i].row - pacman.row)
                            + abs(ghosts[i].col - pacman.col);
                    if (dist < g_dist_chase) {
                        ghostState[i] = g_init_chase;
                    }
                } else if (ghostWait[i] > 0) {
                    --ghostWait[i];
                }
                moveGhost(i);
            } else {
                ++ghostState[i];
            }
        }
    }
    // Update world based on results of move
    updateWorldPositions();

    // Check if a ghost has caught pacman
    reward += genReward();

    // If the game has ended, return now.
    if (endState == true) {
        // Update percepts
        reset();
        m_observation = genObservation();
        m_reward = reward;
        return;
    }

    // Decrement duration of power pill if applicable
    powerP = (powerP > 0) ? powerP - 1 : 0;

    // Ensure we are up to date before generating an observation
    updateWorldPositions();
    // Update percepts
    m_observation = genObservation();
    m_reward = reward;
}


bool Pacman::isFinished(void) {
    return endState;
}
