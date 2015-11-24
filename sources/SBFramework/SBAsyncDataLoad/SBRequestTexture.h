#pragma once
#include <string>
#include "SBTexture/SBTexture.h"

namespace SB
{
	class RequestPull;

	typedef std::shared_ptr<RequestTexture> RequestTexturePtr;

	class RequestTexture : public Request
	{
		friend class SB::RequestPull;
	public:

		SB::TexturePtr GetTexture();

	private:
		virtual void OnLoad(const void* buffer, int size);

		RequestTexture(const std::string& URL, bool copyData, RequestPull* requestPull);

		SB::TexturePtr m_texture;
	};
}