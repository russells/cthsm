#include "testhsm.hh"

int main(int argc, char **argv)
{
	std::cout << "----- Making a new TestHSM\n";
	TestHSM *th = new TestHSM;
	//std::cout << "----- Sending TE_TWO\n";
	//th->sendEvent(TestEvent(TestEvent::TE_TWO));
	std::cout << "----- Sending TE_THREE\n";
	th->sendEvent(TestEvent(TestEvent::TE_THREE));
	std::cout << "----- Sending TE_BACK\n";
	th->sendEvent(TestEvent(TestEvent::TE_BACK));
	std::cout << "----- Sending TE_BACKAGAIN\n";
	th->sendEvent(TestEvent(TestEvent::TE_BACKAGAIN));
	std::cout << "----- Deleting the TestHSM\n";
	delete th;
	return 0;
}

