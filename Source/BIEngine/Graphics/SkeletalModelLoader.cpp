#include "SkeletalModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include "../../3rdParty/stb/stb_image.h"

#include "LightReflectiveMaterial.h"
#include "MaterialLoader.h"
#include "AnimationLoader.h"
#include "Animator.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/TextureLoader.h"

namespace BIEngine {

static std::shared_ptr<LightReflectiveMaterial> modelLoadMaterial(const aiMaterial* const mat, const aiScene* const scene)
{
   aiString name = mat->GetName();

   if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString Path;

      if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
         if (const aiTexture* pTexture = scene->GetEmbeddedTexture(Path.data)) {
            int components;
            int width;
            int height;
            unsigned char* image_data;
            if (pTexture->mHeight == 0) {
               image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(pTexture->pcData), pTexture->mWidth, &width, &height, &components, 0);
            } else {
               image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(pTexture->pcData), pTexture->mWidth * pTexture->mHeight, &width, &height, &components, 0);
            }

            Texture2D::SizedFormat sizedFormat = components > 3 ? Texture2D::SizedFormat::RGBA : Texture2D::SizedFormat::RGB;
            Texture2D::Format texureFormat = components > 3 ? Texture2D::Format::RGBA : Texture2D::Format::RGB;
            auto t2 = Texture2D::Create(width, height, sizedFormat, texureFormat, image_data);

            auto shaderProgramData = std::dynamic_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/skinnedMesh.sp")->GetExtra());

            if (shaderProgramData == nullptr) {
               return nullptr;
            }

            auto material = std::make_shared<LightReflectiveMaterial>(shaderProgramData->GetShaderProgram());
            material->SetDiffuseMap(t2);

            auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

            if (specularMapData == nullptr) {
               return nullptr;
            }

            material->SetSpecularMap(specularMapData->GetTexture());

            auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

            if (normalMapData == nullptr) {
               return nullptr;
            }

            material->SetNormalMap(normalMapData->GetTexture());

            auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

            if (displacementMapData == nullptr) {
               return nullptr;
            }

            material->SetNormalMap(displacementMapData->GetTexture());

            return material;
         } else {
            auto shaderProgramData = std::dynamic_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/skinnedMesh.sp")->GetExtra());

            if (shaderProgramData == nullptr) {
               return nullptr;
            }

            auto material = std::make_shared<LightReflectiveMaterial>(shaderProgramData->GetShaderProgram());

            auto diffuseMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/diffuse_default.bitf")->GetExtra());

            if (diffuseMapData == nullptr) {
               return nullptr;
            }
            material->SetDiffuseMap(diffuseMapData->GetTexture());

            auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

            if (specularMapData == nullptr) {
               return nullptr;
            }

            material->SetSpecularMap(specularMapData->GetTexture());

            auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

            if (normalMapData == nullptr) {
               return nullptr;
            }

            material->SetNormalMap(normalMapData->GetTexture());

            auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

            if (displacementMapData == nullptr) {
               return nullptr;
            }

            material->SetDisplacementMap(displacementMapData->GetTexture());

            return material;
         }
      }
   }

   if (std::string(name.C_Str()).find("bimat") == std::string::npos) {
      auto shaderProgramData = std::dynamic_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/skinnedMesh.sp")->GetExtra());

      if (shaderProgramData == nullptr) {
         return nullptr;
      }

      auto material = std::make_shared<LightReflectiveMaterial>(shaderProgramData->GetShaderProgram());

      auto diffuseMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/diffuse_default.bitf")->GetExtra());

      if (diffuseMapData == nullptr) {
         return nullptr;
      }
      material->SetDiffuseMap(diffuseMapData->GetTexture());

      auto specularMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

      if (specularMapData == nullptr) {
         return nullptr;
      }

      material->SetSpecularMap(specularMapData->GetTexture());

      auto normalMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

      if (normalMapData == nullptr) {
         return nullptr;
      }

      material->SetNormalMap(normalMapData->GetTexture());

      auto displacementMapData = std::dynamic_pointer_cast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

      if (displacementMapData == nullptr) {
         return nullptr;
      }

      material->SetDisplacementMap(displacementMapData->GetTexture());

      return material;
   }

   auto materialData = std::dynamic_pointer_cast<MaterialData>(ResCache::Get()->GetHandle(name.C_Str())->GetExtra());

   if (materialData == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading material for model; Material path: " + std::string(name.C_Str()));
      return nullptr;
   }

   return materialData->GetMaterial();
}

