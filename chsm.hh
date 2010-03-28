#ifndef __chsm_hh__
#define __chsm_hh__

#include <deque>
#include <cassert>

#include <iostream>

namespace CHSM {


class Event {
public:
	enum Signal {
		/**
		 * Sent by the hsm to find out a State's parent.
		 */
		CHE_PARENT,
		/**
		 * Do a State's entry action.
		 */
		CHE_ENTRY,
		/**
		 * Do a State's exit action.
		 */
		CHE_EXIT,
		/**
		 * The first numbered event that can be used by a derived HSM.
		 * The event numbers derived by the user HSM should start at
		 * this number.
		 */
		CHE_USER,
	};
	/**
	 * Constructors of derived event types need to call this constructor
	 * with their derived event number.  Derived types also need a
	 * constructor that takes a single int, because other parts of CHsm
	 * make events assuming that constructor.
	 */
	Event(int event) {
		_event = event;
	};
	/**
	 * Get the event number.  States should call this and switch on the
	 * return value.
	 */
	int event() {
		return _event;
	};
private:
	int _event;
};

template<typename C, typename E>
class CHsm {

protected:

	enum CHsmState {
		/**
		 * Return to indicate that the State has handled the event.
		 * Returned automatically by ch_handled().
		 */
		CH_HANDLED = 1,
		/**
		 * Return to indicate that the State has not handled the event,
		 * and that it has indicated the parent state that might handle
		 * the event.  Returned automatically by ch_parent(State).
		 */
		CH_PARENT,
		/**
		 * Return to indicate that we want to transition to another
		 * state.  Returned automatically by ch_transition(State).
		 */
		CH_TRANSITION,
	};

	/**
	 * The type of all state functions.
	 */
	typedef CHsmState (C::*State)(E);

	/**
	 * A list of States.  This is a deque so we can iterate over it both
	 * forward and backwards.
	 */
	typedef std::deque<State> States;

	typedef typename std::deque<State>::const_iterator
		States_const_iterator;
	typedef typename std::deque<State>::const_reverse_iterator
		States_const_reverse_iterator;

	/**
	 * Called by a state function when it has handled an event.
	 */
	CHsmState ch_handled() {
		return CH_HANDLED;
	};

	/**
	 * Called by a state function when it has not handled an event, to
	 * indicate the parent state to try next.  Also used by the
	 * transition() functions while discovering the path from one state to
	 * another.
	 */
	CHsmState ch_parent(State state) {
		_parentState = state;
		return CH_PARENT;
	};

	/**
	 * Called by a state function when it wants the HSM to transition to a
	 * new state.  Note that this can be called by a state that is not the
	 * current state, in the case that an event is not handled and we start
	 * traversing up to find a state function that does handle it.
	 */
	CHsmState ch_transition(State state) {
		_transitionState = state;
		return CH_TRANSITION;
	};

	/**
	 * A default top state that can be used by derived HSMs.
	 */
	CHsmState topState(E e) {
		return ch_handled();
	};

	/**
	 * \arg top the top state for a derived HSM.  This state should return
	 * ch_handled() and do nothing else.
	 *
	 * \arg initial the initial state for a derived HSM.  A transition from
	 * the top state is done in the constructor.
	 */
	CHsm<C,E>(State top, State initial)
		: _events(),
		  _event_lock(false)
	{
		_topState = top;
		_state = initial;
		transition(initial);
	};

	/**
	 * \arg initial the initial state for a derived HSM.  A transition from
	 * the top state is done in the constructor.
	 */
	CHsm<C,E>(State initial)
		: _events(),
		  _event_lock(false)
	{
		_topState = &C::topState;
		_state = initial;
		transition(initial);
	};

	/**
	 * Does a transition from the current state to the top state so the HSM
	 * can undo all its actions on exit.
	 */
	virtual ~CHsm<C,E>() {
		exitTransition();
	};

public:

	/**
	 * Send an event to this HSM.  Sending the event to parent states, and
	 * transitions are handled.  In theory, all a properly specified
	 * derived HSM needs is a source of events.  This source can be the HSM
	 * itself (perhaps it's a derived class of a GUI object), or something
	 * external.
	 */
	void sendEvent(E e) {
		_events.push_back(e);
		if (! _event_lock)
			sendEvents();
	};

private:
	/**
	 * Set once in the constructor, and never changes afterwards.  This
	 * state should never handle events, but should call ch_handled() and
	 * do nothing else.
	 */
	State _topState;

	/**
	 * The current HSM state.
	 */
	State _state;

	/**
	 * The state to which we will transition.  Set by ch_transition(State).
	 */
	State _transitionState;

	/**
	 * Parent state of the state most recently called.  Set by
	 * ch_parent(State), which is called by any state that does not handle
	 * an event.
	 */
	State _parentState;

	/**
	 * List of events that we are to handle.
	 */
	std::deque<E> _events;

