#include "predict.hpp"

#include <cassert>


CTNode::CTNode(void) :
	m_log_prob_est(0.0),
	m_log_prob_weighted(0.0)
{
	m_count[0] = 0;
	m_count[1] = 0;
	m_child[0] = NULL;
	m_child[1] = NULL;
}


CTNode::~CTNode(void) {
	if (m_child[0]) delete m_child[0];
	if (m_child[1]) delete m_child[1];
}


// number of descendants of a node in the context tree
size_t CTNode::size(void) const {

    size_t rval = 1;
    rval += child(false) ? child(false)->size() : 0;
    rval += child(true)  ? child(true)->size()  : 0;
    return rval;
}


// compute the logarithm of the KT-estimator update multiplier
double CTNode::logKTMul(symbol_t sym) const {
	return log( (double) (m_count[sym] + 0.5)
				/ (double) (m_count[false] + m_count[true] + 1) );
	//TODO probably needs <cmath> for log.
}


// create a context tree of specified maximum depth
ContextTree::ContextTree(size_t depth) :
	m_root(new CTNode()),
	m_depth(depth)
{ return; }


ContextTree::~ContextTree(void) {
	if (m_root) delete m_root;
}


// clear the entire context tree
void ContextTree::clear(void) {
	m_history.clear();
	if (m_root) delete m_root;
	m_root = new CTNode();
}


void ContextTree::update(symbol_t sym) {
	// TODO: implement
	
	CTNode *context_nodes[m_depth];
	
	context_nodes[0] = m_root;
	for (size_t n = 1; n < m_depth; ++n) {
		symbol_t context_symbol;
		// Generate fictional histories of 0s if necessary.
		if (m_history.size() - n < 0) {
			context_symbol = false;
		} else {
			context_symbol = m_history[m_history.size() - n];
		}
		// Create children as you need them.
		if (context_nodes[n-1].m_child[context_symbol] == NULL) {
			context_nodes[n-1].m_child[context_symbol] = new CTNode();
		}
		context_nodes[n] = context_nodes[n-1].m_child[context_symbol];
	}
	
	for (int n = m_depth - 1; n >= 0; --n) {
		// Update the log probabilities, after seeing sym.
		//TODO Verify this. Local KT estimate update, in log form.
		context_nodes[n].m_log_prob_est = context_nodes[n].m_log_prob_est
										  + context_nodes[n].logKTMul(sym);
		++(context_nodes[n].m_count[sym]);// Update a / b.
		//TODO CTW average in log form. I can't derive a nice expression for this in log form.
		context_nodes[n].m_log_prob_weighted = 0;
	}
	
	m_history.push_back(sym);
	
    /* Notes:
       Get context / CTNodes corresponding to that context
       		- What to do when m_history is smaller than m_depth?
       			Is a fictional history of 0s ok?
       Create nodes if this is the first time we have seen this context.
       Update estimates back up to root - need to figure out weighting for log.
       Use -log of KT estimate, to avoid precision errors.
       Update KT estimate - something to do with logKTMul.

       Add "sym" to context.
       	- m_history.push_back(sym);
    */
}


void ContextTree::update(const symbol_list_t &symlist) {
	// TODO: implement
    /* Notes
    Just call update, on each symbol in this list.
    */

    for (symbol_list_t::const_iterator it = symlist.begin(); it != symlist.end(); ++it) {
        update(*it);
    }
}

// TODO this seems strange, when would we ever need to do this?
// updates the history statistics, without touching the context tree
void ContextTree::updateHistory(const symbol_list_t &symlist) {

    for (size_t i=0; i < symlist.size(); i++) {
        m_history.push_back(symlist[i]);
    }
}


// removes the most recently observed symbol from the context tree
void ContextTree::revert(void) {
	// TODO: implement

    /* Notes:
       We can either store a snapshot of the tree in its entirety
       OR
       Just recompute the tree -
            delete whatever nodes were added by the last symbol.
            recalculate estimates.
    */
}


// shrinks the history down to a former size
void ContextTree::revertHistory(size_t newsize) {

    assert(newsize <= m_history.size());
    while (m_history.size() > newsize) m_history.pop_back();
}



// generate a specified number of random symbols
// distributed according to the context tree statistics
void ContextTree::genRandomSymbols(symbol_list_t &symbols, size_t bits) {

	genRandomSymbolsAndUpdate(symbols, bits);

	// restore the context tree to it's original state
	for (size_t i=0; i < bits; i++) revert();
}


// generate a specified number of random symbols distributed according to
// the context tree statistics and update the context tree with the newly
// generated bits
void ContextTree::genRandomSymbolsAndUpdate(symbol_list_t &symbols, size_t bits) {
	// TODO: implement

    // Notes:
    for (size_t i=0; i < bits; i++) {
        //make a symbol somehow
        //symbol_t sym = //TODO
        //add this to symbols
        symbols.push_back(sym);
        //update
        update(sym);
    }
}


// the logarithm of the block probability of the whole sequence
double ContextTree::logBlockProbability(void) {
    return m_root->logProbWeighted();
}

// TODO Watch out, returns the n'th history symbol (from the front),
// 		not the n'th most recent (from the back).
// get the n'th most recent history symbol, NULL if doesn't exist
const symbol_t *ContextTree::nthHistorySymbol(size_t n) const {
    return n < m_history.size() ? &m_history[n] : NULL;
}
