#include "SBSceneDAEConstructor.h"
#include "SBMeshBufferConstructor.h"

#include "SBCommon.h"
#include "SBScene.h"
#include "SBMesh.h"

#include <glm/gtx/rotate_vector.hpp>

#include <fstream>
#include <algorithm>
#include <functional>

using namespace SB;

class Tokenizer
{
public:
	Tokenizer(const char* input) :p(input)
	{
		buffer.resize(100);
	}
	const char* GetNextToken()
	{
		while (*p != NULL && *p == ' ')
		{
			p++;
		}
		const char* start = p;
		if (*start == NULL)
		{
			return NULL;
		}
		while (*p != NULL && *p != ' ')
		{
			p++;
		}
		const char* end = p;
		unsigned int size = static_cast<unsigned int>(end - start);
		if (buffer.size() < size + 1)
		{ 
			buffer.resize(size + 1);
		}
		memcpy(buffer.data(), start, size);
		buffer[size] = NULL;
		return buffer.data();
	}
private:
	const char* p;
	std::vector<char> buffer;
};

void ReadFloatArray(std::vector<float>& data, int& stride, const pugi::xml_node& source)
{
	pugi::xml_node varray = source.child("float_array");
	int count = varray.attribute("count").as_int();
	pugi::xml_node technique_common = source.child("technique_common");
	pugi::xml_node accessor = technique_common.child("accessor");
	stride = accessor.attribute("stride").as_int();
	{
		Tokenizer tokens(varray.child_value());
		data.clear();
		const char* token = NULL;
		while ((token = tokens.GetNextToken()))
		{
			data.push_back(static_cast<float>(atof(token)));
		}
	}
}

struct IsN {
	bool operator() (const char& x) const { return x == '#'; }
	typedef int argument_type;
};

void URLFILTER(std::string& x)
{
	x.erase(
		x.begin(), 
		std::find_if(x.begin(), x.end(), std::not1(IsN()) )
	);
}

bool GetURL(const pugi::xml_node& node, const char* semantic, std::string& URL)
{
	const char* v = node.attribute("semantic").as_string();
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node.attribute("source").as_string();
			URLFILTER(URL);
			return true;
		}
	}
	return false;
}
#define GETURL(node, semantic) GetURL(node, #semantic, semantic)

bool GetURLoffset(const pugi::xml_node& node, const char* semantic, std::string& URL, int& offset)
{
	const char* v = node.attribute("semantic").as_string();
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node.attribute("source").as_string();
			URLFILTER(URL);
			offset = 0;
			if (node.attribute("offset") != NULL)
			{
				offset = node.attribute("offset").as_int();
			}
			return true;
		}
	}
	return false;
}
#define GETURLO(node, semantic, offset) GetURLoffset(node, #semantic, semantic, offset)

bool GetURLoffsetSet(const pugi::xml_node& node, const char* semantic, std::string& URL, int& offset, int& set)
{
	const char* v = node.attribute("semantic").as_string();
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node.attribute("source").as_string();
			URLFILTER(URL);
			offset = 0;
			if (node.attribute("offset") != NULL)
			{
				offset = node.attribute("offset").as_int();
			}
			if (node.attribute("set") != NULL)
			{
				set = node.attribute("set").as_int();
			}
			return true;
		}
	}
	return false;
}
#define GETURLOS(node, semantic, offset, set) GetURLoffsetSet(node, #semantic, semantic, offset, set)

SceneDAEConstructor::SceneDAEConstructor() :m_daeBuffer(NULL)
{
};

SceneDAEConstructor::~SceneDAEConstructor()
{
};

bool SceneDAEConstructor::OpenDAE(const char* filename)
{
	LOGI("Reading COLLADA DAE file: %s", filename);
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	m_daeBuffer.resize(static_cast<unsigned int>(size));
	if (file.read(m_daeBuffer.data(), size))
	{
		return true;
	}
	else
	{
		return false;
	}
};

