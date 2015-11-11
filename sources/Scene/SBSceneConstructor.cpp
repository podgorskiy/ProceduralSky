#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <functional>

#include "SBCommon.h"
#include "SBNode.h"
#include "SBScene.h"
#include "SBMesh.h"

#include "SBSceneConstructor.h"

struct IsN {
	bool operator() (const char& x) const { return x == '#'; }
	typedef int argument_type;
};

template<typename T>
class pushHelper
{
public:
	void pushData(std::vector<T>& vector, int& s, const char* val, float& x, float& y, float&z);
};

template<>
class pushHelper<glm::vec3>
{
public:
	void pushData(std::vector<glm::vec3>& vector, int& s, const char* val, float& x, float& y, float&z)
	{
		switch (s)
		{
		case 0:x = atof(val); s++; break;
		case 1:y = atof(val); s++; break;
		case 2:
		{
			z = atof(val);
			s = 0;
			vector.push_back(glm::vec3(x, y, z));
			break;
		}
		}
	}
};

template<>
class pushHelper<glm::vec2>
{
public:
	void pushData(std::vector<glm::vec2>& vector, int& s, const char* val, float& x, float& y, float&z)
	{
		switch (s)
		{
		case 0:x = atof(val); s++; break;
		case 1:y = atof(val);
		{
			s = 0;
			vector.push_back(glm::vec2(x, y));
			break;
		}
		}
	}
};

template<typename T>
void SBSceneConstructor::ReadDataArray(std::vector<T>& data, TiXmlElement* source)
{
	TiXmlElement* varray = source->FirstChildElement("float_array");
	TiXmlNode* node = varray->FirstChild();
	std::string val = node->Value();
	{
		int count = atoi(varray->Attribute("count"));
		std::vector<std::string> tokens;
		GetTokens(val, tokens, count * 3);
		data.clear();
		data.reserve(count);
		int s = 0;
		float x, y, z;
		for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
		{
			pushHelper<T> ph;
			ph.pushData(data, s, it->c_str(), x, y, z);
		}
	}
}

#define FOREACH(X) for (TiXmlElement* pChild = X; pChild != 0; pChild = pChild->NextSiblingElement())

void GetTokens(const std::string& X, std::vector<std::string>& tokens, int count)
{
	std::istringstream iss(X);
	tokens.reserve((count));
	copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter<std::vector<std::string> >(tokens));
}

void URLFILTER(std::string& x)
{
	x.erase(
		x.begin(), 
		std::find_if(x.begin(), x.end(), std::not1(IsN()) )
	);
}

void GetURL(TiXmlElement* node_, const char* semantic, std::string& URL)
{
	TiXmlElement* node = node_;
	const char* v = node->Attribute("semantic");
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node->Attribute("source");
			URLFILTER(URL);
		}
	}
}
#define GETURL(node, semantic) GetURL(node, #semantic, semantic);

void GetURLoffset(TiXmlElement* node, const char* semantic, std::string& URL, int& offset)
{
	const char* v = node->Attribute("semantic");
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node->Attribute("source");
			URLFILTER(URL);
			if (node->Attribute("offset") != NULL)
			{
				offset = atoi(node->Attribute("offset"));
			}
		}
	}
}
#define GETURLO(node, semantic, offset) GetURLoffset(node, #semantic, semantic, offset);

bool GetURLoffsetSet(TiXmlElement* node, const char* semantic, std::string& URL, int& offset, int& set)
{
	const char* v = node->Attribute("semantic");
	if (v != NULL)
	{
		if (strcmp(v, semantic) == 0)
		{
			URL = node->Attribute("source");
			URLFILTER(URL);
			if (node->Attribute("offset") != NULL)
			{
				offset = atoi(node->Attribute("offset"));
			}
			if (node->Attribute("set") != NULL)
			{
				set = atoi(node->Attribute("set")); 
			}
			return true;
		}
	}
	return false;
}
#define GETURLOS(node, semantic, offset, set) GetURLoffsetSet(node, #semantic, semantic, offset, set)

SBSceneConstructor::SBSceneConstructor() :m_daeBuffer(NULL)
{
};

SBSceneConstructor::~SBSceneConstructor()
{
};

bool SBSceneConstructor::OpenDAE(const char* filename)
{
	LOGI("Reading COLLADA DAE file: %s", filename);
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	m_daeBuffer.resize(size);
	if (file.read(m_daeBuffer.data(), size))
	{
		return true;
	}
	else
	{
		return false;
	}
};

