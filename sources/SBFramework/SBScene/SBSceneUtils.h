#pragma once

namespace SB
{
	class Node;
	class Scene;

	void PushMeshDataToVideoMemory(Node* scene, bool recursive);

	void Merge(Scene* sceneBase, Scene* sceneMerge);

	void RemoveNodes(Node* node, const char* pattern);
}