	/**
	 * Lock to make sure that while we are busy handling an event, any
	 * events sent to us will be queued.
	 */
	bool _event_lock;

	/**
	 * While there are events in our queue, handle them.
	 */
	void sendEvents() {
		while (_events.size()) {
			E e = _events.front();
			_events.pop_front();
			_event_lock = true;
			send1Event(e);
			_event_lock = false;
		}
	};

	void send1Event(E e) {
		// Save a copy of the current state so the loop below does not
		// change the current state.  If necessary, the current state
		// will be changed by transition().
		State state = _state;
		CHsmState s;
		do {
			s = (static_cast<C*>(this)->*state)(e);
			switch (s) {
			case CH_HANDLED:
				break;
			case CH_PARENT:
				state = _parentState;
				break;
			case CH_TRANSITION:
				transition(_state, _transitionState);
				// Force a break from the while loop.
				s = CH_HANDLED;
				break;
			}
		} while (s != CH_HANDLED);
	};

	/**
	 * The maximum depth of any part of the state hierarchy.  A hierarchy
	 * can theoretically be deeper than this, but we set a limit to catch
	 * errors.
	 */
	static const unsigned MAX_DEPTH = 10;

	/**
	 * Transition from one state to another.
	 *
	 * We find out the path from the source state to the destination state,
	 * then call entry and exit actions as required.
	 *
	 * To find out the path, we search for a common state that is either a
	 * parent of both the source and destination or is equal to one or both
	 * of the source or destination states.  No actions are called on that
	 * state unless the source and destination are the same.
	 *
	 * If the source and destination states are the same, we are doing a
	 * self transition, so we call the exit then the entry action for that
	 * state.
	 *
	 * TODO: When a significant application using CHsm exists, profile to
	 * see how much time is spent in here, and work out how to minimise it.
	 * One possible optimisation would be to keep a map of pair<src,dst> to
	 * pair<srcparents,destparents>, and if we have an entry in there then
	 * we call those lists instead of constructing the lists again.
	 *
	 * \arg src the source state (where we start)
	 * \arg dst the destination state (where we end up)
	 */
	void transition(State src, State dst)
	{
		// It would be silly to transition to the top state, whose only
		// function is to discard events.  The only exception to this
		// is in the destructor, when we want to exit completely, but
		// that happens elsewhere.
		assert( dst != _topState );

		// Degenerate case.
		if (src == _topState) {
			transition(dst);
			return;
		}
		if (src == dst) {
			// We are transitioning from a state to itself, so call
			// the exit and then the entry actions.
			(static_cast<C*>(this)->*src)(E(Event::CHE_EXIT));
			(static_cast<C*>(this)->*src)(E(Event::CHE_ENTRY));
			return;
		}

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
		dests.push_back(dst);
		while (srcs.back() != dests.back()) {
			if (strip_states(srcs, dests.back())) {
				break;
			}
			if (strip_states(dests, srcs.back())) {
				break;
			}
			if (srcs.back() != _topState) {
				(static_cast<C*>(this)->*srcs.back()) // func
					(E(Event::CHE_PARENT));	       // arg
				srcs.push_back(_parentState);
				assert( srcs.size() <= MAX_DEPTH );
			}
			if (dests.back() != _topState) {
				(static_cast<C*>(this)->*dests.back()) // func
					(E(Event::CHE_PARENT));		// arg
				dests.push_back(_parentState);
				assert( dests.size() <= MAX_DEPTH );
			}
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
		// order, and the entry actions for the dst states in reverse
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

		// Save the destination state as the current state.  This will
		// be the state that gets first cut at events from now on.
		_state = dst;
	};

	/**
	 * Strip statelist to contain members up to and including state.
	 *
	 * If state is a member of statelist, remove all members of statelist
	 * that are later in the list than state.  If state is not in
	 * statelist, statelist is not altered.
	 *
	 * When we are doing a transition between States, we need to work out
	 * the transition path by searching upwards from both source and
	 * destination States.  We have a complete path when either the highest
	 * parent that we have seen so far of the source State is anywhere in
	 * the destination list, or the highest parent that we have seen so far
	 * of the destination State is anywhere in the source list.
	 *
	 * This function is called twice at each step to tell us if we have
	 * that condition for either the source or destination list.  It tells
	 * us if we have satisfied the condition, and strips off the States
	 * that will be unused because they are higher than the common parent
	 * State.
	 *
	 * \arg statelist the list of States to search.  If state is found in
	 * this list, all States after this (towards the back of statelist)
	 * will be removed.
	 * \arg state the State to search for.
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
			// We should never end up with an empty statelist,
			// since state is required to be contained in it.
			assert( statelist.size() > 0 );
		}
		return found;
	}

	/** The simple transition from the top state to a destination. */
	void transition(State dst)
	{
		// Don't transition to the top state.
		assert( dst != _topState );

		States dests;
		dests.push_front(dst);
		State state = dst;

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
