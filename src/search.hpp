#ifndef __SEARCH_HPP__
#define __SEARCH_HPP__

#include "main.hpp"

class Agent;

// determine the best action by searching ahead
extern action_t search(Agent &agent, timelimit_t mc_timelimit);

#endif // __SEARCH_HPP__
