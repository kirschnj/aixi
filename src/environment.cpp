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
void Tiger::performAction(action_t action) {
	assert(action < m_num_actions);

	if (action == m_listen_action) {
		if (rand01() < 0.85) { //answer truthful
            m_observation = (m_tiger_left ? (percept_t) m_hear_tiger_left : m_hear_tiger_right);
		}
        else {
            m_observation = (m_tiger_left ? (percept_t) m_hear_tiger_right : m_hear_tiger_left);
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



BiasedRockPaperScissor::BiasedRockPaperScissor(options_t &options) 
: m_won_with_rock(false){
	// Set up the initial observation
	m_observation = m_opp_scissors;
	m_reward = 0;
}


void BiasedRockPaperScissor::performAction(action_t action) {
    if (m_won_with_rock) {
        m_observation = m_opp_rock;
    } else {
        m_observation = randRange(3);
    }
    m_won_with_rock = false;
    
    if (m_observation == action) {
        //draw
        m_reward = 1;
    } else if ((action + 1)%3 == m_observation) {
        // Agent wins
        m_reward = 2;
    } else {
        // Agent loses
        m_reward = 0;
        if (m_observation == m_opp_rock) {
            m_won_with_rock = true;
        }
    }
}


KuhnPoker::KuhnPoker(options_t &options) {
    m_opp_nash_parameter = 1.0;
	if (options.count("nash-parameter") > 0) {
		strExtract(options["nash-parameter"], m_opp_nash_parameter);
	}
	assert(0.0 <= m_opp_nash_parameter);
	assert(m_opp_nash_parameter <= 1.0);
	// Set up the initial observation
	m_reward = 0;
    m_showdown = 0;
	reset();
}

void KuhnPoker::opponentAct(unsigned int round) {
    if (round == 0) {
        if (m_opp_card == m_jack) {
            if (rand01() > m_opp_nash_parameter / 3.0) {
                m_opp_action = m_bet;
            } else {
                m_opp_action = m_pass;
            }
        } else if (m_opp_card == m_queen) {
            m_opp_action = m_pass;
        } else {//king
            if (rand01() > m_opp_nash_parameter) {
                m_opp_action = m_bet;
            } else {
                m_opp_action = m_pass;
            }
        }
    } else {//m_round == 1
        // pass-bet is the only way to get to the second round
        if (m_opp_card == m_jack) {
            m_opp_action = m_pass;
        } else if (m_opp_card == m_queen) {
            if (rand01() > (1.0 + m_opp_nash_parameter) / 3.0) {
                m_opp_action = m_bet;
            } else {
                m_opp_action = m_pass;
            }
        } else {//king
            m_opp_action = m_bet;
        }
    }
}

void KuhnPoker::reset(void) {
    // Deal cards
    m_agent_card = randRange(3);
    m_opp_card = (m_agent_card + 1 + randRange(2)) % 3;
    // Create obesrvation
	opponentAct(0);
	// TODO I'm not too sure on the fourth bit...
	m_observation = (m_agent_card << 2) | (m_opp_action << 1) | m_showdown;
    
    m_opp_action_0 = m_opp_action;
    m_showdown = 0;
}

void KuhnPoker::performAction(action_t action) {
    if (m_opp_action_0 == m_bet) {
        if (action == m_bet) {
            // Showdown
            if (m_agent_card > m_opp_card) {
                m_reward = 4;// Agent won 2 chips.
            } else {
                m_reward = 0;// Agent lost 2 chips.
            }
            m_showdown = 1;
        } else {//action == m_pass
            m_reward = 1;// Agent lost 1 chip.
        }
    } else {//m_opp_action_0 == m_pass
        if (action == m_bet) {
            // Opponent second action
            opponentAct(1);
            if (m_opp_action == m_bet) {
                if (m_agent_card > m_opp_card) {
                    m_reward = 4;// Agent won 2 chips.
                } else {
                    m_reward = 0;// Agent lost 2 chips.
                }
                m_showdown = 1;
            } else {//m_opp_action == m_pass
                m_reward = 3;// Agent won 1 chip.
            }
        } else {//action == m_pass
            if (m_agent_card > m_opp_card) {
                m_reward = 3;// Agent won 1 chip.
            } else {
                m_reward = 1;// Agent lost 1 chip.
            }
            m_showdown = 1;
        }
    }
    reset();
}
