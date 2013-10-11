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

#endif // __ENVIRONMENT_HPP__
