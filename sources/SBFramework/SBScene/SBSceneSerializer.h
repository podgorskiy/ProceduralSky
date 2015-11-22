#pragma once
#include <map>
#include <vector>

namespace SB
{
	class IFile;
	class Node;
	class Mesh;

	class Serializer
	{
	public:
		bool SerializeScene(const Node* scene, IFile* file);
		Node* DeSerializeScene(const IFile* file);

		bool SerializeBatchList(const std::vector<SB::Mesh*>, IFile* file);
		void DeSerializeBatchList(std::vector<SB::Mesh*>& batchList, const IFile* file);

		bool SerializeMesh(const SB::Mesh*, IFile* file);
		SB::Mesh* DeSerializeMesh(const IFile* file);
	private:
		typedef int MeshID;

		MeshID PushMesh(const Mesh* mesh);
		void WriteNode(const Node* node, IFile* file);
		void WriteMesh(const Mesh* mesh, MeshID id, IFile* file);

		void ReadNode(Node* node, const IFile* file);
		MeshID ReadMesh(Mesh* mesh, const IFile* file);

		std::map<MeshID, const Mesh*> m_meshs;
		MeshID m_meshID;
		std::map<MeshID, Node*> m_nodesMeshBindings;
	};
}