#include "SBMeshBufferConstructor.h"
#include "SBMesh.h"
#include "SBCommon.h"
#include <cassert>

using namespace SB;

struct quantinizedNormal
{
	char x;
	char y;
	char z;
	char _dummy;
};

GeneralType::operator float(){
	assert(m_type == FLOAT);
	return m_f;
}

float& GeneralType::operator =(float& x){
	assert(m_type == FLOAT);
	return (m_f = x);
}

GeneralType::operator glm::vec2(){
	assert(m_type == VEC2);
	return m_vec2;
}

glm::vec2& GeneralType::operator =(glm::vec2& x){
	assert(m_type == VEC2);
	return (m_vec2 = x);
}

GeneralType::operator glm::vec3(){
	assert(m_type == VEC3);
	return m_vec3;
}

glm::vec3& GeneralType::operator =(glm::vec3& x){
	assert(m_type == VEC3);
	return (m_vec3 = x);
}

GeneralType::operator glm::vec4(){
	assert(m_type == VEC4);
	return m_vec4;
}

glm::vec4& GeneralType::operator =(glm::vec4& x){
	assert(m_type == VEC4);
	return (m_vec4 = x);
}

GeneralType::GeneralType(float x)	
{
	m_type = FLOAT;
	m_f = x;
}

GeneralType::GeneralType(glm::vec2 x)	
{
	m_type = VEC2;
	m_vec2 = x;
}

GeneralType::GeneralType(glm::vec3 x)	
{
	m_type = VEC3;
	m_vec3 = x;
}

GeneralType::GeneralType(glm::vec4 x)	
{
	m_type = VEC4;
	m_vec4 = x;
}

unsigned int BufferConstructor::GetIndex(const std::vector<unsigned int>& links)
{
	std::map<std::vector<unsigned int>, unsigned int>::iterator it = m_links.find(links);
	if (it != m_links.end())
	{
		return it->second;
	}
	return -1;
}

unsigned int BufferConstructor::PushVertex(const SmartVertex& v)
{
	unsigned index = GetIndex(v.m_links);
	if (index != -1)
	{
		return index;
	}
	else
	{
		index = m_vertices.size();
		m_vertices.push_back(v);
		m_links[v.m_links] = index;
		return index;
	}
}