void modelLoaderSetVertexBoneData(SkeletalMesh::VertexBoneData& vertex, int boneID, float weight)
{
   if (weight < std::numeric_limits<float>::epsilon()) {
      return;
   }

   for (int i = 0; i < SkeletalMesh::VertexBoneData::NUM_BONES_PER_VERTEX; ++i) {
      if (vertex.IDs[i] < 0) {
         vertex.Weights[i] = weight;
         vertex.IDs[i] = boneID;
         return;
      }
   }

   Assert(false, "Need more bones per vertex!");
}

using BoneInfoMap = std::map<std::string, std::shared_ptr<Skeleton::BoneInfo>>;

void modelLoaderExtractBoneWeightForVertices(std::vector<SkeletalMesh::VertexBoneData>& vertexBoneData, const aiMesh* const mesh, const BoneInfoMap& boneInfoMap)
{
   for (int i = 0; i < mesh->mNumBones; ++i) {
      const std::string name = mesh->mBones[i]->mName.C_Str();
      auto itr = boneInfoMap.find(name);

      Assert(itr != boneInfoMap.end(), "Found unregistered model bone with name %s", name.c_str());
      if (itr == boneInfoMap.end()) {
         continue;
      }

      const aiVertexWeight* const pWeights = mesh->mBones[i]->mWeights;

      for (int j = 0; j < mesh->mBones[i]->mNumWeights; ++j) {
         const int vertexId = pWeights[j].mVertexId;
         const float weight = pWeights[j].mWeight;

         Assert(vertexId < vertexBoneData.size(), "Bone affects non-existing vertex");
         if (vertexId >= vertexBoneData.size()) {
            continue;
         }

         modelLoaderSetVertexBoneData(vertexBoneData[vertexId], itr->second->id, weight);
      }
   }
}

static std::shared_ptr<SkeletalModelMesh> modelLoaderProcessMesh(aiMesh* mesh, const aiScene* scene, const BoneInfoMap& boneInfoMap)
{
   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;
   std::vector<SkeletalMesh::VertexBoneData> bonesData(mesh->mNumVertices);

   for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;

      // process vertex positions, normals and texture coordinates
      vertex.Position[0] = mesh->mVertices[i].x;
      vertex.Position[1] = mesh->mVertices[i].y;
      vertex.Position[2] = mesh->mVertices[i].z;

      if (mesh->HasNormals()) {
         vertex.Normal[0] = mesh->mNormals[i].x;
         vertex.Normal[1] = mesh->mNormals[i].y;
         vertex.Normal[2] = mesh->mNormals[i].z;
      }

      if (mesh->HasTangentsAndBitangents()) {
         vertex.Tangent[0] = mesh->mTangents[i].x;
         vertex.Tangent[1] = mesh->mTangents[i].y;
         vertex.Tangent[2] = mesh->mTangents[i].z;
      }

      if (mesh->mTextureCoords[0]) {
         vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
         vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;
      }

      vertices.push_back(vertex);
   }

   // process indices
   for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
         indices.push_back(face.mIndices[j]);
      }
   }

   modelLoaderExtractBoneWeightForVertices(bonesData, mesh, boneInfoMap);

   std::shared_ptr<SkeletalMesh> pLoadedMesh = std::make_shared<SkeletalMesh>(vertices, indices, bonesData);

   std::shared_ptr<Material> pMaterial = nullptr;
   // process material
   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      pMaterial = modelLoadMaterial(material, scene);
   }

   std::shared_ptr<SkeletalModelMesh> pModelMeshNode = std::make_shared<SkeletalModelMesh>(pLoadedMesh, pMaterial);

   return pModelMeshNode;
}

