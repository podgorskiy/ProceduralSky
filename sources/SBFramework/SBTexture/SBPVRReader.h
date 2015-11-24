#pragma once

namespace SB
{
	class IFile;
	class Texture;

	class PVRReader
	{
	public:
		void DeSerializeTexture(Texture* texture, const IFile* file);

	private:
	};
}