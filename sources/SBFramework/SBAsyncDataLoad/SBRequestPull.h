#pragma once
#include <string>
#include <memory>
#include <map>
#include <list>

namespace SB
{
	class Request;

	typedef std::shared_ptr<Request> RequestPtr;

	class RequestPull
	{
		friend class Request;
	public:
		void SetUrlPrefix(const std::string& URL);
		const std::string& GetUrlPrefix();

		template<typename T>
		std::shared_ptr<T> CreateRequest(const std::string& URL);

		void SetCountOfSimultaneousRequests(int count);

		void Update();

	private:
		void AddToPending(const RequestPtr& request);

		void RemoveFromRunning(Request* request);

		std::list<RequestPtr> m_pending;
		
		std::map<Request*, RequestPtr> m_running;

		std::string m_urlPrefix;

		int m_runningCount;
		int m_maxCount;
	};

	template<typename T>
	inline std::shared_ptr<T> RequestPull::CreateRequest(const std::string& URL)
	{
		T* r = new T(URL, this);
		std::shared_ptr<T> ptr(r);
		AddToPending(std::static_pointer_cast<Request>(ptr));
		return ptr;
	}
}