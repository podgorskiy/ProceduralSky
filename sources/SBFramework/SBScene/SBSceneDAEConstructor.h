#pragma once
#include "SBaabb.h"

#include <glm/matrix.hpp>
#include <pugixml.hpp>
#include <map>
#include <vector>

namespace SB
{
	class Node;
	class Mesh;

	class SceneDAEConstructor
	{
	public:

		SceneDAEConstructor();
		~SceneDAEConstructor();

		bool OpenDAE(const char* filename);
		void PrintDebugOutput(Node& node, int level = 0, std::vector<bool> last = std::vector<bool>());
		Node* ConstructSBScene();

	private:
		void ReadScene(const pugi::xml_node& hRoot, Node& root);
		void ReadGeometry(const pugi::xml_node& hRoot);
		void ReadMesh(const pugi::xml_node& mesh, const std::string& id);
		void ReadSpline(const pugi::xml_node& spline, const std::string& id);
		void AddChild(const pugi::xml_node& nodeE, Node* parent, Node* root);
		void CreateBB(std::vector<glm::vec3>& v);
		
		void ParseVertex(std::string& POSITION,
			std::string& NORMAL,
			std::string& COLOR,
			std::string& TEXCOORD1,
			std::string& TEXCOORD2,
			int vertexOffset,
			const pugi::xml_node& pChild,
			Mesh* sbmesh);

		void ParseSources(std::string& POSITION,
			std::string& NORMAL,
			std::string& COLOR,
			std::string& TEXCOORD1,
			std::string& TEXCOORD2,
			const pugi::xml_node& pChild,
			Mesh* sbmesh);

		std::map<std::string, std::vector<Mesh*> > m_meshes;

		std::vector<char> m_daeBuffer;
	};
}