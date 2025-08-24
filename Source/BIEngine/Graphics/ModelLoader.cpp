#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include "../../3rdParty/stb/stb_image.h"

#include "MaterialLoader.h"
#include "AnimationLoader.h"
#include "Animator.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/TextureLoader.h"

namespace BIEngine {

static SharedPtr<Material> modelLoadMaterial(const aiMaterial* const mat, const aiScene* const scene)
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

            auto shaderProgramData = StaticPointerCast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/mesh.bisp")->GetExtra());

            if (shaderProgramData == nullptr) {
               return nullptr;
            }

            auto material = MakeShared<Material>(shaderProgramData->GetShaderProgram());
            material->SetColorRgba("material.color", ColorRgba(1.0f, 1.0f, 1.0f, 1.0f));
            material->SetFloat("material.shininess", 16.0f);

            material->AddTexture("material.diffuse", 0, t2);

            auto specularMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

            if (specularMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.specular", 1, specularMapData->GetTexture());

            auto normalMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

            if (normalMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.normal", 2, normalMapData->GetTexture());

            auto displacementMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

            if (displacementMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.displacement", 3, displacementMapData->GetTexture());

            return material;
         } else {
            auto shaderProgramData = StaticPointerCast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/mesh.bisp")->GetExtra());

            if (shaderProgramData == nullptr) {
               return nullptr;
            }

            auto material = MakeShared<Material>(shaderProgramData->GetShaderProgram());
            material->SetColorRgba("material.color", ColorRgba(1.0f, 1.0f, 1.0f, 1.0f));
            material->SetFloat("material.shininess", 16.0f);

            auto diffuseMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/diffuse_default.bitf")->GetExtra());

            if (diffuseMapData == nullptr) {
               return nullptr;
            }
            material->AddTexture("material.diffuse", 0, diffuseMapData->GetTexture());

            auto specularMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

            if (specularMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.specular", 1, specularMapData->GetTexture());

            auto normalMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

            if (normalMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.normal", 2, normalMapData->GetTexture());

            auto displacementMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

            if (displacementMapData == nullptr) {
               return nullptr;
            }

            material->AddTexture("material.displacement", 3, displacementMapData->GetTexture());

            return material;
         }
      }
   }

   if (String(name.C_Str()).Find("bimat") == String::NPos) {
      auto shaderProgramData = StaticPointerCast<ShaderProgramData>(ResCache::Get()->GetHandle("Effects/mesh.bisp")->GetExtra());

      if (shaderProgramData == nullptr) {
         return nullptr;
      }

      auto material = MakeShared<Material>(shaderProgramData->GetShaderProgram());
      material->SetColorRgba("material.color", ColorRgba(1.0f, 1.0f, 1.0f, 1.0f));
      material->SetFloat("material.shininess", 16.0f);

      auto diffuseMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/diffuse_default.bitf")->GetExtra());

      if (diffuseMapData == nullptr) {
         return nullptr;
      }
      material->AddTexture("material.diffuse", 0, diffuseMapData->GetTexture());

      auto specularMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/specular_default.bitf")->GetExtra());

      if (specularMapData == nullptr) {
         return nullptr;
      }

      material->AddTexture("material.specular", 1, specularMapData->GetTexture());

      auto normalMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/normal_default.bitf")->GetExtra());

      if (normalMapData == nullptr) {
         return nullptr;
      }

      material->AddTexture("material.normal", 2, normalMapData->GetTexture());

      auto displacementMapData = StaticPointerCast<TextureData>(ResCache::Get()->GetHandle("Textures/displacement_default.bitf")->GetExtra());

      if (displacementMapData == nullptr) {
         return nullptr;
      }

      material->AddTexture("material.displacement", 3, displacementMapData->GetTexture());

      return material;
   }

   auto materialData = StaticPointerCast<MaterialData>(ResCache::Get()->GetHandle(name.C_Str())->GetExtra());

   if (materialData == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading material for model; Material path: " + String(name.C_Str()));
      return nullptr;
   }

   return materialData->GetMaterial();
}

static SharedPtr<ModelMesh> modelLoaderProcessMesh(aiMesh* mesh, const aiScene* scene)
{
   DynamicArray<Vertex> vertices;
   DynamicArray<unsigned int> indices;

   vertices.Reserve(mesh->mNumVertices);

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

      vertices.PushBack(vertex);
   }

   // process indices
   for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
         indices.PushBack(face.mIndices[j]);
      }
   }

   SharedPtr<Mesh> pLoadedMesh = MakeShared<Mesh>(std::move(vertices), std::move(indices));

   SharedPtr<Material> pMaterial = nullptr;
   // process material
   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      pMaterial = modelLoadMaterial(material, scene);
   }

   SharedPtr<ModelMesh> pModelMeshNode = MakeShared<ModelMesh>(pLoadedMesh, pMaterial);

   return pModelMeshNode;
}

static void modelLoaderProcessNode(SharedPtr<Model> pModel, aiNode* node, const aiScene* scene)
{
   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      pModel->AddModelMesh(modelLoaderProcessMesh(mesh, scene));
   }


   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      modelLoaderProcessNode(pModel, node->mChildren[i], scene);
   }
}

bool ModelResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
{
   SharedPtr<ModelData> pExtra = MakeShared<ModelData>();

   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFileFromMemory(rawBuffer, rawSize, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      Logger::WriteLog(Logger::LogType::ERROR, "Assimp error: " + String(importer.GetErrorString()));
      return false;
   }

   pExtra->m_pModel = MakeShared<Model>();

   modelLoaderProcessNode(pExtra->m_pModel, scene->mRootNode, scene);
   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
