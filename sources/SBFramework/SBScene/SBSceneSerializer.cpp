#include "SBSceneSerializer.h"
#include "SBCommon.h"
#include "SBNode.h"
#include "SBMesh.h"
#include "SBFileSystem/IFile.h"

#include <glm/matrix.hpp>

using namespace SB;

struct DataChunks
{
	enum
	{
		NODE = 1,
		NODE_CHILDS = 2,
		NODE_END = 3,
		MESH = 4,
		MESH_BUFFERS = 5,
		MESH_BUFFERS_COMPRESSED = 6,
		MESH_BUFFERS_NOT_COMPRESSED = 7,
		MESH_BUFFERS_INT = 8,
		MESH_BUFFERS_SHORT = 9,
		MESH_END = 10
	};
};

void Serializer::WriteNode(const Node* node, IFile* file)
{
	file->WriteInt(DataChunks::NODE);
	file->WriteString(node->GetName());

	glm::mat4 transform = node->GetLocalTransform();
	file->Write(reinterpret_cast<char*>(&transform), sizeof(transform));

	file->WriteInt(static_cast<int>(node->IsRenderable()));

	const std::vector<Mesh*>& meshs = node->GetMeshs();
	
	file->WriteInt(static_cast<int>(meshs.size()));

	for (std::vector<Mesh*>::const_iterator it = meshs.begin(); it != meshs.end(); ++it)
	{
		MeshID id = PushMesh(*it);
		file->WriteInt(static_cast<int>(id));
	}

	file->WriteInt(DataChunks::NODE_CHILDS);

	int count = node->GetChildCount();
	for (int i = 0; i < count; i++)
	{
		const Node* n = node->GetChild(i);
		WriteNode(n, file);
	}

	file->WriteInt(DataChunks::NODE_END);
}

bool Serializer::SerializeScene(const Node* scene, IFile* file)
{
	if (!file->Valid())
	{
		return false;
	}

	file->Seek(0);
	
	m_meshID = 0;

	m_meshs.clear();

	WriteNode(scene, file);

	for (std::map<MeshID, const Mesh*>::iterator it = m_meshs.begin(); it != m_meshs.end(); ++it)
	{
		WriteMesh(it->second, it->first, file);
	}
	return true;
}

Serializer::MeshID Serializer::PushMesh(const Mesh* mesh)
{
	m_meshs[m_meshID] = mesh;
	return m_meshID++;
}

void Serializer::WriteMesh(const Mesh* mesh, MeshID id, IFile* file)
{
	file->WriteInt(DataChunks::MESH);
	file->WriteInt(id);
	file->WriteString(mesh->GetMaterialName().c_str());
	file->WriteString(mesh->GetTexture().c_str());
	file->WriteString(mesh->GetTexture2().c_str());
	file->WriteInt(mesh->m_stride);
	file->WriteInt(mesh->m_voffset);
	file->WriteInt(mesh->m_noffset);
	file->WriteInt(mesh->m_coffset);
	file->WriteInt(mesh->m_toffset1);
	file->WriteInt(mesh->m_toffset2);

	file->WriteInt(DataChunks::MESH_BUFFERS);

	typedef  bool (IFile::*WriteFn)(const char* source, int size);
	WriteFn writeFn = NULL;

	bool useCompression = mesh->m_verticesCount * mesh->m_stride > 1024;
	if (useCompression)
	{
		file->WriteInt(DataChunks::MESH_BUFFERS_COMPRESSED);
		writeFn = &IFile::WriteCompressed;
	}
	else
	{
		file->WriteInt(DataChunks::MESH_BUFFERS_NOT_COMPRESSED);
		writeFn = &IFile::Write;
	}
	
	file->WriteInt(mesh->m_indeciesCount);
	if (mesh->m_rawIntIndices != NULL)
	{
		file->WriteInt(DataChunks::MESH_BUFFERS_INT);
		(file->*writeFn)(reinterpret_cast<const char*>(mesh->m_rawIntIndices), mesh->m_indeciesCount * sizeof(int));
	}
	else
	{
		file->WriteInt(DataChunks::MESH_BUFFERS_SHORT);
		(file->*writeFn)(reinterpret_cast<const char*>(mesh->m_rawShortIndices), mesh->m_indeciesCount * sizeof(short));
	}
	file->WriteInt(mesh->m_verticesCount);
	(file->*writeFn)(reinterpret_cast<const char*>(mesh->m_rawbuffer), mesh->m_verticesCount * mesh->m_stride);
	
	file->WriteInt(DataChunks::MESH_END);
}


Node* Serializer::DeSerializeScene(const IFile* file)
{
	if (!file->Valid())
	{
		return nullptr;
	}

	file->Seek(0);

	bool endOfFile = false;

	Node* root = new Node;

	m_nodesMeshBindings.clear();

	while (!endOfFile)
	{
		int dataType = -1;
		endOfFile = !file->ReadInt(dataType);
		switch (dataType)
		{
			case DataChunks::NODE:
			{
				ReadNode(root, file);
				break;
			}
			case DataChunks::MESH:
			{
				Mesh* mesh = new Mesh;
				MeshID id = ReadMesh(mesh, file);
				Node* node = m_nodesMeshBindings[id];
				if (node != NULL)
				{
					node->PushMesh(mesh);
				}
				else
				{
					delete mesh;
				}
				break;
			}
		}
	}
	return root;
}