static void modelLoaderProcessNode(std::shared_ptr<SkeletalModel> pModel, aiNode* node, const aiScene* scene, const BoneInfoMap& boneInfoMap)
{
   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      pModel->AddSkeletalModelMesh(modelLoaderProcessMesh(mesh, scene, boneInfoMap));
   }


   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      modelLoaderProcessNode(pModel, node->mChildren[i], scene, boneInfoMap);
   }
}

static inline glm::mat4 modelLoaderConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
   glm::mat4 to;
   // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
   to[0][0] = from.a1;
   to[1][0] = from.a2;
   to[2][0] = from.a3;
   to[3][0] = from.a4;
   to[0][1] = from.b1;
   to[1][1] = from.b2;
   to[2][1] = from.b3;
   to[3][1] = from.b4;
   to[0][2] = from.c1;
   to[1][2] = from.c2;
   to[2][2] = from.c3;
   to[3][2] = from.c4;
   to[0][3] = from.d1;
   to[1][3] = from.d2;
   to[2][3] = from.d3;
   to[3][3] = from.d4;
   return to;
}

static void modelLoaderProcessBoneInfo(BoneInfoMap& boneInfoMap, aiMesh* const mesh, const aiScene* const scene)
{
   for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
      std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

      if (boneInfoMap.find(boneName) != boneInfoMap.end()) {
         continue;
      }

      std::shared_ptr<Skeleton::BoneInfo> newBoneInfo = std::make_shared<Skeleton::BoneInfo>();
      newBoneInfo->id = boneInfoMap.size();
      newBoneInfo->name = mesh->mBones[boneIndex]->mName.C_Str();
      newBoneInfo->offset = modelLoaderConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
      boneInfoMap[boneName] = newBoneInfo;
   }
}

static std::shared_ptr<Skeleton::BoneInfo> modelLoaderLoadBones(BoneInfoMap& boneInfoMap, aiNode* node, const aiScene* scene)
{
   auto boneMapItr = boneInfoMap.find(node->mName.C_Str());
   if (boneMapItr != boneInfoMap.end()) {
      // This node is actually bone! Get needed info and return
      boneMapItr->second->parentNodeTransform = modelLoaderConvertMatrixToGLMFormat(node->mTransformation);

      for (unsigned int i = 0; i < node->mNumChildren; i++) {
         boneMapItr->second->children.push_back(modelLoaderLoadBones(boneInfoMap, node->mChildren[i], scene));
      }

      return boneMapItr->second;
   }

   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* const mesh = scene->mMeshes[node->mMeshes[i]];
      modelLoaderProcessBoneInfo(boneInfoMap, mesh, scene);
   }

   std::shared_ptr<Skeleton::BoneInfo> ret;
   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      ret = modelLoaderLoadBones(boneInfoMap, node->mChildren[i], scene);
   }

   return ret;
}

bool SkeletalModelResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<SkeletalModelData> pExtra = std::make_shared<SkeletalModelData>();

   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFileFromMemory(rawBuffer, rawSize, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      Logger::WriteLog(Logger::LogType::ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
      return false;
   }

   BoneInfoMap boneInfoMap;
   std::shared_ptr<Skeleton::BoneInfo> pSkeletonRoot = modelLoaderLoadBones(boneInfoMap, scene->mRootNode, scene);
   std::shared_ptr<Skeleton> pSkeleton = std::make_shared<Skeleton>(pSkeletonRoot);
   pExtra->m_pModel = std::make_shared<SkeletalModel>(pSkeleton);
   modelLoaderProcessNode(pExtra->m_pModel, scene->mRootNode, scene, boneInfoMap);

   pHandle->SetExtra(pExtra);


   return true;
}


} // namespace BIEngine
