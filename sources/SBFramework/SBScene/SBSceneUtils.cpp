#include "SBSceneUtils.h" 

#include "SBNode.h"
#include "SBMesh.h"
#include "SBCommon.h"

#include <cctype>
#include <xxhash.h>

struct quantinizedNormal
{
	char x;
	char y;
	char z;
	char _dummy;
};

void SB::Utils::PushMeshDataToVideoMemory(Node* scene, bool recursive)
{
	const std::vector<Mesh*>& meshes = scene->GetMeshs();

	for (std::vector<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		(*it)->CreateVBO();
	}

	if (recursive)
	{
		int count = scene->GetChildCount();
		for (int i = 0; i < count; i++)
		{
			Node* n = scene->GetChild(i);
			PushMeshDataToVideoMemory(n, recursive);
		}
	}
}

void SB::Utils::PushMeshDataToVideoMemory(const std::vector<SB::Mesh*>& input)
{
	for (std::vector<Mesh*>::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		(*it)->CreateVBO();
	}
}

void SB::Utils::Merge(SB::Node* sceneBase, SB::Node* sceneMerge)
{
	std::vector<Node*> rootChilds;
	int count = sceneMerge->GetChildCount();
	for (int i = 0; i < count; i++)
	{
		Node* n = sceneMerge->GetChild(i);
		rootChilds.push_back(n);
	}
	for (std::vector<Node*>::iterator it = rootChilds.begin(); it != rootChilds.end(); ++it)
	{
		sceneBase->AddChild(*(*it));
	}
	delete sceneMerge;
}

bool Match(const char *string, const char *pattern)
{
	while (*pattern)
	{
		if (*pattern == '?')
		{
			if (!*string)
			{
				return false;
			}
			++string;
			++pattern;
		}
		else if (*pattern == '*')
		{
			if (Match(string, pattern + 1))
			{
				return true;
			}
			if (*string && Match(string + 1, pattern))
			{
				return true;
			}
			return false;
		}
		else
		{
			if (toupper(*string++) != toupper(*pattern++))
			{
				return false;
			}
		}
	}

	return !*string && !*pattern;
}


void SB::Utils::RemoveNodes(Node* node, const char* pattern)
{
	if (node == NULL)
	{
		return;
	}
	if (Match(node->GetName().c_str(), pattern))
	{
		node->DetachNode();
		delete node;
	}
	else
	{
		int count = node->GetChildCount();
		std::vector<Node*> nodesToRemove;

		for (int i = 0; i < count; i++)
		{
			nodesToRemove.push_back(node->GetChild(i));
		}
		for (std::vector<Node*>::iterator it = nodesToRemove.begin(); it != nodesToRemove.end(); ++it)
		{
			RemoveNodes(*it, pattern);
		}
	}
}

void DetachALL(SB::Node* node, std::vector<std::pair<SB::Node*, glm::mat4> >& nodecollector)
{
int count = node->GetChildCount();
std::vector<SB::Node*> nodesToDetach;
for (int i = 0; i < count; i++)
{
	nodesToDetach.push_back(node->GetChild(i));
}
for (std::vector<SB::Node*>::iterator it = nodesToDetach.begin(); it != nodesToDetach.end(); ++it)
{
	DetachALL(*it, nodecollector);
}
nodecollector.push_back(std::make_pair(node, node->GetAbsoluteTransform()));
node->DetachNode();
}