void SceneDAEConstructor::PrintDebugOutput(Node& node, int level, std::vector<bool> last)
{
	LOGI("root");
	for (int i = 0; i < node.GetChildCount(); i++)
	{
		Node* child = node.GetChild(i);
		if (child != NULL)
		{
			std::string prefix = "";
			for (int j = 0; j < level; j++)
			{
				if (last[j])
				{
					prefix += "    ";
				}
				else
				{
					prefix += "|   ";
				}
			}
			if (i == node.GetChildCount() - 1)
			{
				last.push_back(true);
			}
			else
			{
				last.push_back(false);
			}
			LOGI((prefix + "|   ").c_str());
			LOGI((prefix + "+---" + std::string(child->GetName().c_str())).c_str());
			if (child->GetChildCount() != 0)
			{
				PrintDebugOutput(*child, level + 1, last);
			}
			last.pop_back();
		}
	}
}

Scene* SceneDAEConstructor::ConstructSBScene()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_buffer(m_daeBuffer.data(), m_daeBuffer.size());

	pugi::xml_node root;

	// block: name
	{
		root = doc.first_child();
		// should always have a valid root but handle gracefully if it does
		if (!root)
		{
			LOGE("No root node in dae document!");
			return 0;
		}
		LOGI("Doc type: %s", root.name());
	}

	// block: asset
	{
		pugi::xml_node node = root.child("asset").child("contributor").child("author");
		if (node != NULL)
		{
			LOGI("author: %s", node.child_value());
		}
	}

	ReadGeometry(root);

	Scene& scene = *new Scene("root");

	glm::mat4 ind = glm::mat4();
	scene.SetLocalTransform(ind);

	ReadScene(root, scene);
	
	//print tree
	//PrintDebugOutput(scene);

	return &scene;
}

void SceneDAEConstructor::ReadGeometry(const pugi::xml_node& root)
{
	pugi::xml_node pElem;

	// block: geometry
	LOGI("loading: library_geometries");
	{
		pugi::xml_node library_geometries = root.child("library_geometries");
		for (pugi::xml_node geometry = library_geometries.child("geometry"); geometry; geometry = geometry.next_sibling("geometry"))
		{
			std::string id = geometry.attribute("id").as_string();
			LOGI("geometry: %s", id.c_str());

			pugi::xml_node mesh = geometry.child("mesh");
			pugi::xml_node spline = geometry.child("spline");
			if (mesh != NULL)
			{
				ReadMesh(mesh, id);
			}
			if (spline != NULL)
			{
				ReadSpline(spline, id);
			}
		}
	}
}

void SceneDAEConstructor::CreateBB(std::vector<glm::vec3>& v)
{

}

void SceneDAEConstructor::ReadSpline(const pugi::xml_node& spline, const std::string& id)
{
	LOGI("Read spline");

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> intangent;
	std::vector<glm::vec3> outtangent;

	int trianglesCount = 0;
	{
		std::string splineType = spline.attribute("closed").as_string();

		bool closed = splineType == "1" ? true : false;

		pugi::xml_node controlVertices = spline.child("control_vertices");
		if (controlVertices == NULL)
		{
			return;
		}

		std::string POSITION;
		std::string IN_TANGENT;
		std::string OUT_TANGENT;
		std::string INTERPOLATION;
		
		for (pugi::xml_node node = controlVertices.first_child(); node; node = node.next_sibling())
		{
			GETURL(node, POSITION);
			GETURL(node, IN_TANGENT);
			GETURL(node, OUT_TANGENT);
			GETURL(node, INTERPOLATION);
		}

		pugi::xml_node positionsE;
		pugi::xml_node intangentE;
		pugi::xml_node outtangentE;
		pugi::xml_node interpolationE;

		for (pugi::xml_node node = spline.first_child(); node; node = node.next_sibling())
		{
			if (node.attribute("id") == NULL)
				continue;
			if (strcmp(node.attribute("id").as_string(), POSITION.c_str()) == 0)
			{
				positionsE = node;
			}
			if (strcmp(node.attribute("id").as_string(), IN_TANGENT.c_str()) == 0)
			{
				intangentE = node;
			}
			if (strcmp(node.attribute("id").as_string(), OUT_TANGENT.c_str()) == 0)
			{
				outtangentE = node;
			}
		}

		if (positionsE != NULL)
		{
			//ReadDataArray(vertices, positionsE);
		}

		if (intangentE != NULL)
		{
			//ReadDataArray(intangent, intangentE);
		}

		if (outtangentE != NULL)
		{
			//ReadDataArray(outtangent, outtangentE);
		}

	}
	glm::aabb3df box;

	for (int i = 0, l = vertices.size(); i < l; i++)
	{
		box.AddPoint(vertices[i]);
	}

	if (id.find("-_") != id.npos)
	{
		return;
	}
}


