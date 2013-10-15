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

bool Agent::lastUpdatePercept(void) const {
    return m_last_update_percept;
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


// number of distinct percepts that can be observed
unsigned int Agent::numObsBits(void) const {
    return m_obs_bits;
}

// number of distinct percepts that can be observed
unsigned int Agent::numRewBits(void) const {
    return m_rew_bits;
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
    assert(!m_last_update_percept); 
    symbol_list_t action_symbols;
	m_ct->genRandomSymbols(action_symbols, m_actions_bits);
    return decodeAction(action_symbols); 
}


// generate a percept distributed according
// to our history statistics
void Agent::genPercept(percept_t &obs, percept_t &rew) const {
 //TODO: Implement
}


// generate a percept distributed to our history statistics, and
// update our mixture environment model with it
void Agent::genPerceptAndUpdate(percept_t &obs, percept_t &rew) {
    assert(m_last_update_percept == false);
    symbol_list_t obs_symbols, rew_symbols;
    
    //generate obs and reward symbols
    m_ct->genRandomSymbolsAndUpdate(obs_symbols, m_obs_bits);
    m_ct->genRandomSymbolsAndUpdate(rew_symbols, m_rew_bits);

    rew = decodeReward(rew_symbols);
    obs = decodeObservation(obs_symbols);

    // Update other properties
    m_last_update_percept=true;
    m_total_reward += rew;
}

// Update the agent's internal model of the world after receiving a percept
void Agent::modelUpdate(percept_t observation, percept_t reward) {
    assert(m_last_update_percept == false);
	
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
	assert(m_last_update_percept == true);
	assert(isActionOk(action));

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
    if(m_time_cycle < mu.age())
        return false;

    //TODO: check this
    
    //go back in history and revert actions and percepts
    while(historySize() > mu.historySize() + 1){
        if(m_last_update_percept){
            m_ct->revert(m_rew_bits + m_obs_bits);
            m_last_update_percept = false;
        }
        else{
            m_ct->revertHistory(m_actions_bits);
            m_last_update_percept = true;
        }
    }

    //revert one more action
    m_ct->revertHistory(m_actions_bits);
    m_last_update_percept = true;

    //if last state was action state, revert one more percept
    if(!mu.lastUpdatePercept()){
        m_ct->revert(m_rew_bits + m_obs_bits);
        m_last_update_percept = false;
    }

    m_time_cycle = mu.age();
    m_total_reward = mu.reward();

    return true;
}


void Agent::reset(void) {
	m_ct->clear();

	m_time_cycle = 0;
	m_total_reward = 0.0;
    m_last_update_percept=false;
}


// probability of selecting an action according to the
// agent's internal model of it's own behaviour
//double Agent::getPredictedActionProb(action_t action) {
//	return NULL; // TODO: implement
//} NOTE: agent gets probability straight from model when it is needed


// get the agent's probability of receiving a particular percept
//double Agent::perceptProbability(percept_t observation, percept_t reward) const {
//	return NULL; // TODO: implement
//} NOTE: agent gets probability straight from model when it is needed


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

// Decodes the observation from a list of symbols
percept_t Agent::decodeObservation(const symbol_list_t &symlist) const {
    return decode(symlist, m_obs_bits);
}

void Agent::load_ct(std::istream &in){
    in >> (*m_ct);
}

void Agent::write_ct(std::ostream &out){
   out << (*m_ct);
}


// used to revert an agent to a previous state
ModelUndo::ModelUndo(const Agent &agent) {
    m_age          = agent.age();
    m_reward       = agent.reward();
    m_history_size = agent.historySize();
    m_last_update_percept = agent.lastUpdatePercept();
}
