#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include "LightReflectiveMaterial.h"
#include "../Utilities/Logger.h"
#include "../Renderer/ShadersLoader.h"
#include "../Renderer/ImageLoader.h"

namespace BIEngine {

static std::shared_ptr<Texture2D> modelLoadMaterialTexture(const aiMaterial* const mat, aiTextureType type)
{
   if (mat->GetTextureCount(type) == 0) {
      return nullptr;
   }

   aiString path;
   mat->GetTexture(type, 0, &path);

   auto textureImageData = std::static_pointer_cast<ImageExtraData>(ResCache::Get()->GetHandle(path.C_Str())->GetExtra());

   if (textureImageData == nullptr) {
      Logger::WriteLog(Logger::LogType::ERROR, "Error while loading image for model; Image path: " + std::string(path.C_Str()));
      return nullptr;
   }

   // TODO: We create new texture for each cached image. Make new texture loader
   std::shared_ptr<Texture2D> pLoadedTexture = Texture2D::Create(textureImageData->GetWidth(), textureImageData->GetHeight(), Texture2D::Format::RGBA, textureImageData->GetData());
   return pLoadedTexture;
}

static std::shared_ptr<LightReflectiveMaterial> modelLoadMaterial(const aiMaterial* const mat)
{
   static const std::string STANDARD_MESH_SHADER_PROGRAM_PATH = "Effects/mesh.sp";
   std::shared_ptr<ShaderProgramData> pShaderProgramData = std::static_pointer_cast<ShaderProgramData>(ResCache::Get()->GetHandle(STANDARD_MESH_SHADER_PROGRAM_PATH)->GetExtra());

   std::shared_ptr<LightReflectiveMaterial> pMaterial = std::make_shared<LightReflectiveMaterial>(pShaderProgramData->GetShaderProgram());

   pMaterial->SetDiffuseMap(modelLoadMaterialTexture(mat, aiTextureType_DIFFUSE));
   pMaterial->SetSpecularMap(modelLoadMaterialTexture(mat, aiTextureType_SPECULAR));

   return pMaterial;
}

static std::shared_ptr<ModelMesh> modelLoaderProcessMesh(aiMesh* mesh, const aiScene* scene)
{
   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;

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

      if (mesh->HasTextureCoords(i)) {
         vertex.TexCoords[0] = mesh->mTextureCoords[i]->x;
         vertex.TexCoords[1] = mesh->mTextureCoords[i]->y;
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

   std::shared_ptr<Mesh> pLoadedMesh = std::make_shared<Mesh>(vertices, indices);

   std::shared_ptr<Material> pMaterial = nullptr;
   // process material
   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      pMaterial = modelLoadMaterial(material);
   }

   std::shared_ptr<ModelMesh> pModelMeshNode = std::make_shared<ModelMesh>(pLoadedMesh, pMaterial);

   return pModelMeshNode;
}

static void modelLoaderProcessNode(std::shared_ptr<Model> pModel, aiNode* node, const aiScene* scene)
{
   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      pModel->AddModelMesh(modelLoaderProcessMesh(mesh, scene));
   }


   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      modelLoaderProcessNode(pModel, node->mChildren[i], scene);
   }
}

bool ObjModelResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<ModelData> pExtra = std::make_shared<ModelData>();

   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFileFromMemory(rawBuffer, rawSize, aiProcess_Triangulate | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      Logger::WriteLog(Logger::LogType::ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
      return false;
   }

   pExtra->m_pModel = std::make_shared<Model>();

   modelLoaderProcessNode(pExtra->m_pModel, scene->mRootNode, scene);
   pHandle->SetExtra(pExtra);

   return true;
}

} // namespace BIEngine
