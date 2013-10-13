#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include "main.hpp"

class Environment {

public:

	// Constructor: set up the initial environment percept
	// TODO: implement in inherited class

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action) = 0; // TODO: implement in inherited class

	// returns true if the environment cannot interact with the agent anymore
	virtual bool isFinished(void) const { return false; } // TODO: implement in inherited class (if necessary)

	void getPercept(symbol_list_t &symlist);

	percept_t getObservation(void) const { return m_observation; }

	percept_t getReward(void) const { return m_reward; }

protected: // visible to inherited classes
	action_t m_last_action;  // the last action performed by the agent
	percept_t m_observation; // the current observation
	percept_t m_reward;      // the current reward

};


// An experiment involving flipping a biased coin and having the agent predict
// whether it will come up heads or tails. The agent receives a reward of 1
// for a correct guess and a reward of 0 for an incorrect guess.
class CoinFlip : public Environment {
public:

	// set up the initial environment percept
	CoinFlip(options_t &options);

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action);

private:
	double p; // Probability of observing 1 (heads)
};


// Tiger environment
class Tiger : public Environment {
public:

	// set up the initial environment percept
	Tiger(options_t &options);

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action);

private:
	bool m_tiger_left;// Whether the tiger is behind the left door or not
	
	const static action_t m_listen_action = 0;
	const static action_t m_open_left = 1;
	const static action_t m_open_right = 2;
	const static unsigned int m_num_actions = 3;
	
	const static percept_t m_no_observation = 0;
	const static percept_t m_hear_tiger_left = 1;
	const static percept_t m_hear_tiger_right = 2;
};

// Pacman environment
class Pacman : public Environment {
public:
	// set up the initial environment percept
	Pacman(options_t &options);

	// receives the agent's action and calculates the new environment percept
	virtual void performAction(action_t action);
    virtual bool isFinished(void);

    void printWorld(void);

private:
    /* Definitions */
    struct point {
        int row;
        int col;
    };

    // State of the game
    bool endState;

    // Size of maze
    const static int size = 19;
    // Maze as shown by the diagram in the assignment spec
    const static bool maze[size][size];
    // Current world
    int world[size][size];

    /* Entities */
    // PacMan
    point pacman;
    // Power pellet
    bool power;
    int numFood;
    // Ghosts
    const static int numGhosts = 4;
    point ghosts[numGhosts];

    // Enums to distinguish between entities
    // TODO: power pellets?
    const static int e_empty = 0;
    const static int e_wall = 1;
    const static int e_food = 2;
    const static int e_pacman = 3;
    const static int e_ghost = 4;
    // Ghost and food on the same location
    const static int e_gf = 5;

    /* Actions */
    // Actions consist of a movement in some direction.
    const static action_t m_move_left = 0;
    const static action_t m_move_right = 1;
    const static action_t m_move_up = 2;
    const static action_t m_move_down = 3;
    const static unsigned int m_num_actions = 4;

    // Start with a positive reward (init=sum(negative rewards))
    // Subtract/add rewards as necessary
    const static percept_t m_reward_move = 1;
    const static percept_t m_reward_wall = 10;
    const static percept_t m_reward_ghost = 50;
    const static percept_t m_reward_food = 10;
    const static percept_t m_reward_win = 100;
    const static percept_t m_reward_init =
            m_reward_move + m_reward_wall + m_reward_ghost;
    
    /* Functions */
    void updateWorldPositions(void);
    bool entityAt(int row, int col, const int ent);
    bool entityScan(point &p, int range, const int ent);
    bool lineOfSight(point &p, action_t dir, const int ent);
    int genReward(void);
    percept_t genObservation(void);

};

#endif // __ENVIRONMENT_HPP__
