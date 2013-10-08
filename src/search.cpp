#include "search.hpp"

#include "agent.hpp"

typedef unsigned long long visits_t;


// search options
static const visits_t     MinVisitsBeforeExpansion = 1;
static const unsigned int MaxDistanceFromRoot  = 100;
static size_t             MaxSearchNodes;

// contains information about a single "state"
class SearchNode {

public:

	SearchNode(bool is_chance_node);

	// determine the next action to play
	action_t selectAction(Agent &agent) const; // TODO: implement

	// determine the expected reward from this node
	reward_t expectation(void) const { return m_mean; }

	// perform a sample run through this node and it's children,
	// returning the accumulated reward from this sample run
	reward_t sample(Agent &agent, unsigned int dfr); // TODO: implement

	// number of times the search node has been visited
	visits_t visits(void) const { return m_visits; }

private:

	bool m_chance_node; // true if this node is a chance node, false otherwise
	double m_mean;      // the expected reward of this node
	visits_t m_visits;  // number of times the search node has been visited

	// TODO: decide how to reference child nodes
	//  e.g. a fixed-size array
};

// simulate a path through a hypothetical future for the agent within it's
// internal model of the world, returning the accumulated reward.
static reward_t playout(Agent &agent, unsigned int playout_len) {
	return 0; // TODO: implement
	/*
		'RollOut' function from the paper.
		Pick actions randomly until playout_len is reached.
		Use agent.getRandomAction() and .genPercept();
		Question: Would the agent's CTW of the environment need to be updated
			while doing this, (and then reverted) or is it not relevant?
	*/
}

// determine the best action by searching ahead using MCTS
extern action_t search(Agent &agent) {
	return agent.genRandomAction(); // TODO: implement
	/*
		Initialise the search tree (non-chance node)
		start timing up to cycle-length-millis
		while ( still more time || less than mc-simulations completed) {
			searchTree.sample(agent, whatever dfr is meant to be (search horizon m i think));
		}
		look through all the successors of the root of our search tree:
		pick the action corresponding to the successor with highest m_mean/expectation().
	*/
}

