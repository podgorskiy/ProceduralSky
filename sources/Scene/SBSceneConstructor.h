#pragma once
#include <glm/matrix.hpp>
#include "../aabb.h"
#include <tinyxml.h>
#include <map>

class SBNode;
class SBScene;
class SBMesh;

///A fabric that constructs sbscenes
class SBSceneConstructor
{
public:
		
	SBSceneConstructor();
	virtual ~SBSceneConstructor();

	virtual bool OpenDAE(const char* filename);
	virtual void PrintDebugOutput(SBNode& node, int level = 0, std::vector<bool> last = std::vector<bool>());
	virtual SBScene* ConstructSBScene();
	
private:

	template<typename T>
	void ReadDataArray(std::vector<T>& data, TiXmlElement* source);

	void ReadScene(TiXmlHandle& hRoot, SBScene& root);
	void ReadGeometry(TiXmlHandle& hRoot);
	void ReadMesh(TiXmlElement* mesh, std::string id);
	void ReadSpline(TiXmlElement* spline, std::string id);
	void AddChild(TiXmlElement* nodeE, SBNode* parent, SBScene* root);
	void CreateBB(std::vector<glm::vec3>& v);

	std::map<std::string, SBMesh*> m_meshes;
	
	std::vector<char> m_daeBuffer;
};
