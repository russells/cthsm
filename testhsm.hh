#ifndef __testhsm_hh__
#define __testhsm_hh__

#include "chsm.hh"
#include <iostream>

using namespace CHSM;

class TestEvent : public Event {
public:
	TestEvent(int n) : Event(n) { };
	enum {
		TE_ONE = CHE_USER,
		TE_TWO,
		TE_THREE,
		TE_BACK,
		TE_BACKAGAIN,
	};
};

class TestHSM : public CHsm<TestHSM, TestEvent> {

public:
	CHsmState topState(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_PARENT:
			std::cout << "topState parent\n";
			return ch_handled();
		case TestEvent::CHE_ENTRY:
			std::cout << "topState entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "topState exit\n";
			return ch_handled();
		default:
			return ch_handled();
		}
	};

	CHsmState commonState(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_PARENT:
			std::cout << "commonState parent\n";
			return ch_parent(&TestHSM::topState);
		case TestEvent::CHE_ENTRY:
			std::cout << "commonState entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "commonState exit\n";
			return ch_handled();
		case TestEvent::TE_ONE:
			std::cout << "commonState TE_ONE\n";
			return ch_transition(&TestHSM::rightBranch1);
		case TestEvent::TE_BACKAGAIN:
			std::cout << "commonState TE_BACKAGAIN\n";
			return ch_transition(&TestHSM::rightBranch3);
		default:
			return ch_parent(&TestHSM::topState);
		}
	};

	CHsmState leftBranch1(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_PARENT:
			std::cout << "leftBranch1 parent\n";
			return ch_parent(&TestHSM::commonState);
		case TestEvent::CHE_ENTRY:
			std::cout << "leftBranch1 entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "leftBranch1 exit\n";
			return ch_handled();
		case TestEvent::TE_ONE:
			return ch_handled();
		case TestEvent::TE_TWO:
			return ch_transition(&TestHSM::leftBranch2);
		default:
			return ch_parent(&TestHSM::commonState);
		}
	};

	CHsmState leftBranch2(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_PARENT:
			std::cout << "leftBranch2 parent\n";
			return ch_parent(&TestHSM::leftBranch1);
		case TestEvent::CHE_ENTRY:
			std::cout << "leftBranch2 entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "leftBranch2 exit\n";
			return ch_handled();
		case TestEvent::TE_TWO:
			return ch_transition(&TestHSM::rightBranch2);
		case TestEvent::TE_THREE:
			return ch_transition(&TestHSM::rightBranch3);
		default:
			return ch_parent(&TestHSM::leftBranch1);
		}
	};

	CHsmState rightBranch1(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_ENTRY:
			std::cout << "rightBranch1 entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "rightBranch1 exit\n";
			return ch_handled();
		case TestEvent::CHE_PARENT:
			std::cout << "rightBranch1 parent\n";
			return ch_parent(&TestHSM::commonState);
		default:
			return ch_parent(&TestHSM::commonState);
		}
	};

	CHsmState rightBranch2(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_ENTRY:
			std::cout << "rightBranch2 entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "rightBranch2 exit\n";
			return ch_handled();
		case TestEvent::CHE_PARENT:
			std::cout << "rightBranch2 parent\n";
			return ch_parent(&TestHSM::rightBranch1);
		default:
			return ch_parent(&TestHSM::rightBranch1);
		}
	};

	CHsmState rightBranch3(TestEvent e) {
		switch (e.number()) {
		case TestEvent::CHE_ENTRY:
			std::cout << "rightBranch3 entry\n";
			return ch_handled();
		case TestEvent::CHE_EXIT:
			std::cout << "rightBranch3 exit\n";
			return ch_handled();
		case TestEvent::CHE_PARENT:
			std::cout << "rightBranch3 parent\n";
			return ch_parent(&TestHSM::rightBranch2);
		case TestEvent::TE_BACK:
			return ch_transition(&TestHSM::leftBranch1);
		default:
			return ch_parent(&TestHSM::rightBranch2);
		}
	};

	TestHSM() : CHsm<TestHSM,TestEvent>(&TestHSM::topState, &TestHSM::leftBranch2) {
		//
	};

};

#endif