void SB::Utils::MakeFlat(std::vector<SB::Mesh*>& meshArray, SB::Node* scene)
{
	std::vector<std::pair<SB::Node*, glm::mat4> > nodecollector;
	DetachALL(scene, nodecollector);

	for (std::vector<std::pair<SB::Node*, glm::mat4> >::iterator it = nodecollector.begin(); it != nodecollector.end(); ++it)
	{
		Node* node = it->first;
		glm::mat4 transform = it->second;

		assert(node->GetChildCount() == 0);

		const std::vector<Mesh*>& meshes = node->GetMeshs();
		for (std::vector<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
		{
			Mesh* mesh = *it;
			for (int i = 0; i < mesh->m_verticesCount; ++i)
			{
				int offset = (i*mesh->m_stride + mesh->m_voffset) / sizeof(float);
				float vx = mesh->m_rawbuffer[offset + 0];
				float vy = mesh->m_rawbuffer[offset + 1];
				float vz = mesh->m_rawbuffer[offset + 2];

				glm::vec4 v(vx, vy, vz, 1.0f);

				v = transform * v;

				mesh->m_rawbuffer[offset + 0] = v.x;
				mesh->m_rawbuffer[offset + 1] = v.y;
				mesh->m_rawbuffer[offset + 2] = v.z;

				float normalraw = mesh->m_rawbuffer[i*mesh->m_stride / sizeof(float) + mesh->m_noffset / sizeof(float)];
				quantinizedNormal& qnormal = reinterpret_cast<quantinizedNormal&>(normalraw);

				glm::vec3 normal(qnormal.x, qnormal.y, qnormal.z);

				normal = glm::mat3(transform) * normal;

				normal = glm::normalize(normal) * 127.0f;

				quantinizedNormal qnormalNew;
				qnormalNew.x = normal.x;
				qnormalNew.y = normal.y;
				qnormalNew.z = normal.z;

				float& normalrawNew = reinterpret_cast<float&>(qnormalNew);

				mesh->m_rawbuffer[i*mesh->m_stride / sizeof(float) + mesh->m_noffset / sizeof(float)] = normalrawNew;
			}
			meshArray.push_back(mesh);
		}

		delete node;
	}
}

void SB::Utils::BatchMeshes(const std::vector<SB::Mesh*>& input, std::vector<SB::Mesh*>& output)
{
	if (input.size() < 2)
	{
		return;
	}

	std::map<long long, std::vector<Mesh*> > m_meshBanks;
	for (std::vector<SB::Mesh*>::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		long long hash = GetHashFromMesh(*it);
		std::vector<Mesh*>& mashBank = m_meshBanks[hash];
		mashBank.push_back(*it);
	}
	for (std::map<long long, std::vector<Mesh*> >::iterator it = m_meshBanks.begin(); it != m_meshBanks.end(); ++it)
	{
		ProcessMeshBank(it->second, output);
	}
}

void SB::Utils::ProcessMeshBank(const std::vector<SB::Mesh*>& input, std::vector<SB::Mesh*>& output)
{
	std::vector<SB::Mesh*>::const_iterator base = input.begin();
	for (; base != input.end();)
	{
		std::vector<SB::Mesh*>::const_iterator it = base + 1;
		for (; it != input.end(); ++it)
		{
			int firstIndicesSize = (*base)->m_indeciesCount;
			int secondIndicesSize = (*it)->m_indeciesCount;
			if (firstIndicesSize + secondIndicesSize > 0xFFFF)
			{
				break;
			}
			else
			{
				AppendMesh(*base, *it);
			}
		}
		output.push_back(*base);
		base = it;
	}
}


int SB::Utils::GetIndex(SB::Mesh* mesh, int index)
{
	if (mesh->m_rawIntIndices != NULL)
	{
		return mesh->m_rawIntIndices[index];
	}
	else if (mesh->m_rawShortIndices != NULL)
	{
		return mesh->m_rawShortIndices[index];
	}
	return -1;
}

void SB::Utils::AppendMesh(SB::Mesh* base, SB::Mesh* other)
{
	assert(base->m_stride == other->m_stride);
	int firstBufferSize = base->m_verticesCount * base->m_stride / sizeof(float);
	int secondBufferSize = other->m_verticesCount * other->m_stride / sizeof(float);

	float* newRawBuffer = new float[firstBufferSize + secondBufferSize];

	memcpy(newRawBuffer, base->m_rawbuffer, firstBufferSize * sizeof(float));
	memcpy(newRawBuffer + firstBufferSize, other->m_rawbuffer, secondBufferSize * sizeof(float));
	SafeDeleteArray(base->m_rawbuffer);
	SafeDeleteArray(other->m_rawbuffer);
	base->m_rawbuffer = newRawBuffer;

	int firstIndicesSize = base->m_indeciesCount;
	int secondIndicesSize = other->m_indeciesCount;
	if (firstIndicesSize + secondIndicesSize > 0xFFFF)
	{
		unsigned int* newRawBuffer = new unsigned int[firstIndicesSize + secondIndicesSize];
		for (int i = 0; i < firstIndicesSize; i++)
		{
			int index = GetIndex(base, i);
			newRawBuffer[i] = index;
		}
		for (int i = 0; i < secondIndicesSize; i++)
		{
			int index = GetIndex(other, i);
			newRawBuffer[i + firstIndicesSize] = index + base->m_verticesCount;
		}
		SafeDeleteArray(base->m_rawIntIndices);
		SafeDeleteArray(base->m_rawShortIndices);
		base->m_rawIntIndices = newRawBuffer;
	}
	else
	{
		unsigned short* newRawBuffer = new unsigned short[firstIndicesSize + secondIndicesSize];
		for (int i = 0; i < firstIndicesSize; i++)
		{
			short index = GetIndex(base, i);
			newRawBuffer[i] = index;
		}
		for (int i = 0; i < secondIndicesSize; i++)
		{
			short index = GetIndex(other, i);
			newRawBuffer[i + firstIndicesSize] = index + base->m_verticesCount;
		}
		SafeDeleteArray(base->m_rawIntIndices);
		SafeDeleteArray(base->m_rawShortIndices);
		base->m_rawShortIndices = newRawBuffer;
	}
	
	SafeDeleteArray(other->m_rawIntIndices);
	SafeDeleteArray(other->m_rawShortIndices);

	base->m_indeciesCount = base->m_indeciesCount + other->m_indeciesCount;
	base->m_verticesCount = base->m_verticesCount + other->m_verticesCount;

	delete other;
}


long long SB::Utils::GetHashFromMesh(SB::Mesh* mesh)
{
	struct HashValues
	{
		int m_stride;
		int m_voffset;
		int m_noffset;
		int m_coffset;
		int m_toffset1;
		int m_toffset2;
		char materialinstance[64];
	};
	HashValues v;
	memset(v.materialinstance, 0, 64);
	strcpy(v.materialinstance, mesh->m_materialName.c_str());
	v.m_stride = mesh->m_stride;
	v.m_voffset = mesh->m_voffset;
	v.m_noffset = mesh->m_noffset;
	v.m_coffset = mesh->m_coffset;
	v.m_toffset1 = mesh->m_toffset1;
	v.m_toffset2 = mesh->m_toffset2;
	long long hash = XXH64(&v, sizeof(HashValues), 0);
	return hash;
}