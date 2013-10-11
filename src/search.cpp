#include "search.hpp"

#include "agent.hpp"

#include "util.hpp"

typedef unsigned long long visits_t;


// search options
static const visits_t     MinVisitsBeforeExpansion = 1;
static const unsigned int MaxDistanceFromRoot  = 100;// Note, Agent defines it's own horizon size;
static size_t             MaxSearchNodes;

// contains information about a single "state"
class SearchNode {

public:

	SearchNode(bool is_chance_node, unsigned int num_actions,
	    unsigned int num_percepts);

	// determine the next action to play
	action_t selectAction(Agent &agent, unsigned int dfr) const; // TODO: implement

	// determine the expected reward from this node
	reward_t expectation(void) const { return m_mean; }

	// perform a sample run through this node and it's children,
	// returning the accumulated reward from this sample run
	reward_t sample(Agent &agent, unsigned int dfr); // TODO: implement

	// number of times the search node has been visited
	visits_t visits(void) const { return m_visits; }
	
	SearchNode* child(unsigned int n) { return child[n]; }

private:

	bool m_chance_node; // true if this node is a chance node, false otherwise
	double m_mean;      // the expected reward of this node
	visits_t m_visits;  // number of times the search node has been visited

	// TODO: decide how to reference child nodes
	//  e.g. a fixed-size array
	SearchNode* child[];
};

SearchNode::SearchNode(bool is_chance_node,
    unsigned int num_actions, unsigned int num_percepts)
: m_chance_node(is_chance_node) {
    if (m_chance_node) {
        child = SearchNode*[num_percepts];
    } else {
        child = SearchNode*[num_actions];
    }
}

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

action_t SearchNode::selectAction(Agent& agent, unsigned int dfr) {
    std::vector<action_t> unexplored_actions;
    for (action_t a = 0; a < agent.numActions(); ++a) {
        if (child[a] == NULL) {
            unexplored_actions.push_back(a);
        }
    }
    if (!unexplored_actions.empty()) {
        action_t action = unexplored_actions.at(
                                randRange(unexplored_actions.size()));
        child[action] = SearchNode(true, agent.numActions(), agent.numPercepts());
        return action;
    } else {
        action_t arg_max = 0;
        double max = (1.0 / (dfr * agent.maxReward())) * child[0].m_mean
                    + C * sqrt((log(m_visits)/child[0].m_visits));
        for (action_t a = 1; a < agent.numActions(); ++a) {
            double f = (1.0 / (dfr * agent.maxReward())) * child[a].m_mean
                    + C * sqrt((log(m_visits)/child[a].m_visits));
            // Notes: agent.minReward() defined as 0, so omitted.
            // TODO Need constant C defined somewhere.
            if (f > max) {
                max = f;
                arg_max = action;
            }
        }
        
        return arg_max;
    }
}

reward_t SearchNode::sample(Agent &agent, unsigned int dfr) {
    double newReward;
    if (dfr == 0) {
        return 0;// TODO catch this earlier, no point generating any nodes at 
                // this depth. Waste of space.
    } else if (m_chance_node) {
        // Generate observation reward percept.
        // decode reward into an int r.
        // decode whole percept into int percept_int
        if (child[percept_int] == NULL) {
            child[percept_int] = SearchNode(false,
                                    agent.numActions(), agent.numPercepts());
        }
        newReward = r + child[percept_int].sample(agent, dfr - 1);
    } else if {m_visits == 0) {
        newReward = playout(agent, dfr);
    } else {
        action_t action = selectAction(agent);
        newReward = child[action].sample(agent, drf);
    }
    m_mean = (1.0 / (double) (m_visits + 1)) * (newReward + m_visits * m_mean);
    ++m_visits;
    return newReward;
}

// determine the best action by searching ahead using MCTS
extern action_t search(Agent &agent) {
    SearchNode search_tree(false, agent.numActions(), agent.numPercepts());
    
    // TODO Timing
    while (timeNow < maxGivenTime /*TODO*/) {
        search_tree.sample(agent, agent.horizon());
    }
    
    // TODO assumes all actions sampled at least once (children exist).
    double best_reward = search_tree.child(0).expectation();
    unsigned int best_action = 0;
    for (unsigned int i = 1; i < agent.numActions(); ++i) {
        if (search_tree.child(i).expectation() > best_reward) {
            best_reward = search_tree.child(i).expectation();
            best_action = i;
        }
    }
    
    return best_action;
    
    
	return agent.genRandomAction(); // TODO: implement
	/*
		Initialise the search tree (non-chance node)
		start timing up to cycle-length-millis
		while ( still more time || less than mc-simulations completed) {
			searchTree.sample(agent, max distance from root));
		}
		look through all the successors of the root of our search tree:
		pick the action corresponding to the successor with highest m_mean/expectation().
	*/
}

