#pragma once
#include <memory>

namespace SB
{
	class PVRReader;
	class Texture;
	typedef std::shared_ptr<Texture> TexturePtr;

	class Texture
	{
		friend class SB::PVRReader;
	public:
		enum EPVRTColourSpace
		{
			ePVRTCSpacelRGB = 0,
			ePVRTCSpacesRGB = 1
		};

		enum EPVRTPixelFormat
		{
			ePVRTPF_PVRTCI_2bpp_RGB = 0,
			ePVRTPF_PVRTCI_2bpp_RGBA = 1,
			ePVRTPF_PVRTCI_4bpp_RGB = 2,
			ePVRTPF_PVRTCI_4bpp_RGBA = 3,
			ePVRTPF_DXT1 = 7,
			ePVRTPF_DXT2 = 8,
			ePVRTPF_DXT3 = 9,
			ePVRTPF_DXT4 = 10,
			ePVRTPF_DXT5 = 11
		};

		struct TextureHeader
		{
			EPVRTPixelFormat pixelFormatCompressed;
			int	pixelFormatUncompressed;
			EPVRTColourSpace colourSpace;
			int	height;
			int	width;
			int	MIPMapCount;
			bool cubemap;
		};

		~Texture();

		static TexturePtr GenerateTexture();
		
		void Bind(int slot);
		
		void UnBind();

	private:
		Texture();
		TextureHeader header;
		unsigned int m_textureHandle;
	};
}