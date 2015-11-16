#pragma once 
#include <vector>
#include <map>

namespace SB
{
	class EventManager;

	template<typename EventType>
	class EventReceiver
	{
		friend class EventManager;
	public:
		EventReceiver();
		virtual ~EventReceiver();

		virtual void EventReceiver_OnEvent(const EventType& event) = 0;
	private:
		EventManager* m_eventManager;
	};

	class EventManager
	{
	public:
		template<typename EventType>
		void AttachReceiver(EventReceiver<EventType>* receiver);

		template<typename EventType>
		void DetachReceiver(EventReceiver<EventType>* receiver);

		template<typename EventType>
		void Dispatch(const EventType& event);

	private:
		struct IType
		{
			typedef int TypeID;
		protected:
			static int m_idGenerator;
		};

		template<typename T>
		struct DType: public IType
		{
			static IType::TypeID GetTypeID()
			{
				static IType::TypeID TypeId = m_idGenerator++;
				return TypeId;
			}
		};

		class ISpecificEventManager
		{
		};

		template<typename EventType>
		class SpecificEventManager : public ISpecificEventManager
		{
		public:
			void Dispatch(const EventType& event);

			void AttachReceiver(EventReceiver<EventType>* receiver);

			void DetachReceiver(EventReceiver<EventType>* receiver);

		private:
			std::vector< EventReceiver<EventType>* > m_eventReceivers;
		};

		template<typename EventType>
		SpecificEventManager<EventType>* GetSpecificEventManager();

		std::map<IType::TypeID, ISpecificEventManager*> m_specificEventManagers;
	};

	template<typename EventType>
	inline EventReceiver<EventType>::EventReceiver() :m_eventManager(NULL)
	{}

	template<typename EventType>
	inline EventReceiver<EventType>::~EventReceiver()
	{
		if (m_eventManager != NULL)
		{
			m_eventManager->DetachReceiver(this);
		}
	}

	template<typename EventType>
	inline void EventManager::AttachReceiver(EventReceiver<EventType>* receiver)
	{
		SpecificEventManager<EventType>* eventManager = GetSpecificEventManager<EventType>();
		eventManager->AttachReceiver(receiver);
		receiver->m_eventManager = this;
	}

	template<typename EventType>
	inline void EventManager::DetachReceiver(EventReceiver<EventType>* receiver)
	{
		SpecificEventManager<EventType>* eventManager = GetSpecificEventManager<EventType>();
		eventManager->DetachReceiver(receiver);
		receiver->m_eventManager = this;
	}
	
	template<typename EventType>
	inline void EventManager::Dispatch(const EventType& event)
	{
		SpecificEventManager<EventType>* eventManager = GetSpecificEventManager<EventType>();
		eventManager->Dispatch(event);
	}

	template<typename EventType>
	inline EventManager::SpecificEventManager<EventType>* EventManager::GetSpecificEventManager()
	{
		std::map<IType::TypeID, ISpecificEventManager*>::iterator it = m_specificEventManagers.find(DType<EventType>::GetTypeID());
		if (it == m_specificEventManagers.end())
		{
			SpecificEventManager<EventType>* specificEventManager = new SpecificEventManager<EventType>;
			m_specificEventManagers[DType<EventType>::GetTypeID()] = specificEventManager;
			return specificEventManager;
		}
		else
		{
			return static_cast<SpecificEventManager<EventType>*>(it->second);
		}
	}
	
	template<typename EventType>
	void EventManager::SpecificEventManager<EventType>::Dispatch(const EventType& event)
	{
		for (std::vector< EventReceiver<EventType>* >::iterator it = m_eventReceivers.begin(); it != m_eventReceivers.end(); ++it)
		{
			(*it)->EventReceiver_OnEvent(event);
		}
	}

	template<typename EventType>
	void EventManager::SpecificEventManager<EventType>::AttachReceiver(EventReceiver<EventType>* receiver)
	{
		for (std::vector< EventReceiver<EventType>* >::iterator it = m_eventReceivers.begin(); it != m_eventReceivers.end(); ++it)
		{
			if ((*it) == receiver)
			{
				return;
			}
		}
		m_eventReceivers.push_back(receiver);
	}

	template<typename EventType>
	void EventManager::SpecificEventManager<EventType>::DetachReceiver(EventReceiver<EventType>* receiver)
	{
		for (std::vector< EventReceiver<EventType>* >::iterator it = m_eventReceivers.begin(); it != m_eventReceivers.end(); ++it)
		{
			if ((*it) == receiver)
			{
				m_eventReceivers.erase(it);
			}
		}
	}
}
