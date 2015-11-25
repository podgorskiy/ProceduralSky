#include "SBPVRReader.h"
#include "SBTexture.h"
#include "SBCommon.h"
#include "SBFileSystem/IFile.h"
#include "SBOpenGLHeaders.h"

using namespace SB;

void PVRReader::DeSerializeTexture(Texture* texture, const IFile* file)
{
	if (!file->Valid())
	{
		return;
	}

	file->Seek(0);

	Texture::TextureHeader& header = texture->header;
	int dummy;
	int version;
	int flags;
	int channelType;
	int depth;
	int numSurfaces;
	int numFaces;
	int metaDataSize;
	int pixelFormatCompressed;
	int colourSpace;

	file->ReadInt(version);
	file->ReadInt(flags);
	file->ReadInt(pixelFormatCompressed);
	file->ReadInt(header.pixelFormatUncompressed);
	file->ReadInt(colourSpace);
	file->ReadInt(channelType);
	file->ReadInt(header.height);
	file->ReadInt(header.width);
	file->ReadInt(depth);
	file->ReadInt(numSurfaces);
	file->ReadInt(numFaces);
	file->ReadInt(header.MIPMapCount);
	file->ReadInt(metaDataSize);

	header.pixelFormatCompressed = static_cast<SB::Texture::EPVRTPixelFormat>(pixelFormatCompressed);
	header.colourSpace = static_cast<SB::Texture::EPVRTColourSpace>(colourSpace);

	if (version != 0x03525650)
	{
		LOGW("Error reading texture\n");
		return;
	}

	assert(version == 0x03525650);
	assert(flags == 0 || flags == 2);
	assert(depth == 1);
	assert(header.colourSpace == 1 || header.colourSpace == 0);
	assert(channelType == 0);
	assert(numSurfaces == 1);
	assert(numFaces == 1 || numFaces == 6);

	header.cubemap = numFaces == 6;

	file->Seek(file->Tell() + metaDataSize);

	texture->Bind(0);
	
	char* tempBuffer = NULL;
	const char* p = file->GetPointerConst();
	if (p != nullptr)
	{
		p += file->Tell();
	}
	else
	{ 
		int size = file->GetSize() - file->Tell();
		tempBuffer = new char[size];
		file->Read(tempBuffer, size);
		p = tempBuffer;
	}

	int blockSizeBytes = 0;
	int blockSizeTexels = 0;
	unsigned int compressionType = 0;
	switch (header.pixelFormatCompressed)
	{
	case Texture::ePVRTPF_DXT1:
		compressionType = COMPRESSED_RGB_S3TC_DXT1_EXT;
		blockSizeBytes = 8;
		blockSizeTexels = 16;
		break;
	case Texture::ePVRTPF_DXT2:
	case Texture::ePVRTPF_DXT3:
		compressionType = COMPRESSED_RGBA_S3TC_DXT3_EXT;
		blockSizeBytes = 16;
		blockSizeTexels = 16;
		break;
	case Texture::ePVRTPF_DXT4:
	case Texture::ePVRTPF_DXT5:
		compressionType = COMPRESSED_RGBA_S3TC_DXT5_EXT;
		blockSizeBytes = 16;
		blockSizeTexels = 16;
	case Texture::ePVRTPF_PVRTCI_2bpp_RGB:
		compressionType = COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		blockSizeBytes = 8;
		blockSizeTexels = 32;
		break;
	case Texture::ePVRTPF_PVRTCI_2bpp_RGBA:
		compressionType = COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
		blockSizeBytes = 8;
		blockSizeTexels = 32;
		break;
	case Texture::ePVRTPF_PVRTCI_4bpp_RGB:
		compressionType = COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		blockSizeBytes = 8;
		blockSizeTexels = 16;
		break;
	case Texture::ePVRTPF_PVRTCI_4bpp_RGBA:
		compressionType = COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		blockSizeBytes = 8;
		blockSizeTexels = 16;
		break;
	}

	int mipmapDivider = 1;
	for (int mipmap = 0; mipmap < header.MIPMapCount; ++mipmap)
	{
		for (int face = 0; face < numFaces; ++face)
		{
			if (header.pixelFormatUncompressed == 0)
			{
				int width = header.width / mipmapDivider;
				int height = header.height / mipmapDivider;
				int size = (width * height * blockSizeBytes) / blockSizeTexels;
				if (size < blockSizeBytes)
				{
					size = blockSizeBytes;
				}
				int faceType = header.cubemap ? (GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : GL_TEXTURE_2D;
				glCompressedTexImage2D(faceType, mipmap, compressionType, width, height, 0, size, p);
				p += size;
			}
			else
			{
				assert(false);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.width, header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			}
		}
		mipmapDivider *= 2;
	}

	SafeDeleteArray(tempBuffer);

	if (header.MIPMapCount > 1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture->UnBind();
}
