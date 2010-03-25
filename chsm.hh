#ifndef __chsm_hh__
#define __chsm_hh__

#include <deque>
#include <cassert>

#include <iostream>

namespace CHSM {


class Event {
public:
	enum Signal {
		CHE_PARENT,
		CHE_ENTRY,
		CHE_EXIT,
		CHE_USER,
	};
	Event(int num) {
		_num = num;
	};
protected:
	int _num;
public:
	int number() {
		return _num;
	};
};

template<typename C, typename E>
class CHsm {

protected:

	enum CHsmState {
		CH_HANDLED = 1,
		CH_SUPER,
		CH_TRANSITION,
	};

	typedef CHsmState (C::*State)(E);
	typedef std::deque<State> States;
	typedef typename std::deque<State>::iterator
		States_iterator;
	typedef typename std::deque<State>::const_iterator
		States_const_iterator;
	typedef typename std::deque<State>::const_reverse_iterator
		States_const_reverse_iterator;

	CHsmState ch_handled() {
		return CH_HANDLED;
	};
	CHsmState ch_parent(State state) {
		_parentState = state;
		return CH_SUPER;
	};
	CHsmState ch_transition(State state) {
		_transitionState = state;
		return CH_TRANSITION;
	};

	CHsmState topState(E e) {
		return ch_handled();
	};

	CHsmState aState(E e) {
		return ch_handled();
	};

	// For setting the initial state.
	void setState(State state) {
		_state = state;
	};

	CHsm<C,E>(State top, State initial) {
		_topState = top;
		_state = initial;
		transition(initial);
	};

	CHsm<C,E>(State initial) {
		_topState = &C::topState;
		_state = initial;
		transition(initial);
	};

	~CHsm<C,E>() {
		exitTransition();
	};

private:
	/** Set once in the constructor, and never changes afterwards. */
	State _topState;
	/** The current HSM state. */
	State _state;
	/** The state to which we will transition. */
	State _transitionState;
	/** Parent state of the state most recently called. */
	State _parentState;

public:

	void sendEvent(E e) {
		State initialState = _state;
		State state = initialState;
		CHsmState s;
		do {
			s = (static_cast<C*>(this)->*state)(e);
			switch (s) {
			case CH_HANDLED:
				break;
			case CH_SUPER:
				state = _parentState;
				break;
			case CH_TRANSITION:
				transition(_state, _transitionState);
				s = CH_HANDLED;
				break;
			}
		} while (s != CH_HANDLED);
	};

private:

	/** The maximum depth of any part of the state hierarchy. */
	static const unsigned MAX_DEPTH = 10;

	void transition(State src, State dest)
	{
		// Degenerate case.
		if (src == _topState) {
			transition(dest);
			return;
		}
		if (src == dest) {
			// We are transitioning from a state to itself, so call
			// the exit and then the entry actions.
			(static_cast<C*>(this)->*src)(E(Event::CHE_EXIT));
			(static_cast<C*>(this)->*src)(E(Event::CHE_ENTRY));
			return;
		}

		// It would be silly to transition to the top state, whose only
		// function is to discard events.  The only exception to this
		// is in the destructor, when we want to exit completely, but
		// that happens elsewhere.
		assert( dest != _topState );

		States srcs;
		States dests;


		// Walk the tree to find the transition path.  Each parent path
		// will be pushed onto its queue until we have a complete path.
		//
		// The path is complete when the any member of srcs is equal to
		// the tail of dests, or any member of dests is equal to the
		// tail of srcs.
		//

		srcs.push_back(src);
		dests.push_back(dest);
		while (srcs.back() != dests.back()) {
			//std::cout << "while\n";
			if (strip_states(srcs, dests.back())) {
				//std::cout << "breaking due to strip_states(srcs,dests.back())\n";
				break;
			}
			if (strip_states(dests, srcs.back())) {
				//std::cout << "breaking due to strip_states(dests,srcs.back())\n";
				break;
			}
			if (srcs.back() != _topState) {
				(static_cast<C*>(this)->*srcs.back()) // func
					(E(Event::CHE_PARENT));	       // arg
				srcs.push_back(_parentState);
				//if (_parentState == _topState) std::cout << "  That was _topState\n";
			}
			if (dests.back() != _topState) {
				(static_cast<C*>(this)->*dests.back()) // func
					(E(Event::CHE_PARENT));		// arg
				dests.push_back(_parentState);
				//if (_parentState == _topState) std::cout << "  That was _topState\n";
			}
			if (srcs.back() == dests.back()) std::cout << "  srcs.back() == dests.back()\n";
		}
		// We now have two lists of States, and there should be at
		// least one entry in each list.
		assert( srcs.size() > 0 );
		assert( dests.size() > 0 );
		// The back() of the lists should be the same State.
		assert( srcs.back() == dests.back() );
		// We don't want to call any actions on the common parent
		// state, so remove it from both lists.
		srcs.pop_back();
		dests.pop_back();

		// Now call the exit action for the src states in forward list
		// order, and the entry actions for the dest states in reverse
		// list order.
		States_const_iterator srcit;
		for (srcit = srcs.begin(); srcit != srcs.end(); srcit++) {
			State s = *srcit;
			(static_cast<C*>(this)->*s)(E(Event::CHE_EXIT));
		}
		States_const_reverse_iterator destit;
		for (destit = dests.rbegin(); destit != dests.rend(); destit++) {
			State d = *destit;
			(static_cast<C*>(this)->*d)(E(Event::CHE_ENTRY));
		}
		_state = dest;
	};

	/**
	 * Strip statelist to contain members up to and including state.
	 *
	 * If state is a member of statelist, remove all members of statelist
	 * that are later in the list than state.  If state is not in
	 * statelist, statelist is not altered.
	 *
	 * \return true if state was found.
	 */
	bool strip_states(States& statelist, State& state)
	{
		States_const_reverse_iterator list_it;
		int to_remove = 0;
		bool found = false;
		// Yes, list_it++ is correct.
		for (list_it = statelist.rbegin();
		     list_it != statelist.rend(); list_it++) {
			State s = *list_it;
			if (s == state) {
				found = true;
				break;
			} else {
				to_remove++;
			}
		}
		if (found) {
			if (to_remove) {
				while (to_remove--) {
					statelist.pop_back();
				}
			}
		}
		return found;
	}

	/** The simple transition from the top state to a destination. */
	void transition(State dest)
	{
		if (dest == _topState) {
			return;
		}
		States dests;
		dests.push_front(dest);
		State state = dest;

		do {
			(static_cast<C*>(this)->*state)(E(Event::CHE_PARENT));
			if (_parentState != _topState) {
				dests.push_front(_parentState);
				assert( dests.size() <= MAX_DEPTH );
			}
			state = _parentState;
		} while (_parentState != _topState);


		States_const_iterator i;
		for (i=dests.begin(); i != dests.end(); i++) {
			(static_cast<C*>(this)->*(*i))(E(Event::CHE_ENTRY));
		}
	};

	void transition(std::deque<State> srcs, std::deque<State> dests)
	{
	};

	/** Exit back to the top state.  Called only by the destructor. */
	void exitTransition()
	{
		while (_state != _topState) {
			(static_cast<C*>(this)->*_state)(E(Event::CHE_EXIT));
			(static_cast<C*>(this)->*_state)(E(Event::CHE_PARENT));
			_state = _parentState;
		}
	};

};


} // namespace CHSM
#endif /* __chsm_hh__*/
