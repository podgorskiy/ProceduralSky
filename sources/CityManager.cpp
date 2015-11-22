#include "CityManager.h"

#include "SBShader/SBShader.h"
#include "SBScene/SBSceneDAEConstructor.h"
#include "SBScene/SBMeshBufferConstructor.h"
#include "SBScene/SBMesh.h"
#include "SBScene/SBNode.h"
#include "SBScene/SBSceneRenderer.h"
#include "SBScene/SBSceneSerializer.h"
#include "SBScene/SBSceneUtils.h" 
#include "SBFileSystem/SBCFile.h"
#include "SBCamera.h"
#include "SBOpenGLHeaders.h"
#include "SBAsyncDataLoad/SBRequestPull.h"
#include "SBAsyncDataLoad/SBRequestData.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <sstream>

void CityManager::Init()
{
	m_rpull.SetUrlPrefix("async_data/");
	m_rpull.SetCountOfSimultaneousRequests(5);
	//MakeCityBatches();
	MakeOcean();
	LoadCityBatches();
		
	SB::RequestDataPtr r = m_rpull.CreateRequest<SB::RequestData>("ocean_batch.bdae");
	r->SetCallback([this](const SB::MemoryFile& memfile)
	{
		LoadOcean(&memfile);
	});
}

void CityManager::SetShader(SB::Shader* shader)
{
	m_terrainShader = shader;
}

void CityManager::ConstructCityFromDAE()
{
	SB::Node* city = new SB::Node;

	const char* cityParts[] = 
	{
		"casino_backstreet.dae",
		"strip01.dae",
		"rich_residential.dae",
		"airport.dae",
		"construction.dae",
		"desert00.dae",
		"desert01.dae",
		"desert02.dae",
		"desert03.dae",
		"downtown.dae",
		"government.dae",
		"industrial.dae",
		"poor_residential.dae",
		"stadium.dae",
		"strip00.dae",
		"strip02.dae",
		"strip03.dae"
	};


	for (int i = 0; i < sizeof(cityParts) / sizeof(cityParts[0]); i++)
	{
		SB::SceneDAEConstructor sc;
		sc.OpenDAE((std::string("city_source/") + cityParts[i]).c_str());
		SB::Node* scene = sc.ConstructSBScene();
		SB::Utils::RemoveNodes(scene, "additif_*");
		SB::Utils::RemoveNodes(scene, "bush_*");
		SB::Utils::RemoveNodes(scene, "tree_*");
		SB::Utils::RemoveNodes(scene, "cactus_*");
		SB::Utils::RemoveNodes(scene, "*__b0_1*");
		SB::Utils::RemoveNodes(scene, "*__p0_1*");
		SB::Utils::RemoveNodes(scene, "*__t0_1*");
		SB::Utils::RemoveNodes(scene, "road_lines_*");
		SB::Utils::Merge(city, scene);
	}

	{
		SB::CFile file;
		SB::Serializer serializer;
		file.Open("city_source/city.bdae", SB::IFile::FILE_WRITE);
		serializer.SerializeScene(city, &file);
	}
}

void CityManager::MakeCityBatches()
{
	SB::CFile file;
	SB::Serializer serializer;
	file.Open("city_source/city.bdae", SB::IFile::FILE_READ);
	SB::Node* city = serializer.DeSerializeScene(&file);

	std::vector<SB::Mesh*> meshList;
	SB::Utils::MakeFlat(meshList, city);

	std::vector<SB::Mesh*> batchedMeshList;
	SB::Utils::BatchMeshes(meshList, batchedMeshList);

	pugi::xml_document doc;

	pugi::xml_node rootNode = doc.append_child();
	rootNode.set_name("root");

	int id = 0;
	for (std::vector<SB::Mesh*>::iterator it = batchedMeshList.begin(); it != batchedMeshList.end(); ++it, ++id)
	{
		SB::CFile fileBatch; 
		std::ostringstream name;
		name << "city_part_" << id << ".bdae";
		std::string filename = "async_data/";
		filename += name.str();
		fileBatch.Open(filename, SB::IFile::FILE_WRITE);
		SB::Serializer se;
		se.SerializeMesh(*it, &fileBatch);
		
		pugi::xml_node partNode = rootNode.append_child();
		partNode.set_name("cityPart");
		partNode.append_attribute("file") = name.str().c_str();
	}

	std::ostringstream cityDescription;
	doc.print(cityDescription);
	SB::CFile fileCityDescription;
	fileCityDescription.Open("data/cityDescription.xml", SB::IFile::FILE_WRITE);
	fileCityDescription.Write(cityDescription.str().c_str(), cityDescription.str().size());
}

void CityManager::LoadCityBatches()
{
	SB::CFile fileCityDescription;
	fileCityDescription.Open("data/cityDescription.xml", SB::IFile::FILE_READ);

	std::vector<char> buffer;
	buffer.resize(static_cast<unsigned int>(fileCityDescription.GetSize()));
	fileCityDescription.Read(&buffer[0], fileCityDescription.GetSize());

	pugi::xml_document doc;
	doc.load_buffer(buffer.data(), buffer.size());
	pugi::xml_node root = doc.first_child();

	for (pugi::xml_node part = root.child("cityPart"); part; part = part.next_sibling("cityPart"))
	{
		std::string filename = part.attribute("file").as_string();
		SB::RequestDataPtr r = m_rpull.CreateRequest<SB::RequestData>(filename);
		r->SetCallback([this](const SB::MemoryFile& memfile)
		{
			PushCityBatch(&memfile);
		});
	}
}

void CityManager::MakeOcean()
{
	SB::SceneDAEConstructor sc;
	sc.OpenDAE("city_source/ocean.dae");
	SB::Node* scene = sc.ConstructSBScene();

	std::vector<SB::Mesh*> meshList;
	SB::Utils::MakeFlat(meshList, scene);

	std::vector<SB::Mesh*> batchedMeshList;
	SB::Utils::BatchMeshes(meshList, batchedMeshList);

	SB::CFile fileBatch;
	fileBatch.Open("async_data/ocean_batch.bdae", SB::IFile::FILE_WRITE);
	SB::Serializer se;
	se.SerializeBatchList(batchedMeshList, &fileBatch);
}

void CityManager::LoadOcean(const SB::IFile* file)
{
	SB::Serializer se;
	std::vector<SB::Mesh*> batchedMeshList;
	se.DeSerializeBatchList(batchedMeshList, file);

	SB::Utils::PushMeshDataToVideoMemory(batchedMeshList);

	for (int i = 0; i < batchedMeshList.size(); ++i)
	{
		glm::mat4 transform(1.0f);
		m_renderlistOcean.push_back(SB::SceneRenderer::Entity(transform, batchedMeshList[i]));
	}
}

void CityManager::PushCityBatch(const SB::IFile* file)
{
	SB::Serializer se;
	SB::Mesh* mesh = se.DeSerializeMesh(file);
	
	if (mesh != nullptr)
	{
		mesh->CreateVBO();
		glm::mat4 transform(1.0f);
		m_renderlistCity.push_back(SB::SceneRenderer::Entity(transform, mesh));
	}
}

void CityManager::Draw(SB::Camera* camera)
{
	m_sceneRenderer->Render(m_renderlistCity, camera, m_terrainShader);
	m_sceneRenderer->Render(m_renderlistOcean, camera, m_terrainShader);
	m_rpull.Update();
}