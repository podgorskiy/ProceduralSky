#pragma once
#include <string>

namespace SB
{
	class RequestPull;
	class RequestData;
	class RequestTexture;

	class Request
	{
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
		Request(const std::string& URL, RequestPull* requestPull);
		
		void OnError();

		virtual void OnLoad(const void* buffer, int size);

		RequestPull* m_requestPull;

		bool m_done;
		bool m_failed;
		std::string m_url;
	};
}