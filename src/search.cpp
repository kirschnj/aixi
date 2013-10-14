#include "search.hpp"

#include "agent.hpp"
#include "util.hpp"

#include <cmath>


// search options
static const visits_t     MinVisitsBeforeExpansion = 1;
static const unsigned int MaxDistanceFromRoot  = 100;// Note, Agent defines it's own horizon size;
static size_t             MaxSearchNodes;

SearchNode::SearchNode(bool is_chance_node,
    unsigned int num_actions, unsigned int num_percepts)
    : m_chance_node(is_chance_node), m_visits(0) {
    //make child field the right size
    int num_children = m_chance_node ? num_percepts : num_actions;
    m_child.resize(num_children, NULL);
    
    //make list of unexplored actions
    if(!is_chance_node){
        for(int i = 0; i < num_actions; ++i){
            m_unexplored_actions.push_back(i);
        }
    }
}

SearchNode::~SearchNode() {
    // Destroy any allocated children.
    for (unsigned int i = 0; i < m_child.size(); ++i) {
        if (m_child[i] != NULL) {
            delete m_child[i];
        }
    }
}

// simulate a path through a hypothetical future for the agent within it's
// internal model of the world, returning the accumulated reward.
static reward_t playout(Agent &agent, unsigned int playout_len) {
	reward_t r = 0;
	for (unsigned int i = 0; i < playout_len; ++i) {
	    // Pick a random action
	    action_t a = agent.genRandomAction();
	    agent.modelUpdate(a);

        percept_t rew;
        percept_t obs;
	    agent.genPerceptAndUpdate(obs, rew); // random percept
	    
	    r = r + rew;
    }
	return r;
}

action_t SearchNode::selectAction(Agent& agent, unsigned int dfr) {
    //choose unexplored action at random if any and append to tree
    if (!m_unexplored_actions.empty()) {
        int action_index = randRange(m_unexplored_actions.size());
        action_t action = m_unexplored_actions.at(action_index);
        m_unexplored_actions.erase(m_unexplored_actions.begin() + action_index);

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
        percept_t percept = (rew << agent.numObsBits()) | obs;
        
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
extern action_t search(Agent &agent, timelimit_t timelimit) {

    //save agent's state
    ModelUndo undo = ModelUndo(agent);

    SearchNode search_tree(false, agent.numActions(), agent.numPercepts());

    //sample
    for(visits_t i = 0; i < timelimit; ++i){    
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

