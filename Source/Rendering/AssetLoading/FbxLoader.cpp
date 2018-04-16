#include "FbxLoader.h"
#include <fbxsdk.h>
#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../MeshManagement/Mesh.h"

#include "../Animation/Skeleton.h"

#include <map>

namespace
{
	FbxMesh* GetMesh(FbxNode* node)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		FbxMesh* mesh = node->GetMesh();
		if (mesh != nullptr && mesh->GetPolygonVertexCount() > 0)
		{
			return mesh;
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (node->GetChild(i) == nullptr)
			{
				continue;
			}

			FbxMesh* mesh = node->GetChild(i)->GetMesh();
			if (mesh == nullptr || mesh->GetPolygonVertexCount() <= 0)
			{
				if (node->GetChild(i)->GetChildCount() == 0)
				{
					return nullptr;
				}
				else
				{
					for (int j = 0; j < node->GetChild(i)->GetChildCount(); ++j)
					{
						mesh = GetMesh(node->GetChild(i)->GetChild(j));
						if (mesh != nullptr && mesh->GetPolygonVertexCount() > 0)
						{
							std::cout << "mesh found at indexes i: " << i << " j: " << j << std::endl;
							return mesh;
						}
					}
				}
			}
			else
			{
				std::cout << "mesh found at index i: " << i << std::endl;
				std::cout << "mesh polygon count: " << mesh->GetPolygonCount() << std::endl;
				return mesh;
			}
		}

		return nullptr;
	}

	FbxMesh* GetMeshes(FbxNode* node, std::vector<FbxMesh*>& meshes)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		FbxMesh* mesh = node->GetMesh();
		if (mesh != nullptr)
		{
			meshes.push_back(mesh);
			mesh = nullptr;
			//return mesh;
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			if (node->GetChild(i) == nullptr)
			{
				continue;
			}

			FbxMesh* mesh = node->GetChild(i)->GetMesh();
			if (mesh == nullptr || mesh->GetPolygonVertexCount() <= 0)
			{
				if (node->GetChild(i)->GetChildCount() == 0)
				{
					return nullptr;
				}
				else
				{
					for (int j = 0; j < node->GetChild(i)->GetChildCount(); ++j)
					{
						mesh = GetMeshes(node->GetChild(i)->GetChild(j), meshes);
						if (mesh != nullptr && mesh->GetPolygonVertexCount() > 0)
						{
							std::cout << "mesh found at indexes i: " << i << " j: " << j << std::endl;
							meshes.push_back(mesh);
							mesh = nullptr;
							//return mesh;
						}
					}
				}
			}
			else
			{
				std::cout << "mesh found at index i: " << i << std::endl;
				std::cout << "mesh polygon count: " << mesh->GetPolygonCount() << std::endl;
				//return mesh;
				meshes.push_back(mesh);
				mesh = nullptr;
			}
		}

		return nullptr;
	}

	FbxNode* FindRoot(FbxNode* node)
	{
		if (node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			return nullptr;
		}

		FbxNode* parent = node->GetParent();
		if (parent != nullptr && parent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			return node;
		}

		return FindRoot(node->GetParent());
	}
}

void PrintAnimationStackInfo(FbxImporter* importer)
{
	if (importer->IsFBX())
	{
		std::cout << "model is an fbx" << std::endl;

		FBXSDK_printf("Animation Stack Information\n");

		int lAnimStackCount = importer->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);

		for (int i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = importer->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}
	}
}

void ValidateModelLoad(FbxScene* scene, FbxManager* manager, FbxImporter* importer)
{
	if (!importer->Import(scene))
	{
		std::cout << "import unsuccessful" << std::endl;
	}
	importer->Destroy();

	FbxAxisSystem fbxAxis = scene->GetGlobalSettings().GetAxisSystem();
	if (fbxAxis != FbxAxisSystem::OpenGL)
	{
		std::cout << "fbxaxis not opengl" << std::endl;
	}
}

void ImportFileToScene(std::string assetLocation, const char* assetName, FbxScene* scene, FbxManager* manager)
{
	const char* fileName = assetLocation.append(assetName).c_str();

	FbxIOSettings *ioSettings = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ioSettings);

	FbxImporter *importer = FbxImporter::Create(manager, "");
	if (!importer->Initialize(fileName, -1, manager->GetIOSettings()))
	{
		std::cout << "Initialize unsuccessful" << std::endl;
	}

	PrintAnimationStackInfo(importer);
	ValidateModelLoad(scene, manager, importer);
}

