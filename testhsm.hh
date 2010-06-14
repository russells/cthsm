/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENCE" (Revision 42):
 * <russells@adelie.cx> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Russell Steicke.
 * ----------------------------------------------------------------------------
 */

#ifndef __testhsm_hh__
#define __testhsm_hh__

#include "cthsm.hh"
#include <iostream>

using namespace CTHSM;


/**
 * An example hsm event class.
 *
 * This class derives from CTHSM::Event (but that is not absolutely required.)
 * It must have
 *
 * - a constructor that takes a single int (the event number)
 *
 * - an int event() method that returns the event number.  This method is
 *   provided by CTHSM::Event if you derive from that.
 *
 * In the current implementation, this class must be copyable and assignable.
 * If you do derive from CTHSM::Event and add no virtual functions or data
 * members, the size of your class should be sizeof(int), so there's no
 * overhead in copying or assigning.
 */
class TestEvent : public Event {
public:
	TestEvent(int n) : Event(n) { };
	/**
	 * This anonymous enum provides the event numbers specific to this
	 * derived event type.
	 */
	enum {
		//* We must start our event numbers at or after CTHE_USER.
		TE_ONE = CTHE_USER,
		TE_TWO,
		TE_THREE,
		TE_BACK,
		TE_BACKAGAIN,
	};
};


/**
 * An example HSM.
 *
 * Your own HSM is required to be derived from the HSM template class, with
 * itself as the first template argument, and the event class as the second
 * template argument.  The template class provides the event queue, and the
 * mechanism for deciding what state is current, for transitioning between
 * states, and for sending events to the current state.
 *
 * Your own class adds the means to handle your own specialised events with
 * event methods, and the shape of the event handler hierarchy.  This shape is
 * discovered at runtime by the event mechanism asking each state for a pointer
 * to its parent state (the CTHE_PARENT event.)
 */
class TestHSM : public CTHsm<TestHSM, TestEvent> {

public:
	/**
	 * Our constructor needs to call the parent CTHsm<TestHSM,TestEvent>
	 * constructor with one or two arguments:
	 *
	 * - The initial state for this HSM, which must be a state from this
	 *   HSM, as the base class can't know what transitions you want to do.
	 *
	 * - And possibly also the top state for this HSM, which can be a state
	 *   from this HSM, as in this case, or the topState() provided by the
	 *   base template class.
	 *
	 * The second argument defaults to &TestHSM::topState, so we don't have
	 * to specify that.
	 *
	 * If you use the provided topState() method, you must get its pointer
	 * to member address as &TestHSM::topState, and not
	 * &CTHsm<TestHSM,TestEvent>::topState due to C++ rules for access to
	 * protected members.
	 *
	 * Also, we must call cthsmStart() before handling any events.  It's
	 * safest to do that here.
	 *
	 * (Note: in this implementation we have shadowed the template class
	 * topState() with one of our own.  That will change in a this example,
	 * soon, as will the method of indicating the top of the hierarchy
	 * expected by the event machinery in CTHsm<C,E>.)
	 */
	TestHSM() : CTHsm<TestHSM,TestEvent>(&TestHSM::leftBranch2) {
		cthsmStart();
	};

	CTHsmState topState(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_PARENT:
			std::cout << "topState parent\n";
			return cth_handled();
		case TestEvent::CTHE_ENTRY:
			std::cout << "topState entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "topState exit\n";
			return cth_handled();
		default:
			return cth_handled();
		}
	};

	CTHsmState commonState(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_PARENT:
			std::cout << "commonState parent\n";
			return cth_parent(&TestHSM::topState);
		case TestEvent::CTHE_ENTRY:
			std::cout << "commonState entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "commonState exit\n";
			return cth_handled();
		case TestEvent::TE_ONE:
			std::cout << "commonState TE_ONE\n";
			return cth_transition(&TestHSM::rightBranch1);
		case TestEvent::TE_BACKAGAIN:
			std::cout << "commonState TE_BACKAGAIN\n";
			return cth_transition(&TestHSM::rightBranch3);
		default:
			return cth_parent(&TestHSM::topState);
		}
	};

	CTHsmState leftBranch1(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_PARENT:
			std::cout << "leftBranch1 parent\n";
			return cth_parent(&TestHSM::commonState);
		case TestEvent::CTHE_ENTRY:
			std::cout << "leftBranch1 entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "leftBranch1 exit\n";
			return cth_handled();
		case TestEvent::TE_ONE:
			return cth_handled();
		case TestEvent::TE_TWO:
			return cth_transition(&TestHSM::leftBranch2);
		default:
			return cth_parent(&TestHSM::commonState);
		}
	};

	CTHsmState leftBranch2(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_PARENT:
			std::cout << "leftBranch2 parent\n";
			return cth_parent(&TestHSM::leftBranch1);
		case TestEvent::CTHE_ENTRY:
			std::cout << "leftBranch2 entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "leftBranch2 exit\n";
			return cth_handled();
		case TestEvent::TE_TWO:
			return cth_transition(&TestHSM::rightBranch2);
		case TestEvent::TE_THREE:
			return cth_transition(&TestHSM::rightBranch3);
		default:
			return cth_parent(&TestHSM::leftBranch1);
		}
	};

	CTHsmState rightBranch1(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_ENTRY:
			std::cout << "rightBranch1 entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "rightBranch1 exit\n";
			return cth_handled();
		case TestEvent::CTHE_PARENT:
			std::cout << "rightBranch1 parent\n";
			return cth_parent(&TestHSM::commonState);
		default:
			return cth_parent(&TestHSM::commonState);
		}
	};

	CTHsmState rightBranch2(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_ENTRY:
			std::cout << "rightBranch2 entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "rightBranch2 exit\n";
			return cth_handled();
		case TestEvent::CTHE_PARENT:
			std::cout << "rightBranch2 parent\n";
			return cth_parent(&TestHSM::rightBranch1);
		default:
			return cth_parent(&TestHSM::rightBranch1);
		}
	};

	CTHsmState rightBranch3(TestEvent e) {
		switch (e.event()) {
		case TestEvent::CTHE_ENTRY:
			std::cout << "rightBranch3 entry\n";
			return cth_handled();
		case TestEvent::CTHE_EXIT:
			std::cout << "rightBranch3 exit\n";
			return cth_handled();
		case TestEvent::CTHE_PARENT:
			std::cout << "rightBranch3 parent\n";
			return cth_parent(&TestHSM::rightBranch2);
		case TestEvent::TE_BACK:
			return cth_transition(&TestHSM::leftBranch1);
		default:
			return cth_parent(&TestHSM::rightBranch2);
		}
	};

};

#endif
