#pragma once
#include <string>
#include <functional>

namespace SB
{
	class RequestPull;
	class RequestData;
	class RequestTexture;

	class Request
	{
		friend class SB::RequestPull;
		friend class SB::RequestData;
		friend class SB::RequestTexture;
		friend void OnErrorRequest(void* arg);
		friend void OnloadRequest(void* arg, void* buffer, int size);
	public:
		virtual ~Request(){};
		
		bool IsReady();
		bool IsSuceeded();
		bool IsFailed();
		
	private:
		void Start();

		Request(const std::string& URL, bool copyData, RequestPull* requestPull);
		
		void OnError();

		virtual void OnLoad(const void* buffer, int size);
		
		RequestPull* m_requestPull;
		
		bool m_done;
		bool m_failed;
		bool m_copyData;
		std::string m_url;
	};
}