#include "SBSceneSerializer.h"
#include "SBCommon.h"
#include "SBNode.h"
#include "SBScene.h"
#include "SBMesh.h"
#include "IFile.h"

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

void SceneSerializer::WriteNode(const Node* node, IFile* file)
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

bool SceneSerializer::Serialize(const Node* scene, IFile* file)
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

SceneSerializer::MeshID SceneSerializer::PushMesh(const Mesh* mesh)
{
	m_meshs[m_meshID] = mesh;
	return m_meshID++;
}

void SceneSerializer::WriteMesh(const Mesh* mesh, MeshID id, IFile* file)
{
	file->WriteInt(DataChunks::MESH);
	file->WriteInt(id);
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


Scene* SceneSerializer::DeSerialize(IFile* file)
{
	if (!file->Valid())
	{
		return false;
	}

	file->Seek(0);

	bool endOfFile = false;

	Scene* root = new Scene;

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

void SceneSerializer::ReadNode(Node* node, IFile* file)
{
	std::string name;
	file->ReadString(name);
	node->SetName(name);

	glm::mat4 transform;
	file->Read(reinterpret_cast<char*>(&transform), sizeof(transform));
	node->SetLocalTransform(transform);

	int renderable = 0;
	file->ReadInt(renderable);
	node->SetRenderable(static_cast<bool>(renderable));

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
			child->SetScene(node->GetScene());
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

SceneSerializer::MeshID SceneSerializer::ReadMesh(Mesh* mesh, IFile* file)
{
	int id;
	file->ReadInt(id);
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

	assert(!endOfFile);
	
	typedef  bool (IFile::*ReadFn)(char* source, int size);
	ReadFn readFn = NULL;

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

	assert(!endOfFile);

	return id;
}