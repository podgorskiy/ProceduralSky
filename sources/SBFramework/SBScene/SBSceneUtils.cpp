#include "SBSceneUtils.h" 

#include "SBScene.h"
#include "SBMesh.h"

#include <cctype>

void SB::PushMeshDataToVideoMemory(Node* scene, bool recursive)
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

void SB::Merge(SB::Scene* sceneBase, SB::Scene* sceneMerge)
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


void SB::RemoveNodes(Node* node, const char* pattern)
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