#include "search.hpp"

#include "agent.hpp"
#include "util.hpp"

#include <cmath>

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
	action_t selectAction(Agent &agent, unsigned int dfr); // TODO: implement

	// determine the expected reward from this node
	reward_t expectation(void) const { return m_mean; }

	// perform a sample run through this node and it's children,
	// returning the accumulated reward from this sample run
	reward_t sample(Agent &agent, unsigned int dfr); // TODO: implement

	// number of times the search node has been visited
	visits_t visits(void) const { return m_visits; }
	
	SearchNode* child(unsigned int n) { return m_child[n]; }

private:

	bool m_chance_node; // true if this node is a chance node, false otherwise
	double m_mean;      // the expected reward of this node
	visits_t m_visits;  // number of times the search node has been visited

	// TODO: decide how to reference child nodes
	//  e.g. a fixed-size array
	std::vector<SearchNode*> m_child;
};

SearchNode::SearchNode(bool is_chance_node,
    unsigned int num_actions, unsigned int num_percepts)
: m_chance_node(is_chance_node) {
    int num_children;
    if (m_chance_node) {
        num_children = num_percepts;
    } else {
        num_children = num_actions;
    }
    for (int i = 0; i < num_children; ++i) {
        m_child.push_back(NULL);
    }
}

// simulate a path through a hypothetical future for the agent within it's
// internal model of the world, returning the accumulated reward.
static reward_t playout(Agent &agent, unsigned int playout_len) {
	if (playout_len == 0) {
	    return 0;
	}
	// Pick a random action
	action_t a = agent.genRandomAction();
	agent.modelUpdate(a);

    percept_t rew;
    percept_t obs;
	agent.genPerceptAndUpdate(obs, rew); // random percept
    
	return rew + playout(agent, playout_len - 1);
}

action_t SearchNode::selectAction(Agent& agent, unsigned int dfr) {
    std::vector<action_t> unexplored_actions;
    
    //find unexplored actions
    for (action_t a = 0; a < agent.numActions(); ++a) {
        if (m_child[a] == NULL) {
            unexplored_actions.push_back(a);
        }
    }
    //choose unexplored action at random if any and append to tree
    if (!unexplored_actions.empty()) {
        action_t action = unexplored_actions.at(
                                randRange(unexplored_actions.size()));
        m_child[action] = new SearchNode(true, agent.numActions(), agent.numPercepts());
        return action;
    } else {
        action_t arg_max = 0;
        double C = 1;
        double max = (1.0 / (dfr * agent.maxReward())) * m_child[0]->m_mean
                    + C * sqrt((log(m_visits)/m_child[0]->m_visits));
        //search for argmax
        for (action_t a = 1; a < agent.numActions(); ++a) {
            double f = (1.0 / (dfr * agent.maxReward())) * m_child[a]->m_mean
                    + C * sqrt((log(m_visits)/m_child[a]->m_visits));
            // Notes: agent.minReward() defined as 0, so omitted.
            // TODO Need constant C defined somewhere.
            if (f > max) {
                max = f;
                arg_max = a;
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
        // Generate whole observation-reward percept.
        // Make sure the percept is added to the agent's model and history.
        percept_t obs;
        percept_t rew;
        agent.genPerceptAndUpdate(obs, rew);

        //calc index of whole percept
        percept_t percept = (obs << agent.numObsBits()) & obs;
        
        if (m_child[percept] == NULL) {
            m_child[percept] = new SearchNode(false,
                                    agent.numActions(), agent.numPercepts());
        }
        newReward = rew + m_child[percept]->sample(agent, dfr - 1);
    } else if (m_visits == 0) {
        newReward = playout(agent, dfr);
    } else {
        action_t action = selectAction(agent, dfr);
        agent.modelUpdate(action);
        newReward = m_child[action]->sample(agent, dfr);
    }
    m_mean = (1.0 / (double) (m_visits + 1)) * (newReward + m_visits * m_mean);
    ++m_visits;
    return newReward;
}

// determine the best action by searching ahead using MCTS
extern action_t search(Agent &agent, timelimit_t time_limit) {

    //save agent's state
    ModelUndo undo = ModelUndo(agent);

    SearchNode search_tree(false, agent.numActions(), agent.numPercepts());

    //sample time_limit times
    for(visits_t i = 0; i < time_limit; ++i){    
        search_tree.sample(agent, agent.horizon());
        agent.modelRevert(undo);
    }
    
    // we assume timelimit is large enough so that every action was sampled. this is checked in main_loop.
    double best_reward = search_tree.child(0)->expectation();
    unsigned int best_action = 0;
    for (unsigned int i = 1; i < agent.numActions(); ++i) {
        if (search_tree.child(i)->expectation() > best_reward) {
            best_reward = search_tree.child(i)->expectation();
            best_action = i;
        }
    }
    
    return best_action;
}

