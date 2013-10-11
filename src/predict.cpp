#include "predict.hpp"

#include <cassert>

// compute log(0.5)
static const double log_half = log(0.5);

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
    // Note, CTNode includes a visits() function, which we could use in place
    // of m_count[false] + m_count[true]. That's optional (they do exactly the
    // same thing, just visits() looks shorter).

    /* We could consider using a cache (like the table in the lectures) where
       we calculate a bunch of smallish KT estimates, which are looked up
       rather than re-computed every time we call this function.
       I imagine a lookup will be faster than using log(), and considering
       we'll be constantly updating the CTW tree, this should improve speed a
       little. At least in the beginning, before we get really large numbers
       for a and b (which won't be in our table).

       A possible implementation:
        
        // Cache initialisation
        int tableSize = 128 // Could make this larger even
        double KTCache[tableSize][tableSize];
        for (int i = 0; i < tableSize; i++) {
            for (int j = 0; j < tableSize; j++) {
                KTCache[i][j] = log( ((double) i + 0.5) / ((double) j + 1));
            }
        }

        // Later calls to logKTMul
        if (visits() < tableSize) {
            return KTCache[m_count[sym]][visits()];
        }

    */
}


// create a context tree of specified maximum depth
ContextTree::ContextTree(size_t depth) :
    m_root(new CTNode()),
    m_depth(depth)
{
    // Create a fictional history of 'depth' number of 0s.
    for (size_t i = 0; i < depth; ++i) {
        m_history.push_back(false);
    }
}


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
    
    // Path based on context
    // TODO consider renaming this, it's a bit long..
    CTNode *context_nodes[m_depth];

    context_nodes[0] = m_root;
    for (size_t n = 1; n < m_depth; ++n) {
        symbol_t context_symbol = m_history[m_history.size() - n];
        // Create children as they are needed.
        if (context_nodes[n-1].m_child[context_symbol] == NULL) {
            context_nodes[n-1].m_child[context_symbol] = new CTNode();
        }
        context_nodes[n] = context_nodes[n-1].m_child[context_symbol];
    }

    // Update possibilities from leaf back to root
    for (int n = m_depth - 1; n >= 0; --n) {
        // Update the log probabilities, after seeing sym.
        // Local KT estimate update, in log form.
        context_nodes[n].m_log_prob_est = context_nodes[n].m_log_prob_est
                                          + context_nodes[n].logKTMul(sym);
        // Update a / b.
        ++(context_nodes[n].m_count[sym]);

        // Update weighted probabilities
        if (n == m_depth - 1) {
            // Leaf node
            context_nodes[n].m_log_prob_weighted =
                    context_nodes[n].logProbEstimated();
        } else {
            // Compute log(0.5 * (P_e + P_w0 * P_w1))
            // == log(0.5) + log(P_e)
            //    + log(1 + exp(log(P_w0) + log(P_w1) - log(P_e))
            // TODO check syntax/calculation
            double log_w0;
            if (context_nodes[n].child(false) != NULL) {
                log_w0 = context_nodes[n].child(false).logProbWeighted();
            } else {
                log_w0 = 0;
            }
            double log_w1;
            if (context_nodes[n].child(true) != NULL) {
                log_w1 = context_nodes[n].child(true).logProbWeighted();
            } else {
                log_w1 = 0;
            }
            errno = 0;
            double tmp_exp = exp(log_w0 + log_w1 - context_nodes[n].logProbEstimated());
            if (errno == ERANGE) {
                // exp() overflowed the range of a double.
                // Then the '1 +' in '1 + exp' is irrelevant.
                // log(1 + exp(log(P_w0) + log(P_w1) - log(P_e)) becomes:
                // log(P_w0) + log(P_w1) - log(P_e)
                // TODO double check this.
                context_nodes[n].m_log_prob_weighted = log_half
                    + context_nodes[n].logProbEstimated()
                    + log_w0 + log_w1 - context_nodes[n].logProbEstimated();
            } else {
                context_nodes[n].m_log_prob_weighted = log_half
                    + context_nodes[n].logProbEstimated()
                    + log(1.0 + tmp_exp);
            }
        }
    }
    
    m_history.push_back(sym);
    
    /* Notes:
       Get context / CTNodes corresponding to that context
            - What to do when m_history is smaller than m_depth?
                Is a fictional history of 0s ok?
    Jesse:
    Rather than creating a node based on arbitrary histories, we could just
    not update (the probabilities, symbols still should be added to history)
    until we have enough context (seen depth bits).
    I'm not sure about this though.

       Create nodes if this is the first time we have seen this context.
       Update estimates back up to root - need to figure out weighting for log.
       Use -log of KT estimate, to avoid precision errors.
       Update KT estimate - something to do with logKTMul.

       Add "sym" to context.
        - m_history.push_back(sym);
    */
}


void ContextTree::update(const symbol_list_t &symlist) {
    // Call update, on each symbol in this list.
    for (symbol_list_t::const_iterator it = symlist.begin(); it != symlist.end(); ++it) {
        update(*it);
    }
}

// TODO this seems strange, when would we ever need to do this?
// Answer: Action-Conditional CTW: action symbols aren't added to the tree,
//      they are just appended straight to the history.
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
            We probably won't have enough memory to do this.
       OR
       Just recompute the tree -
            delete whatever nodes were added by the last symbol.
            recalculate estimates.
    */

    // ensure we have a recently observed symbol
    if (historySize() == 0) return;
    
    // Get latest symbol (to update counts) and remove from history
    symbol_t latest_sym = m_history.back();
    m_history.pop_back();

    // Path based on context
    CTNode *context_nodes[m_depth];

    context_nodes[0] = m_root;
    // Traverse tree to leaf
    for (size_t n = 1; n < m_depth; ++n) {
        symbol_t context_symbol;
        // Default to false if history does not exist
        if (m_history.size() - n < 0) {
            context_symbol = false;
        } else {
            context_symbol = m_history[m_history.size() - n];
        }
        context_nodes[n] = context_nodes[n-1].m_child[context_symbol];
    }

    // Update estimates
    for (int n = m_depth - 1; n >= 0; --n) {
        // Remove effects of last update
        --context_nodes[n].m_count[latest_sym];
        // TODO: consider deleting this node if it contains no visits.
        /*

        if (context_nodes[n].visits() == 0) {
            delete context_nodes[n];
            continue;
        }

        */
        context_nodes[n].m_log_prob_est -= logKTMul(latest_sym);

        // Update weighted probabilities
        if (n == m_depth - 1) {
            // Leaf node
            context_nodes[n].m_log_prob_weighted = context_nodes[n].logProbEstimated();
        } else {
            // Compute log(0.5 * (P_e + P_w0 * P_w1))
            // TODO check syntax/calculation
            double log_w0 = context_nodes[n].child(false).logProbWeighted();
            double log_w1 = context_nodes[n].child(true).logProbWeighted();
            double log_exp = log_w0 + log_w1 - context_nodes[n].logProbEstimated();

            // TODO: deal with/avoid overflow
            context_nodes[n].m_log_prob_weighted = log_half
                + context_nodes[n].logProbEstimated()
                + log(1.0 + exp(log_exp));
        }
    }
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
//      not the n'th most recent (from the back).
// get the n'th most recent history symbol, NULL if doesn't exist
const symbol_t *ContextTree::nthHistorySymbol(size_t n) const {
    return n < m_history.size() ? &m_history[n] : NULL;
}
