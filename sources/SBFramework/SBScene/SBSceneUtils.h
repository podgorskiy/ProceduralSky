#pragma once
#include <vector>

namespace SB
{
	class Node;
	class Mesh;

	class Utils
	{
	public:
		static void PushMeshDataToVideoMemory(Node* scene, bool recursive);

		static void PushMeshDataToVideoMemory(const std::vector<SB::Mesh*>& input);

		static void Merge(Node* sceneBase, Node* sceneMerge);

		static void RemoveNodes(Node* node, const char* pattern);
		
		static void MakeFlat(std::vector<SB::Mesh*>& meshArray, SB::Node* scene);

		static void BatchMeshes(const std::vector<SB::Mesh*>& input, std::vector<SB::Mesh*>& output);

	private:
		static void ProcessMeshBank(const std::vector<SB::Mesh*>& input, std::vector<SB::Mesh*>& output);

		static void AppendMesh(SB::Mesh* base, SB::Mesh* other);

		static int GetIndex(SB::Mesh* mesh, int index);

		static long long GetHashFromMesh(SB::Mesh* mesh);
	};
}