void SBSceneConstructor::PrintDebugOutput(SBNode& node, int level, std::vector<bool> last)
{
	for (int i = 0; i < node.GetChildCount(); i++)
	{
		SBNode* child = node.GetChild(i);
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

SBScene* SBSceneConstructor::ConstructSBScene()
{	
	TiXmlDocument * dae = new TiXmlDocument;

	dae->Parse(m_daeBuffer.data(), 0, TIXML_ENCODING_UTF8);
	
	TiXmlHandle hDoc(dae);
	TiXmlElement* pElem;
	TiXmlNode* node;
	TiXmlHandle hRoot(0);

	// block: name
	{
		pElem = hDoc.FirstChildElement().Element();
		// should always have a valid root but handle gracefully if it does
		if (!pElem)
		{
			LOGE("No root node in dae document!");
			return 0;
		}
		LOGI("Doc type: %s", pElem->Value());

		// save this for later
		hRoot = TiXmlHandle(pElem);
	}

	// block: asset
	{
		node = hRoot.FirstChild("asset").FirstChild("contributor").FirstChild("author").FirstChild().Node();
		if (node != NULL)
		{
			LOGI("author: %s", node->Value());
		}
	}

	ReadGeometry(hRoot);

	SBScene& root = *new SBScene("root");

	glm::mat4 ind = glm::mat4();
	root.SetLocalTransform(ind);

	ReadScene(hRoot, root);
	
	//print tree
	LOGI("root");
	PrintDebugOutput(root);

	return &root;
}

void SBSceneConstructor::ReadGeometry(TiXmlHandle& hRoot)
{
	TiXmlElement* pElem;

	// block: geometry
	LOGI("loading: library_geometries");
	{
		pElem = hRoot.FirstChild("library_geometries").Element();
		FOREACH(pElem->FirstChildElement())
		{
			if (strcmp(pChild->Value(), "geometry") != 0) continue;
			std::string id = pChild->Attribute("id");
			LOGI("geometry: %s", id.c_str());

			TiXmlElement* mesh = pChild->FirstChildElement("mesh");
			TiXmlElement* spline = pChild->FirstChildElement("spline");
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

void SBSceneConstructor::CreateBB(std::vector<glm::vec3>& v)
{

}

void SBSceneConstructor::ReadSpline(TiXmlElement* spline, std::string id)
{
	LOGI("Read spline");

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> intangent;
	std::vector<glm::vec3> outtangent;

	int trianglesCount = 0;
	{
		std::string splineType = spline->Attribute("closed");

		bool closed = splineType == "1" ? true : false;

		TiXmlElement* controlVertices = spline->FirstChildElement("control_vertices");
		if (controlVertices == NULL)
		{
			return;
		}

		std::string POSITION;
		std::string IN_TANGENT;
		std::string OUT_TANGENT;
		std::string INTERPOLATION;

		FOREACH(controlVertices->FirstChildElement())
		{
			GETURL(pChild, POSITION);
			GETURL(pChild, IN_TANGENT);
			GETURL(pChild, OUT_TANGENT);
			GETURL(pChild, INTERPOLATION);
		}

		TiXmlElement* positionsE = NULL;
		TiXmlElement* intangentE = NULL;
		TiXmlElement* outtangentE = NULL;
		TiXmlElement* interpolationE = NULL;

		FOREACH(spline->FirstChildElement())
		{
			if (pChild->Attribute("id") == NULL)
				continue;
			if (strcmp(pChild->Attribute("id"), POSITION.c_str()) == 0)
			{
				positionsE = pChild;
			}
			if (strcmp(pChild->Attribute("id"), IN_TANGENT.c_str()) == 0)
			{
				intangentE = pChild;
			}
			if (strcmp(pChild->Attribute("id"), OUT_TANGENT.c_str()) == 0)
			{
				outtangentE = pChild;
			}
		}

		if (positionsE != NULL)
		{
			ReadDataArray(vertices, positionsE);
		}

		if (intangentE != NULL)
		{
			ReadDataArray(intangent, intangentE);
		}

		if (outtangentE != NULL)
		{
			ReadDataArray(outtangent, outtangentE);
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


void SBSceneConstructor::ReadMesh(TiXmlElement* mesh, std::string id)
{
	TiXmlNode* node;

	std::string POSITION;
	std::string VERTEX;
	std::string NORMAL;
	std::string COLOR;
	std::string TEXCOORD1;
	std::string TEXCOORD2;
	bool VERTEX_;
	bool NORMAL_;
	bool COLOR_;
	bool TEXCOORD1_;
	bool TEXCOORD2_;
	int trianglesCount = 0;

	SBMesh* sbmesh = new SBMesh;

	{
		TiXmlElement* triangles = mesh->FirstChildElement("polylist");
		if (triangles == NULL)
		{
			return;
		}
		node = triangles->FirstChildElement("p")->FirstChild();
		if (node == NULL)
		{
			return;
		}
		FOREACH(triangles->FirstChildElement())
		{
			GETURLO(pChild, VERTEX, sbmesh->m_voffset);
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

		VERTEX_ = VERTEX.size() > 0;
		NORMAL_ = NORMAL.size() > 0;
		COLOR_ = COLOR.size() > 0;
		TEXCOORD1_ = TEXCOORD1.size() > 0;
		TEXCOORD2_ = TEXCOORD2.size() > 0;
		if (VERTEX_)
			sbmesh->m_stride++;
		if (NORMAL_)
			sbmesh->m_stride++;
		if (COLOR_)
			sbmesh->m_stride++;
		if (TEXCOORD1_)
			sbmesh->m_stride++;
		if (TEXCOORD2_)
			sbmesh->m_stride++;
		std::string val = node->Value();
		{
			int count = atoi(triangles->Attribute("count"));

			std::vector<std::string> tokens;
			GetTokens(val, tokens, 3 * count);
			sbmesh->m_indices.reserve(count);
			int s = 0;
			for (int i = 0; i < 3 * count; i++)
			{
				for (int k = 0; k < sbmesh->m_stride; k++)
				{
					sbmesh->m_indices.push_back(atoi(tokens[sbmesh->m_stride * i + k].c_str()));
				}
			}
		}
	}

	TiXmlElement* positionsE = NULL;
	TiXmlElement* normalsE = NULL;
	TiXmlElement* textcoordE1 = NULL;
	TiXmlElement* textcoordE2 = NULL;
	FOREACH(mesh->FirstChildElement())
	{
		if (pChild->Attribute("id") == NULL)
			continue;
		if (strcmp(pChild->Attribute("id"), VERTEX.c_str()) == 0)
		{
			GETURL(pChild->FirstChildElement("input"), POSITION);
		}
	}

	FOREACH(mesh->FirstChildElement())
	{
		if (pChild->Attribute("id") == NULL)
			continue;
		if (strcmp(pChild->Attribute("id"), POSITION.c_str()) == 0)
		{
			positionsE = pChild;
		}
		if (strcmp(pChild->Attribute("id"), NORMAL.c_str()) == 0)
		{
			normalsE = pChild;
		}
		if (strcmp(pChild->Attribute("id"), TEXCOORD1.c_str()) == 0)
		{
			textcoordE1 = pChild;
		}
		if (strcmp(pChild->Attribute("id"), TEXCOORD2.c_str()) == 0)
		{
			textcoordE2 = pChild;
		}
	}

	if (positionsE != NULL)
	{
		ReadDataArray(sbmesh->m_vertices, positionsE);
	}

	if (normalsE != NULL)
	{
		ReadDataArray(sbmesh->m_normals, normalsE);
	}

	if (textcoordE1 != NULL)
	{
		ReadDataArray(sbmesh->m_textcoord1, textcoordE1);
	}
	if (textcoordE2 != NULL)
	{
		ReadDataArray(sbmesh->m_textcoord2, textcoordE2);
	}

	m_meshes[id] = sbmesh;
}

void SBSceneConstructor::ReadScene(TiXmlHandle& hRoot, SBScene& root)
{
	TiXmlElement* pElem;
	//scene url
	std::string sceneURL;
	{
		pElem = hRoot.FirstChild("scene").FirstChild("instance_visual_scene").Element();
		sceneURL = pElem->Attribute("url");
		URLFILTER(sceneURL);
		LOGI("scene URL: %s", sceneURL.c_str());
	}

	// block: scene
	TiXmlElement* rootE;
	LOGI("Loading library_visual_scenes");
	{
		pElem = hRoot.FirstChild("library_visual_scenes").Element();
		FOREACH(pElem->FirstChildElement())
		{
			if (strcmp(pChild->Attribute("id"), sceneURL.c_str()) == 0)
			{
				rootE = pChild;
				FOREACH(rootE->FirstChildElement())
				{
					AddChild(pChild, &root, &root);
				}
			}
		}
	}
}


void SBSceneConstructor::AddChild(TiXmlElement* nodeE, SBNode* parent, SBScene* root)
{
	if (strcmp(nodeE->Value(), "node") != 0)
	{
		return;
	}
	std::string name;
	if (nodeE->Attribute("name") == NULL)
	{
		int count = parent->GetChildCount();
		char id[20];
		itoa(count, id, 20);
		name = std::string(parent->GetName().c_str()) + "-" + id;
	}
	else
	{
		name = nodeE->Attribute("name");
	}

	SBNode* node = new SBNode(name.c_str(), *root);
	parent->AddChild(*node);

	if (nodeE->FirstChildElement("instance_geometry"))
	{
		std::string url = nodeE->FirstChildElement("instance_geometry")->Attribute("url");
		URLFILTER(url);
		node->SetMesh(m_meshes[url]);
	}

	if (nodeE->FirstChildElement("matrix"))
	{
		glm::mat4 m;
		TiXmlNode* n = nodeE->FirstChildElement("matrix")->FirstChild();
		std::string val = n->Value();
		{
			std::vector<std::string> tokens;
			GetTokens(val, tokens, 16);
			int k = 0;
			for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			{
				int j = k / 4;
				int i = k - j * 4;
				k++;
				m[i][j] = atof(it->c_str());
			}
		}
		node->SetLocalTransform(m);
	}
	
	FOREACH(nodeE->FirstChildElement())
	{
		AddChild(pChild, node, root);
	}
}