#pragma once
#include <string>

namespace SB
{
	class RequestPull;

	class RequestTexture : public Request
	{
		friend class SB::RequestPull;
	public:

	private:
		virtual void OnLoad(const void* buffer, int size){};

		RequestTexture(const std::string& URL, RequestPull* requestPull) : Request(URL, requestPull){};
	};
}