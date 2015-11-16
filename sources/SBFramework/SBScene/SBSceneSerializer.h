#pragma once
#include <map>

namespace SB
{
	class IFile;
	class Scene;
	class Node;
	class Mesh;

	class SceneSerializer
	{
	public:
		bool Serialize(const Node* scene, IFile* file);
		Scene* DeSerialize(IFile* file);
	private:
		typedef int MeshID;

		MeshID PushMesh(const Mesh* mesh);
		void WriteNode(const Node* node, IFile* file);
		void WriteMesh(const Mesh* mesh, MeshID id, IFile* file);

		void ReadNode(Node* node, IFile* file);
		MeshID ReadMesh(Mesh* mesh, IFile* file);

		std::map<MeshID, const Mesh*> m_meshs;
		MeshID m_meshID;
		std::map<MeshID, Node*> m_nodesMeshBindings;
	};
}