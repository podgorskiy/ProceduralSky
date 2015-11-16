#include "SBEventManager.h"

int SB::EventManager::IType::m_idGenerator = 0;

/*

struct CustomEventType
{
	std::string message;
};

class fooo : public SB::EventReceiver < CustomEventType >, public SB::EventReceiver < int >
{
public:
	void EventReceiver_OnEvent(const CustomEventType& sustomEventType)
	{
		printf("CustomEventType: %s\n", sustomEventType.message.c_str());
	}

	void EventReceiver_OnEvent(const int& number)
	{
		printf("number: %d\n", number);
	}
};

fooo bar;

SB::EventManager em;

em.AttachReceiver<CustomEventType>(&bar);
em.AttachReceiver<int>(&bar);

CustomEventType someEvent;
someEvent.message = "Hello!";

em.Dispatch(someEvent);
em.Dispatch(42);
*/