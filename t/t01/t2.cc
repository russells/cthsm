/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENCE" (Revision 42):
 * <russells@adelie.cx> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Russell Steicke.
 * ----------------------------------------------------------------------------
 */

#include "cthsm.hh"
#include <iostream>

using namespace CTHSM;

bool tact_called = false;

class E2 : public Event {
public:
	E2(int n) : Event(n) { };
	enum {
		//* We must start our event numbers at or after CTHE_USER.
		TE_ONE = CTHE_USER,
		TE_TWO,
		TE_THREE,
	};
};


class T2 : public CTHsm<T2, E2> {

public:
	T2() : CTHsm<T2,E2>(&T2::leftBranch)
	{
		cthsmStart();
	};

	CTHsmState topState(E2 e) {
		switch (e.event()) {
		case E2::CTHE_PARENT:
			return CTH_I_AM_THE_TOP_STATE;
		case E2::CTHE_ENTRY:
			return cth_handled();
		case E2::CTHE_EXIT:
			return cth_handled();
		default:
			return cth_handled();
		}
	};

	CTHsmState leftBranch(E2 e)
	{
		switch (e.event()) {
		case E2::TE_ONE:
			return cth_transition(&T2::rightBranch, &T2::myTact);
		default:
			return cth_parent(&T2::topState);
		}
	}

	CTHsmState rightBranch(E2 e)
	{
		switch (e.event()) {
		default:
			return cth_parent(&T2::topState);
		}
	}

	void myTact(void)
	{
		tact_called = true;
	}
};

int main(int argc, char **argv)
{
	{
		T2 t2;
		t2.sendEvent(E2(E2::TE_ONE));
	}
	if (tact_called)
		return 0;
	else {
		std::cerr << "tact_called was not set in t2.cc\n";
		return 99;
	}
}