void Serializer::ReadNode(Node* node, const IFile* file)
{
	std::string name;
	file->ReadString(name);
	node->SetName(name);

	glm::mat4 transform;
	file->Read(reinterpret_cast<char*>(&transform), sizeof(transform));
	node->SetLocalTransform(transform);

	int renderable = 0;
	file->ReadInt(renderable);
	node->SetRenderable(renderable == 1);

	int meshCount = 0;
	file->ReadInt(meshCount);

	for (int i = 0; i < meshCount; ++i)
	{
		MeshID id;
		file->ReadInt(static_cast<int&>(id));
		m_nodesMeshBindings[id] = node;
	}

	bool endOfFile = false;
	int dataType = -1;

	while (!endOfFile)
	{
		endOfFile = !file->ReadInt(dataType);
		if (dataType == DataChunks::NODE_CHILDS)
		{
			break;
		}
	}

	assert(!endOfFile);

	while (!endOfFile)
	{
		endOfFile = !file->ReadInt(dataType);
		switch (dataType)
		{
		case DataChunks::NODE:
		{
			Node* child = new Node;
			ReadNode(child, file);
			node->AddChild(*child);
			break;
		}
		case DataChunks::NODE_END:
			return;
		}
	}
	return;
}

Serializer::MeshID Serializer::ReadMesh(Mesh* mesh, const IFile* file)
{
	int id = -1;
	file->ReadInt(id);
	std::string materialName;
	file->ReadString(materialName);
	mesh->SetMaterialName(materialName);
	std::string textureName;
	file->ReadString(textureName);
	mesh->SetTexture(textureName);
	std::string textureName2;
	file->ReadString(textureName2);
	mesh->SetTexture2(textureName2);
	file->ReadInt(mesh->m_stride);
	file->ReadInt(mesh->m_voffset);
	file->ReadInt(mesh->m_noffset);
	file->ReadInt(mesh->m_coffset);
	file->ReadInt(mesh->m_toffset1);
	file->ReadInt(mesh->m_toffset2);

	bool endOfFile = false;
	int dataType = -1;

	while (!endOfFile)
	{
		endOfFile = !file->ReadInt(dataType);
		if (dataType == DataChunks::MESH_BUFFERS)
		{
			break;
		}
	}

	ASSERT(!endOfFile, "Unwexpected end of file");
	
	typedef  bool (IFile::*ReadFn)(char* source, int size) const;
	ReadFn readFn = nullptr;

	int compression = -1;
	file->ReadInt(compression);
	if (compression == DataChunks::MESH_BUFFERS_COMPRESSED)
	{
		readFn = &IFile::ReadCompressed;
	}
	else if (compression == DataChunks::MESH_BUFFERS_NOT_COMPRESSED)
	{
		readFn = &IFile::Read;
	}

	file->ReadInt(mesh->m_indeciesCount);
	int indicesType = -1;
	file->ReadInt(indicesType);
	switch (indicesType)
	{
	case DataChunks::MESH_BUFFERS_INT:
		mesh->m_rawIntIndices = new unsigned int[mesh->m_indeciesCount];
		(file->*readFn)(reinterpret_cast<char*>(mesh->m_rawIntIndices), mesh->m_indeciesCount * sizeof(unsigned int));
		break;
	case DataChunks::MESH_BUFFERS_SHORT:
		mesh->m_rawShortIndices = new unsigned short[mesh->m_indeciesCount];
		(file->*readFn)(reinterpret_cast<char*>(mesh->m_rawShortIndices), mesh->m_indeciesCount * sizeof(unsigned short));
		break;
	}
	file->ReadInt(mesh->m_verticesCount);
	mesh->m_rawbuffer = new float[mesh->m_verticesCount * mesh->m_stride / sizeof(float)];
	(file->*readFn)(reinterpret_cast<char*>(mesh->m_rawbuffer), mesh->m_verticesCount * mesh->m_stride);
	
	while (!endOfFile)
	{
		endOfFile = !file->ReadInt(dataType);
		if (dataType == DataChunks::MESH_END)
		{
			break;
		}
	}

	ASSERT(!endOfFile, "Unwexpected end of file");

	return id;
}

bool Serializer::SerializeBatchList(const std::vector<SB::Mesh*> batchList, IFile* file)
{
	if (!file->Valid())
	{
		return false;
	}
	file->Seek(0);
	int id = 0;
	for (std::vector<SB::Mesh*>::const_iterator it = batchList.begin(); it != batchList.end(); ++it)
	{
		WriteMesh(*it, id++, file);
	}
	return true;
}

void Serializer::DeSerializeBatchList(std::vector<SB::Mesh*>& batchList, const IFile* file)
{
	if (!file->Valid())
	{
		return;
	}

	file->Seek(0);

	bool endOfFile = false;

	while (!endOfFile)
	{
		int dataType = -1;
		endOfFile = !file->ReadInt(dataType);
		if (dataType == DataChunks::MESH)
		{
			Mesh* mesh = new Mesh;
			MeshID id = ReadMesh(mesh, file);
			batchList.push_back(mesh);
		}
	}
}

bool Serializer::SerializeMesh(const SB::Mesh* mesh, IFile* file)
{
	if (!file->Valid())
	{
		return false;
	}
	file->Seek(0);
	WriteMesh(mesh, 0, file);
	return true;
}

SB::Mesh* Serializer::DeSerializeMesh(const IFile* file)
{
	if (!file->Valid())
	{
		return nullptr;
	}

	file->Seek(0);

	int dataType = -1;
	file->ReadInt(dataType);
	
	if (dataType == DataChunks::MESH)
	{
		Mesh* mesh = new Mesh;
		MeshID id = ReadMesh(mesh, file);
		return mesh;
	}
	return nullptr;
}