void BufferConstructor::ConvertMesh(Mesh* mesh)
{
	SafeDeleteArray(mesh->m_rawIntIndices);
	SafeDeleteArray(mesh->m_rawShortIndices);
	SafeDeleteArray(mesh->m_rawbuffer);
	Mesh::VerticesMaps*(& vmaps) = mesh->m_verticesMaps;

	int indicesCount = 0;
	bool integer = false;

	for (std::vector<int>::iterator it = vmaps->m_indices.begin(); it != vmaps->m_indices.end(); ++it)
	{
		if (*it > 0xFFFF)
		{
			integer = true;
		}
	}

	mesh->m_indeciesCount = vmaps->m_indices.size();
	if (integer)
	{
		mesh->m_rawIntIndices = new unsigned int[mesh->m_indeciesCount];
	}
	else
	{
		mesh->m_rawShortIndices = new unsigned short[mesh->m_indeciesCount];
	}
	
	for (std::vector<int>::iterator it = vmaps->m_indices.begin(); it != vmaps->m_indices.end();)
	{
		SmartVertex vertex;

		if(mesh->m_voffset != -1)
		{
			int posIndex = *(it + mesh->m_voffset);
			vertex.m_vdata.push_back(vmaps->m_vertices[posIndex]);
			vertex.m_links.push_back(posIndex);
		}
		if(mesh->m_noffset != -1)
		{
			int normalIndex = *(it + mesh->m_noffset);
			vertex.m_vdata.push_back(vmaps->m_normals[normalIndex]);
			vertex.m_links.push_back(normalIndex);
		}
		if(mesh->m_coffset != -1)
		{
			int colorIndex = *(it + mesh->m_coffset);
			vertex.m_vdata.push_back(vmaps->m_color[colorIndex]);
			vertex.m_links.push_back(colorIndex);
		}
		if(mesh->m_toffset1 != -1)
		{
			int tex1Index = *(it + mesh->m_toffset1);
			vertex.m_vdata.push_back(vmaps->m_textcoord1[tex1Index]);
			vertex.m_links.push_back(tex1Index);
		}
		if(mesh->m_toffset2 != -1)
		{
			int tex2Index = *(it + mesh->m_toffset2);
			vertex.m_vdata.push_back(vmaps->m_textcoord2[tex2Index]);
			vertex.m_links.push_back(tex2Index);
		}

		it += mesh->m_stride;

		unsigned int index = PushVertex(vertex);

		if (integer)
		{
			mesh->m_rawIntIndices[indicesCount++] = index;
		}
		else
		{
			mesh->m_rawShortIndices[indicesCount++] = static_cast<unsigned short>(index);
		}
	}
	
	int offset = 0;

	if(mesh->m_voffset != -1)
	{
		mesh->m_voffset = offset;
		int size = 3 * sizeof(float);
		offset += size;
	}	
	if (mesh->m_noffset != -1)
	{
		mesh->m_noffset = offset;
		int size = sizeof(int); // 3 bytes + align
		offset += size;
	}
	if(mesh->m_coffset != -1)
	{
		mesh->m_coffset = offset;
		int size = 3 * sizeof(float);
		offset += size;
	}
	if(mesh->m_toffset1 != -1)
	{
		mesh->m_toffset1 = offset;
		int size = 2 * sizeof(float);
		offset += size;
	}
	if(mesh->m_toffset2 != -1)
	{
		mesh->m_toffset2 = offset;
		int size = 2 * sizeof(float);
		offset += size;
	}

	mesh->m_stride = offset;

	mesh->m_rawbuffer = new float[m_vertices.size() * mesh->m_stride];
	mesh->m_verticesCount = m_vertices.size();

	int rawbufferPointer = 0;

	for (std::vector<const SmartVertex>::iterator it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		std::vector<GeneralType>::iterator itt = (*it).m_vdata.begin(); 
		{
			GeneralType v = (*itt++);

			if(mesh->m_voffset != -1)
			{
				glm::vec3 pos = v;
				mesh->m_rawbuffer[rawbufferPointer++] = pos.x;
				mesh->m_rawbuffer[rawbufferPointer++] = pos.y;
				mesh->m_rawbuffer[rawbufferPointer++] = pos.z;
			}			
			if (mesh->m_noffset != -1)
			{
				v = (*itt++);
				glm::vec3 normal = v;
				normal = glm::normalize(normal) * 127.0f;
				float normalFloat;
				quantinizedNormal& qnormal = reinterpret_cast<quantinizedNormal&>(normalFloat);
				qnormal.x = static_cast<char>(std::floor(normal.x));
				qnormal.y = static_cast<char>(std::floor(normal.y));
				qnormal.z = static_cast<char>(std::floor(normal.z));
				mesh->m_rawbuffer[rawbufferPointer++] = normalFloat;
			}
			if(mesh->m_coffset != -1)
			{
				v = (*itt++);
				glm::vec3 color = v;
				mesh->m_rawbuffer[rawbufferPointer++] = color.x;
				mesh->m_rawbuffer[rawbufferPointer++] = color.y;
				mesh->m_rawbuffer[rawbufferPointer++] = color.z;
			}
			if(mesh->m_toffset1 != -1)
			{
				v = (*itt++);
				glm::vec2 tex1 = v;
				mesh->m_rawbuffer[rawbufferPointer++] = tex1.x;
				mesh->m_rawbuffer[rawbufferPointer++] = tex1.y;
			}
			if(mesh->m_toffset2 != -1)
			{
				v = (*itt++);
				glm::vec2 tex2 = v;
				mesh->m_rawbuffer[rawbufferPointer++] = tex2.x;
				mesh->m_rawbuffer[rawbufferPointer++] = tex2.y;
			}
		}
	}
	
	SafeDelete(vmaps);
}