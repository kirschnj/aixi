#include "environment.hpp"

#include <cassert>

#include "util.hpp"

CoinFlip::CoinFlip(options_t &options) {
	// Determine the probability of the coin landing on heads
	p = 1.0;
	if (options.count("coin-flip-p") > 0) {
		strExtract(options["coin-flip-p"], p);
	}
	assert(0.0 <= p);
	assert(p <= 1.0);

	// Set up the initial observation
	m_observation = rand01() < p ? 1 : 0;
	m_reward = 0;
}

// Observes 1 (heads) with probability p and 0 (tails) with probability 1 - p.
// Observations are independent of the agent's actions. Gives a reward of 1 if
// the agent correctly predicts the next observation and 0 otherwise.
void CoinFlip::performAction(action_t action) {
	m_observation = rand01() < p ? 1 : 0;
	m_reward = action == m_observation ? 1 : 0;
}





Tiger::Tiger(options_t &options) {
	// Set up the initial observation
	m_observation = m_no_observation;
	m_reward = 0;
	// Place the tiger randomly.
	if (rand01() < 0.5) {
		m_tiger_left = true;
	} else {
		m_tiger_left = false;
	}
}

// Note: rewards translated to be positive.
// TODO describe the Tiger environment.
void Tiger::performAction(action_t action) {
	assert(action < m_num_actions);
	if (action == m_listen_action) {
		if (m_tiger_left) {
			if (rand01() < 0.85) {
				m_observation = m_hear_tiger_left;
			} else {
				m_observation = m_hear_tiger_right;
			}
		} else {
			if (rand01() < 0.85) {
				m_observation = m_hear_tiger_right;
			} else {
				m_observation = m_hear_tiger_left;
			}
		}
		m_reward = 99;
	} else {
		m_observation = m_no_observation;
		if (action == m_open_left) {
			if (m_tiger_left) {
				m_reward = 0;
			} else {
				m_reward = 110;
			}
		} else if (action == m_open_right) {
			if (m_tiger_left) {
				m_reward = 110;
			} else {
				m_reward = 0;
			}
		}
		// Reset the tiger randomly
		if (rand01() < 0.5) {
			m_tiger_left = true;
		} else {
			m_tiger_left = false;
		}
	}
}