void SceneDAEConstructor::ParseVertex(	
					std::string& POSITION, 
					std::string& NORMAL,
					std::string& COLOR,
					std::string& TEXCOORD1,
					std::string& TEXCOORD2,
					int vertexOffset,
					const pugi::xml_node& pChild,
					Mesh* sbmesh)
{
	if (GETURL(pChild, POSITION)) sbmesh->m_voffset = vertexOffset;
	if (GETURL(pChild, NORMAL)) sbmesh->m_noffset = vertexOffset;
	if (GETURL(pChild, COLOR)) sbmesh->m_coffset = vertexOffset;
	if (GetURL(pChild, "TEXCOORD", TEXCOORD1)) sbmesh->m_toffset1 = vertexOffset;
}

void SceneDAEConstructor::ParseSources(	
					std::string& POSITION, 
					std::string& NORMAL,
					std::string& COLOR,
					std::string& TEXCOORD1,
					std::string& TEXCOORD2,
					const pugi::xml_node& pChild,
					Mesh* sbmesh)
{
	GETURLO(pChild, POSITION, sbmesh->m_voffset);
	GETURLO(pChild, NORMAL, sbmesh->m_noffset);
	GETURLO(pChild, COLOR, sbmesh->m_coffset);
	int set = 0;
	std::string TEXCOORD;
	int toffset = 0;
	if (GETURLOS(pChild, TEXCOORD, toffset, set))
	{
		if (set == 0)
		{
			TEXCOORD1 = TEXCOORD;
			sbmesh->m_toffset1 = toffset;
		}
		if (set == 1)
		{
			TEXCOORD2 = TEXCOORD;
			sbmesh->m_toffset2 = toffset;
		}
	}
}

struct SourcesURL
{
	std::string POSITION;
	std::string NORMAL;
	std::string COLOR;
	std::string TEXCOORD1;
	std::string TEXCOORD2;
};

struct MeshUnit
{
	SourcesURL urls;
	Mesh* sbmesh;
};

struct float_array
{
	int stride;
	std::vector<float> fa;
};