void ExtractVertexInfoFromMesh(FbxMesh* fbxMesh, std::vector<GLfloat>& vertexInfo)
{
	int vertexCount = 0;
	vertexCount = fbxMesh->GetPolygonVertexCount();

	int triangleCount = fbxMesh->GetPolygonVertexCount() / 3;
	int indicesCount = fbxMesh->GetPolygonVertexCount();

	FbxVector4* fbxVerts = new FbxVector4[vertexCount];
	int arrayIndex = 0;
	memcpy(fbxVerts, fbxMesh->GetControlPoints(), vertexCount * sizeof(FbxVector4));

	float wValue = 1.f;
	vertexInfo.resize(triangleCount * 9);

	int highestIndex = 0;

	for (int j = 0; j < triangleCount; ++j)
	{
		int index = fbxMesh->GetPolygonVertex(j, 0);
		//fbxMesh->getpol

		if (highestIndex < index)
		{
			highestIndex = index;
		}

		int vertIndex = j * 9;

		vertexInfo[vertIndex] = (GLfloat)fbxVerts[index][0];
		vertexInfo[vertIndex + 1] = (GLfloat)fbxVerts[index][1];
		vertexInfo[vertIndex + 2] = (GLfloat)fbxVerts[index][2];

		/*for (int i = 0; i < 4; ++i)
		{
		jointIndices.push_back(weightsMap[index][i]._jointIndex);
		jointWeights.push_back(weightsMap[index][i]._jointWeight);
		}*/

		index = fbxMesh->GetPolygonVertex(j, 1);
		vertexInfo[vertIndex + 3] = (GLfloat)fbxVerts[index][0];
		vertexInfo[vertIndex + 4] = (GLfloat)fbxVerts[index][1];
		vertexInfo[vertIndex + 5] = (GLfloat)fbxVerts[index][2];

		if (highestIndex < index)
		{
			highestIndex = index;
		}

		/*for (int i = 0; i < 4; ++i)
		{
		jointIndices.push_back(weightsMap[index][i]._jointIndex);
		jointWeights.push_back(weightsMap[index][i]._jointWeight);
		}*/

		index = fbxMesh->GetPolygonVertex(j, 2);
		vertexInfo[vertIndex + 6] = (GLfloat)fbxVerts[index][0];
		vertexInfo[vertIndex + 7] = (GLfloat)fbxVerts[index][1];
		vertexInfo[vertIndex + 8] = (GLfloat)fbxVerts[index][2];

		if (highestIndex < index)
		{
			highestIndex = index;
		}

		/*for (int i = 0; i < 4; ++i)
		{
		jointIndices.push_back(weightsMap[index][i]._jointIndex);
		jointWeights.push_back(weightsMap[index][i]._jointWeight);
		}*/
	}

	std::cout << "highest: " << highestIndex << std::endl;
}

void PrintFbxMeshes(FbxNode* rootNode)
{
	std::vector<FbxMesh*> fbxMeshes = std::vector<FbxMesh*>();
	GetMeshes(rootNode, fbxMeshes);
	std::cout << "GetMeshes count is: " << fbxMeshes.size() << std::endl;
}

FbxNode* GetRootNodeFromFile(std::string assetLocation, const char* assetName)
{
	FbxManager *manager = FbxManager::Create();
	FbxScene *scene = FbxScene::Create(manager, "tempName");
	ImportFileToScene(assetLocation, assetName, scene, manager);

	return scene->GetRootNode();
}

void GetMeshesFromRootNode(FbxNode* rootNode)
{
	std::vector<FbxMesh*> meshes = std::vector<FbxMesh*>();
	GetMeshes(rootNode, meshes);
}

void ExtractUVInfoFromMesh(FbxMesh* fbxMesh, std::vector<GLfloat>& fbxUVs)
{
	// Get the name of each set of UV coords
	FbxStringList UVSetNameList;
	fbxMesh->GetUVSetNames(UVSetNameList);

	FbxArray<FbxVector2> uvtest;
	fbxMesh->GetPolygonVertexUVs(UVSetNameList.GetStringAt(0), uvtest);
	for (int h = 0; h < uvtest.GetCount(); h++)
	{
		fbxUVs.push_back(uvtest[h].mData[0]);
		fbxUVs.push_back(1 - uvtest[h].mData[1]);
	}
}

void ExtractAnimationInfoFromMesh(FbxMesh* fbxMesh)
{

}

Mesh* FbxLoader::LoadFbx(std::string assetLocation, const char* assetName)
{
	FbxNode* rootNode = GetRootNodeFromFile(assetLocation, assetName);
	FbxMesh* fbxMesh = GetMesh(rootNode);
	if (fbxMesh == nullptr)
	{
		std::cout << "FbxMesh not found" << std::endl;
		return nullptr;
	}

	std::vector<GLfloat> fbxUVs = std::vector<GLfloat>();//std::vector<GLfloat>(vertexCount * 6);
	ExtractUVInfoFromMesh(fbxMesh, fbxUVs);
	
	std::vector<GLfloat> vertexInfo;
	ExtractVertexInfoFromMesh(fbxMesh, vertexInfo);

	Mesh* shardliteMesh = new Mesh(vertexInfo, assetName);
	shardliteMesh->SetUVs(fbxUVs);

	return shardliteMesh;
}


//FbxSkin *pSkin = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);
//if (pSkin == NULL)
//	return nullptr;

// bone count
//int ncBones = pSkin->GetClusterCount();

/*Skeleton* skeleton = new Skeleton();
FbxNode* root;
if (ncBones > 0)
{
// cluster
FbxCluster* cluster = pSkin->GetCluster(0);

// bone ref
FbxNode* pBone = cluster->GetLink();
root = FindRoot(pBone);


//Skeleton skeleton;
skeleton->_joints.push_back(Joint(root->GetName(), -1));
skeleton->_joints[0]._node = root;
PopulateSkeleton(skeleton->_joints[0], root, skeleton, 0);

if (root != nullptr)
{
std::cout << "Root name: " << root->GetName() << std::endl;
}
}*/


/*shardliteMesh->m_skeleton = skeleton;
shardliteMesh->SetJointIndices(jointIndices);
shardliteMesh->SetJointWeights(jointWeights);*/