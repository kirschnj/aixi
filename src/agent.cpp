#include "agent.hpp"

#include <cassert>
#include <cmath>

#include "predict.hpp"
#include "search.hpp"
#include "util.hpp"


// construct a learning agent from the command line arguments
Agent::Agent(options_t & options) {
	std::string s;

	strExtract(options["agent-actions"], m_actions);
	strExtract(options["agent-horizon"], m_horizon);
	strExtract(options["observation-bits"], m_obs_bits);
	strExtract<unsigned int>(options["reward-bits"], m_rew_bits);

	// calculate the number of bits needed to represent the action
	for (unsigned int i = 1, c = 1; i < m_actions; i *= 2, c++) {
		m_actions_bits = c;
	}
	
	// calculate the number of possible percepts
	m_percepts = pow(2, m_obs_bits + m_rew_bits);

	m_ct = new ContextTree(strExtract<unsigned int>(options["ct-depth"]));

	reset();
}


// destruct the agent and the corresponding context tree
Agent::~Agent(void) {
	if (m_ct) delete m_ct;
}


// current age of the agent in cycles
age_t Agent::age(void) const {
	return m_time_cycle;
}

// the total accumulated reward across an agents lifespan
reward_t Agent::reward(void) const {
	return m_total_reward;
}


// the average reward received by the agent at each time step
reward_t Agent::averageReward(void) const {
    return age() > 0 ? reward() / reward_t(age()) : 0.0;
}

// maximum reward in a single time instant
reward_t Agent::maxReward(void) const {
	return reward_t((1 << m_rew_bits) - 1);
}


// minimum reward in a single time instant
reward_t Agent::minReward(void) const {
	return 0.0;
}


// number of distinct actions
unsigned int Agent::numActions(void) const {
	return m_actions;
}

// number of distinct percepts that can be observed
unsigned int Agent::numPercepts(void) const {
	return m_percepts;
}


// the length of the stored history for an agent
size_t Agent::historySize(void) const {
	return m_ct->historySize();
}


// length of the search horizon used by the agent
size_t Agent::horizon(void) const {
	return m_horizon;
}


// generate an action uniformly at random
action_t Agent::genRandomAction(void) const {
	return randRange(m_actions);
}


// generate an action distributed according
// to our history statistics
action_t Agent::genAction(void) const {
	return NULL; // TODO: implement
	/*
		TODO Note: the CTW is 'action-conditional', which means it doesn't
			encode the probabilities of actions. it encodes P((o,r)1:n | a1:n)
			So I'm not sure what this function is
			meant to actually mean.
	*/
}


// generate a percept distributed according
// to our history statistics
percept_t Agent::genPercept(void) const {
	// TODO: implement
	
	double percept_prob[numPercepts()];
	double joint_prob = m_ct->logBlockProbability();
	
	for (unsigned int p = 0; p < numPercepts(); ++p) {
	    symbol_list_t percept_symbols;
	    encode(percept_symbols, p, m_obs_bits + m_rew_bits);
	    //TODO move probability getting into CTW predict() functions.
	    m_ct->update(percept_symbols);
	    percept_prob[p] = m_ct->logBlockProbability() - joint_prob;
	    for (unsigned int i = 0; i < m_obs_bits + m_rew_bits; ++i) {
	        m_ct->revert();
	    }
	}
	
	double random = rand01();
	double sum;
	percept_t percept = numPercepts() - 1;
	for (unsigned int p = 0; p < numPercepts(); ++p) {
	    sum += exp(percept_prob[p]);
	    if (random <= sum) {
	        percept = p;
	        break;
	    }
	}
	
	return percept;
	/*
		Very similar to above, but this time we have to go over all
		2^numPerceptBits possible percepts.
		for (every possible percept sequence) {
			compute conditional probability of seeing that sequence,
			given everything we've already seen:
				read the joint probability so far from the root of the CTW (log_prob_weights)
				get the joint probability of everything and this prospective percept
					(by updating the CTW with all the symbols sequentially)
				divide the latter by the former, to get the conditional probability
					of seeing that percept sequence
				And be careful of log space, so division means subtraction.
				revert the CTW
		}
		compute random number in [0, 1]
		double probSum = 0;
		for (every possible percept) {
			probSum = probSum + percept probability
			if (probSum > random number) {
				return this percept.
			}
		}
		On the off chance that numerical approximations mean the sum is less than 1,
		and we make it past the loop:
		return last percept
	*/
}


// generate a percept distributed to our history statistics, and
// update our mixture environment model with it
percept_t Agent::genPerceptAndUpdate(void) {
	return NULL; // TODO: implement
}


// Update the agent's internal model of the world after receiving a percept
void Agent::modelUpdate(percept_t observation, percept_t reward) {
	// Update internal model
	symbol_list_t percept;
	encodePercept(percept, observation, reward);

	m_ct->update(percept);


	// Update other properties
	m_total_reward += reward;
	m_last_update_percept = true;
}


// Update the agent's internal model of the world after performing an action
void Agent::modelUpdate(action_t action) {
	assert(isActionOk(action));
	assert(m_last_update_percept == true);

	// Update internal model
	symbol_list_t action_syms;
	encodeAction(action_syms, action);
	
	m_ct->updateHistory(action_syms);

	m_time_cycle++;
	m_last_update_percept = false;
}


// revert the agent's internal model of the world
// to that of a previous time cycle, false on failure
bool Agent::modelRevert(const ModelUndo &mu) {
	return NULL; // TODO: implement
}


void Agent::reset(void) {
	m_ct->clear();

	m_time_cycle = 0;
	m_total_reward = 0.0;
}


// probability of selecting an action according to the
// agent's internal model of it's own behaviour
double Agent::getPredictedActionProb(action_t action) {
	return NULL; // TODO: implement
}


// get the agent's probability of receiving a particular percept
double Agent::perceptProbability(percept_t observation, percept_t reward) const {
	return NULL; // TODO: implement
}


// action sanity check
bool Agent::isActionOk(action_t action) const {
	return action < m_actions;
}


// reward sanity check
bool Agent::isRewardOk(reward_t reward) const {
    return reward >= minReward() && reward <= maxReward();
}


// Encodes an action as a list of symbols
void Agent::encodeAction(symbol_list_t &symlist, action_t action) const {
	symlist.clear();

	encode(symlist, action, m_actions_bits);
}

// Encodes a percept (observation, reward) as a list of symbols
void Agent::encodePercept(symbol_list_t &symlist, percept_t observation, percept_t reward) const {
	symlist.clear();

	encode(symlist, observation, m_obs_bits);
	encode(symlist, reward, m_rew_bits);
}

// Decodes the observation from a list of symbols
action_t Agent::decodeAction(const symbol_list_t &symlist) const {
	return decode(symlist, m_actions_bits);
}


// Decodes the reward from a list of symbols
percept_t Agent::decodeReward(const symbol_list_t &symlist) const {
	return decode(symlist, m_rew_bits);
}


// used to revert an agent to a previous state
ModelUndo::ModelUndo(const Agent &agent) {

    m_age          = agent.age();
    m_reward       = agent.reward();
    m_history_size = agent.historySize();

}