void SceneDAEConstructor::ReadMesh(const pugi::xml_node& mesh, const std::string& id)
{
	std::vector<MeshUnit> buffers;

	std::vector<pugi::xml_node> trianglesLists;

	for (pugi::xml_node node = mesh.first_child(); node; node = node.next_sibling())
	{
		std::string elementType = node.name();
		if (elementType == "polylist" || elementType == "triangles")
		{
			trianglesLists.push_back(node);
		}
	}
	
	for (std::vector<pugi::xml_node>::iterator it = trianglesLists.begin(); it != trianglesLists.end(); ++it)
	{
		MeshUnit unit;
		SourcesURL& urls = unit.urls;
		unit.sbmesh = new Mesh;
		unit.sbmesh->m_verticesMaps = new Mesh::VerticesMaps;
		Mesh::VerticesMaps* vmaps = unit.sbmesh->m_verticesMaps;

		std::string VERTEX;
		int vertexOffset;
		for (pugi::xml_node node = it->first_child(); node; node = node.next_sibling())
		{
			GETURLO(node, VERTEX, vertexOffset);
		}

		pugi::xml_node vertices;
		for (pugi::xml_node node = mesh.child("vertices"); node; node = node.next_sibling("vertices"))
		{
			if ((node.attribute("id") != NULL) && (strcmp(node.attribute("id").as_string(), VERTEX.c_str()) == 0))
			{
				vertices = node;
			}
		}

		for (pugi::xml_node node = vertices.first_child(); node; node = node.next_sibling())
		{
			ParseVertex(unit.urls.POSITION, unit.urls.NORMAL, unit.urls.COLOR, unit.urls.TEXCOORD1, unit.urls.TEXCOORD2, vertexOffset, node, unit.sbmesh);
		}

		for (pugi::xml_node node = it->first_child(); node; node = node.next_sibling())
		{
			ParseSources(unit.urls.POSITION, unit.urls.NORMAL, unit.urls.COLOR, unit.urls.TEXCOORD1, unit.urls.TEXCOORD2, node, unit.sbmesh);
		}

		pugi::xml_node nodePrimitives = it->child("p");

		{
			int count = it->attribute("count").as_int();
						
			Tokenizer tokens(nodePrimitives.child_value());
			const char* token = NULL;
			int tokensCount = 0;
			while ((token = tokens.GetNextToken()))
			{
				vmaps->m_indices.push_back(atoi(token));
				tokensCount++;
			}
			unit.sbmesh->m_stride = tokensCount / 3 / count;
		}

		buffers.push_back(unit);
	}
		
	std::map<std::string, float_array> sources;

	for (pugi::xml_node node = mesh.child("source"); node; node = node.next_sibling("source"))
	{
		if (node.attribute("id") == NULL)
			continue;
		float_array& floats = sources[node.attribute("id").as_string()];

		ReadFloatArray(floats.fa, floats.stride, node);
	}
		
	for (std::vector<MeshUnit>::iterator it = buffers.begin(); it != buffers.end(); ++it)
	{
		float_array* POSITION = &sources[it->urls.POSITION];
		float_array* NORMAL = &sources[it->urls.NORMAL];
		float_array* COLOR = &sources[it->urls.COLOR];
		float_array* TEXCOORD1 = &sources[it->urls.TEXCOORD1];
		float_array* TEXCOORD2 = &sources[it->urls.TEXCOORD2];
		
		Mesh::VerticesMaps* vmaps = it->sbmesh->m_verticesMaps;

		for (unsigned int i = 0; i < vmaps->m_indices.size();)
		{
			if(it->sbmesh->m_voffset != -1)
			{
				int index = vmaps->m_indices[i + it->sbmesh->m_voffset];
				float x = POSITION->fa[index * POSITION->stride + 0];
				float y = POSITION->fa[index * POSITION->stride + 1];
				float z = POSITION->fa[index * POSITION->stride + 2];
				vmaps->m_vertices[index] = glm::vec3(x, y, z);
			}
			if(it->sbmesh->m_noffset != -1)
			{
				int index = vmaps->m_indices[i + it->sbmesh->m_noffset];
				float x = NORMAL->fa[index * NORMAL->stride + 0];
				float y = NORMAL->fa[index * NORMAL->stride + 1];
				float z = NORMAL->fa[index * NORMAL->stride + 2];
				vmaps->m_normals[index] = glm::vec3(x, y, z);
			}
			if(it->sbmesh->m_coffset != -1)
			{
				int index = vmaps->m_indices[i + it->sbmesh->m_coffset];
				float x = COLOR->fa[index * COLOR->stride + 0];
				float y = x;
				float z = x;
				if (COLOR->stride > 1)
				{
					y = COLOR->fa[index * COLOR->stride + 1];
				}
				if (COLOR->stride > 2)
				{
					z = COLOR->fa[index * COLOR->stride + 2];
				}
				vmaps->m_color[index] = glm::vec3(x, y, z);
			}
			if(it->sbmesh->m_toffset1 != -1)
			{
				int index = vmaps->m_indices[i + it->sbmesh->m_toffset1];
				float u = TEXCOORD1->fa[index * TEXCOORD1->stride + 0];
				float v = TEXCOORD1->fa[index * TEXCOORD1->stride + 1];
				vmaps->m_textcoord1[index] = glm::vec2(u, v);
			}
			if(it->sbmesh->m_toffset2 != -1)
			{
				int index = vmaps->m_indices[i + it->sbmesh->m_toffset2];
				float u = TEXCOORD2->fa[index * TEXCOORD2->stride + 0];
				float v = TEXCOORD2->fa[index * TEXCOORD2->stride + 1];
				vmaps->m_textcoord2[index] = glm::vec2(u, v);
			}
			
			i += it->sbmesh->m_stride;
		}

		BufferConstructor sbc;
		sbc.ConvertMesh(it->sbmesh);

		m_meshes[id].push_back(it->sbmesh);
	}

}

