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

class TestEvent : public Event {
public:
	TestEvent(int n) : Event(n) { };
	enum {
		TE_ONE = CTHE_USER,
		TE_TWO,
		TE_THREE,
		TE_BACK,
		TE_BACKAGAIN,
	};
};

class TestHSM : public CTHsm<TestHSM, TestEvent> {

public:
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

	TestHSM() : CTHsm<TestHSM,TestEvent>(&TestHSM::topState, &TestHSM::leftBranch2) {
		cthsmStart();
	};

};

#endif
