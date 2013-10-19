#ifndef __SEARCH_HPP__
#define __SEARCH_HPP__

#include "main.hpp"
#include <map>

class Agent;

// determine the best action by searching ahead
extern action_t search(Agent &agent, timelimit_t mc_timelimit);

typedef unsigned long long visits_t;

// contains information about a single "state"
class SearchNode {

public:

	SearchNode(bool is_chance_node, unsigned int num_actions);
    
    ~SearchNode();
    
	// determine the next action to play
	action_t selectAction(Agent &agent, unsigned int dfr); 

	// determine the expected reward from this node
	reward_t expectation(void) const { return m_mean; }

	// perform a sample run through this node and it's children,
	// returning the accumulated reward from this sample run
	reward_t sample(Agent &agent, unsigned int dfr); 

	// number of times the search node has been visited
	visits_t visits(void) const { return m_visits; }
	
	SearchNode* child(unsigned int n) { return m_child[n]; }

private:

	bool m_chance_node; // true if this node is a chance node, false otherwise
	double m_mean;      // the expected reward of this node
	visits_t m_visits;  // number of times the search node has been visited

	// TODO: decide how to reference child nodes
	//  e.g. a fixed-size array
	std::map<unsigned int, SearchNode*> m_child;
    std::vector<action_t> m_unexplored_actions;
};


#endif // __SEARCH_HPP__