void SceneDAEConstructor::ReadScene(const pugi::xml_node& root, Scene& scene)
{
	pugi::xml_node pElem;
	//scene url
	std::string sceneURL;
	{
		pElem = root.child("scene").child("instance_visual_scene");
		sceneURL = pElem.attribute("url").as_string();
		URLFILTER(sceneURL);
		LOGI("scene URL: %s", sceneURL.c_str());
	}

	// block: scene
	pugi::xml_node rootE;
	LOGI("Loading library_visual_scenes");
	{
		pElem = root.child("library_visual_scenes");
		for (pugi::xml_node node = pElem.child("visual_scene"); node; node = node.next_sibling("visual_scene"))
		{
			if (strcmp(node.attribute("id").as_string(), sceneURL.c_str()) == 0)
			{
				rootE = node;
				for (pugi::xml_node node = rootE.child("node"); node; node = node.next_sibling("node"))
				{
					AddChild(node, &scene, &scene);
				}
			}
		}
	}
}

template<int count>
void ReadVector(Tokenizer& tokenizer, float (&output)[count])
{
	for (int i = 0; i < count; ++i)
	{
		const char* tokenDefault = "0";
		const char* token = "0";
		if ((token = tokenizer.GetNextToken()))
		{
			tokenDefault = token;
		}
		output[i] = static_cast<float>(atof(tokenDefault));
	}
}

void SceneDAEConstructor::AddChild(const pugi::xml_node& nodeE, Node* parent, Scene* root)
{
	std::string name;
	if (nodeE.attribute("name") == NULL)
	{
		int count = parent->GetChildCount();
		char id[20];
		sprintf(id,"%d",count);
		name = parent->GetName() + "-" + id;
	}
	else
	{
		name = nodeE.attribute("name").as_string();
	}

	Node* node = new Node(name.c_str(), *root);
	parent->AddChild(*node);

	if (nodeE.child("instance_geometry"))
	{
		std::string url = nodeE.child("instance_geometry").attribute("url").as_string();
		URLFILTER(url);
		if(m_meshes.find(url) != m_meshes.end())
		{
			node->SetMeshs(m_meshes[url]);
		}
		else
		{
			printf("");
		}
	}

	glm::mat4 transform(1.0);
	glm::vec3 translate(0.0);
	glm::vec3 scale(1.0);
	glm::mat4 rotate(1.0);

	bool hasMatrix = false;

	if (nodeE.child("matrix"))
	{
		const pugi::xml_node& n = nodeE.child("matrix");
		{
			Tokenizer tokens(n.child_value());
			const char* token = NULL;
			int k = 0;
			while ((token = tokens.GetNextToken()) && k <16)
			{
				int j = k / 4;
				int i = k - j * 4;
				k++;
				transform[i][j] = static_cast<float>(atof(token));
			}
		}
		hasMatrix = true;
	}

	if (nodeE.child("translate"))
	{
		const pugi::xml_node& n = nodeE.child("translate");
		{
			float v[3];
			Tokenizer tokens(n.child_value());
			ReadVector(tokens, v);
			translate = glm::vec3(v[0], v[1], v[2]);
		}
	}

	if (nodeE.child("scale"))
	{
		const pugi::xml_node& n = nodeE.child("scale");
		{
			float v[3];
			Tokenizer tokens(n.child_value());
			ReadVector(tokens, v);
			scale = glm::vec3(v[0], v[1], v[2]);
		}
	}

	if (nodeE.child("rotate"))
	{
		const pugi::xml_node& n = nodeE.child("rotate");
		{
			float v[4];
			Tokenizer tokens(n.child_value());
			ReadVector(tokens, v);
			glm::vec3 axies = glm::vec3(v[0], v[1], v[2]);
			if (glm::length(axies) > glm::epsilon<float>())
			{
				rotate = glm::rotate(v[3] / 180.0f * glm::pi<float>(), axies);
			}
		}
	}

	if (!hasMatrix)
	{
		transform = glm::translate(translate) * rotate * glm::scale(scale);
	}

	node->SetLocalTransform(transform);

	for (pugi::xml_node child = nodeE.child("node"); child; child = child.next_sibling("node"))
	{
		AddChild(child, node, root);
	}
}