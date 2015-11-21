#pragma once
#include <string>
#include <memory>
#include <map>

namespace SB
{
	class Request;
	class RequestData;
	class RequestTexture;

	typedef std::shared_ptr<RequestData> RequestDataPtr;
	typedef std::shared_ptr<RequestTexture> RequestTexturePtr;

	class RequestPull
	{
		friend class Request;
	public:
		void SetUrlPrefix(const std::string& URL);
		const std::string& GetUrlPrefix();

		RequestDataPtr CreateDataRequest(const std::string& URL);
		RequestTexturePtr CreateTextureRequest(const std::string& URL);

	private:
		void AddToPending(RequestDataPtr request);
		void AddToPending(RequestTexturePtr request);
		void RemoveFromPending(Request* request);

		std::map<Request*, RequestDataPtr> m_pendingData;
		std::map<Request*, RequestTexturePtr> m_pendingTextures;
		std::string m_urlPrefix;
	};
}