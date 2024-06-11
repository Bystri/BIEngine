#include <vector>
#include <set>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <tinyxml2.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../../BIEngine/Utilities/Logger.h"

std::string gAssetPath;
std::string gSaveFolder;

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

static tinyxml2::XMLElement* modelLoaderExtractBoneWeightForVertices(tinyxml2::XMLDocument* const pDoc, const aiMesh* const mesh)
{
   tinyxml2::XMLElement* const pBoneWeightsElement = pDoc->NewElement("BoneWeights");
   for (int i = 0; i < mesh->mNumBones; ++i) {
      const std::string name = mesh->mBones[i]->mName.C_Str();

      tinyxml2::XMLElement* const pBoneElement = pDoc->NewElement("Bone");
      pBoneWeightsElement->LinkEndChild(pBoneElement);
      pBoneElement->SetAttribute("name", name.c_str());

      const aiVertexWeight* const pWeights = mesh->mBones[i]->mWeights;

      for (int j = 0; j < mesh->mBones[i]->mNumWeights; ++j) {
         const int vertexId = pWeights[j].mVertexId;
         const float weight = pWeights[j].mWeight;

         if (weight < std::numeric_limits<float>::epsilon()) {
            continue;
         }

         tinyxml2::XMLElement* const pWeightElement = pDoc->NewElement("Weight");
         pBoneElement->LinkEndChild(pWeightElement);
         pWeightElement->SetAttribute("vertId", vertexId);
         pWeightElement->SetAttribute("weight", weight);
      }
   }

   return pBoneWeightsElement;
}

static std::string modelLoaderProcessTexture(const aiTexture* const pTexture)
{
   tinyxml2::XMLDocument textureDoc;
   tinyxml2::XMLElement* const pTexutreElement = textureDoc.NewElement("Texture");
   textureDoc.LinkEndChild(pTexutreElement);

   tinyxml2::XMLElement* const pImgDataElement = textureDoc.NewElement("ImgData");
   pTexutreElement->LinkEndChild(pImgDataElement);
   const std::string imagePath = gAssetPath + "/" + std::string(pTexture->mFilename.C_Str()) + "." + pTexture->achFormatHint;
   pImgDataElement->SetAttribute("path", imagePath.c_str());

   tinyxml2::XMLElement* const pCreationParamsElement = textureDoc.NewElement("CreationParams");
   pTexutreElement->LinkEndChild(pCreationParamsElement);
   pCreationParamsElement->SetAttribute("wrapS", "Repeat");
   pCreationParamsElement->SetAttribute("wrapT", "Repeat");
   pCreationParamsElement->SetAttribute("filterMin", "Linear");
   pCreationParamsElement->SetAttribute("filterMax", "Linear");

   const std::string savePath = gSaveFolder + "/" + pTexture->mFilename.C_Str() + ".bitf";
   textureDoc.SaveFile(savePath.c_str());

   const std::string assetPath = gAssetPath + "/" + pTexture->mFilename.C_Str() + ".bitf";

   return assetPath;
}

static std::string modelLoadMaterial(const aiMaterial* const mat, const aiScene* const scene)
{
   aiString name = mat->GetName();

   tinyxml2::XMLDocument materialXmlDoc;
   tinyxml2::XMLElement* const pMaterialElement = materialXmlDoc.NewElement("Material");
   materialXmlDoc.LinkEndChild(pMaterialElement);

   tinyxml2::XMLElement* const pShaderProgramElement = materialXmlDoc.NewElement("ShaderProgram");
   pMaterialElement->LinkEndChild(pShaderProgramElement);
   pShaderProgramElement->SetAttribute("path", "Effects/mesh.bisp");

   tinyxml2::XMLElement* const pParamsElement = materialXmlDoc.NewElement("Params");
   pMaterialElement->LinkEndChild(pParamsElement);
   pParamsElement->SetAttribute("isDoubleSided", false);

   tinyxml2::XMLElement* const pColorsRgbaElement = materialXmlDoc.NewElement("ColorsRgba");
   pMaterialElement->LinkEndChild(pColorsRgbaElement);
   tinyxml2::XMLElement* const pColorElement = materialXmlDoc.NewElement("Color");
   pColorsRgbaElement->LinkEndChild(pColorElement);
   pColorElement->SetAttribute("paramName", "material.color");
   pColorElement->SetAttribute("r", 1.0f);
   pColorElement->SetAttribute("g", 1.0f);
   pColorElement->SetAttribute("b", 1.0f);
   pColorElement->SetAttribute("a", 1.0f);

   tinyxml2::XMLElement* const pFloatsElement = materialXmlDoc.NewElement("Floats");
   pMaterialElement->LinkEndChild(pFloatsElement);
   for (int i = 0; i < mat->mNumProperties; ++i) {
      if (std::string(mat->mProperties[i]->mKey.C_Str()) != "$mat.shininess") {
         continue;
      }

      tinyxml2::XMLElement* const pFloatElement = materialXmlDoc.NewElement("Float");
      pFloatsElement->LinkEndChild(pFloatElement);
      pFloatElement->SetAttribute("paramName", "material.shininess");
      const float* pVal = (float*)mat->mProperties[i]->mData;
      pFloatElement->SetAttribute("f", *pVal);
   }

   tinyxml2::XMLElement* const pTexturesElement = materialXmlDoc.NewElement("Textures");
   pMaterialElement->LinkEndChild(pTexturesElement);
   if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString Path;

      if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
         if (const aiTexture* pTexture = scene->GetEmbeddedTexture(Path.data)) {
            tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
            pTexturesElement->LinkEndChild(pTextureElement);
            pTextureElement->SetAttribute("paramName", "material.diffuse");
            pTextureElement->SetAttribute("slot", 0);
            const std::string texPath = modelLoaderProcessTexture(pTexture);
            pTextureElement->SetAttribute("path", texPath.c_str());
         }
      }
   } else {
      tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
      pTexturesElement->LinkEndChild(pTextureElement);
      pTextureElement->SetAttribute("paramName", "material.diffuse");
      pTextureElement->SetAttribute("slot", 0);
      pTextureElement->SetAttribute("path", "textures/diffuse_default.bitf");
   }

   if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
      aiString Path;

      if (mat->GetTexture(aiTextureType_SPECULAR, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
         if (const aiTexture* pTexture = scene->GetEmbeddedTexture(Path.data)) {
            tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
            pTexturesElement->LinkEndChild(pTextureElement);
            pTextureElement->SetAttribute("paramName", "material.specular");
            pTextureElement->SetAttribute("slot", 1);
            const std::string texPath = modelLoaderProcessTexture(pTexture);
            pTextureElement->SetAttribute("path", texPath.c_str());
         }
      }
   } else {
      tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
      pTexturesElement->LinkEndChild(pTextureElement);
      pTextureElement->SetAttribute("paramName", "material.specular");
      pTextureElement->SetAttribute("slot", 1);
      pTextureElement->SetAttribute("path", "textures/specular_default.bitf");
   }

   if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
      aiString Path;

      if (mat->GetTexture(aiTextureType_HEIGHT, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
         if (const aiTexture* pTexture = scene->GetEmbeddedTexture(Path.data)) {
            tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
            pTexturesElement->LinkEndChild(pTextureElement);
            pTextureElement->SetAttribute("paramName", "material.normal");
            pTextureElement->SetAttribute("slot", 2);
            const std::string texPath = modelLoaderProcessTexture(pTexture);
            pTextureElement->SetAttribute("path", texPath.c_str());
         }
      }
   } else {
      tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
      pTexturesElement->LinkEndChild(pTextureElement);
      pTextureElement->SetAttribute("paramName", "material.normal");
      pTextureElement->SetAttribute("slot", 2);
      pTextureElement->SetAttribute("path", "textures/normal_default.bitf");
   }

   if (mat->GetTextureCount(aiTextureType_DISPLACEMENT) > 0) {
      aiString Path;

      if (mat->GetTexture(aiTextureType_DISPLACEMENT, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
         if (const aiTexture* pTexture = scene->GetEmbeddedTexture(Path.data)) {
            tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
            pTexturesElement->LinkEndChild(pTextureElement);
            pTextureElement->SetAttribute("paramName", "material.displacement");
            pTextureElement->SetAttribute("slot", 3);
            const std::string texPath = modelLoaderProcessTexture(pTexture);
            pTextureElement->SetAttribute("path", texPath.c_str());
         }
      }
   } else {
      tinyxml2::XMLElement* const pTextureElement = materialXmlDoc.NewElement("Texture");
      pTexturesElement->LinkEndChild(pTextureElement);
      pTextureElement->SetAttribute("paramName", "material.displacement");
      pTextureElement->SetAttribute("slot", 3);
      pTextureElement->SetAttribute("path", "textures/displacement_default.bitf");
   }

   const std::string savePath = gSaveFolder + "/" + name.C_Str() + ".bimat";
   materialXmlDoc.SaveFile(savePath.c_str());

   const std::string assetPath = gAssetPath + "/" + name.C_Str() + ".bimat";

   return assetPath;
}

static void modelLoaderProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene, std::string& meshPath, std::string& matPath)
{
   tinyxml2::XMLDocument skinnedMeshXmlDoc;

   tinyxml2::XMLElement* const pSkinnedMeshElement = skinnedMeshXmlDoc.NewElement("SkinnedMesh");
   skinnedMeshXmlDoc.LinkEndChild(pSkinnedMeshElement);

   tinyxml2::XMLElement* const pVerticesElement = skinnedMeshXmlDoc.NewElement("Vertices");
   pSkinnedMeshElement->LinkEndChild(pVerticesElement);
   for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      tinyxml2::XMLElement* const pVertexElement = skinnedMeshXmlDoc.NewElement("Vertex");
      pVerticesElement->LinkEndChild(pVertexElement);

      // process vertex positions, normals and texture coordinates
      pVertexElement->SetAttribute("posX", mesh->mVertices[i].x);
      pVertexElement->SetAttribute("posY", mesh->mVertices[i].y);
      pVertexElement->SetAttribute("posZ", mesh->mVertices[i].z);

      if (mesh->HasNormals()) {
         pVertexElement->SetAttribute("normX", mesh->mNormals[i].x);
         pVertexElement->SetAttribute("normY", mesh->mNormals[i].y);
         pVertexElement->SetAttribute("normZ", mesh->mNormals[i].z);
      }

      if (mesh->HasTangentsAndBitangents()) {
         pVertexElement->SetAttribute("tanX", mesh->mTangents[i].x);
         pVertexElement->SetAttribute("tanY", mesh->mTangents[i].y);
         pVertexElement->SetAttribute("tanZ", mesh->mTangents[i].z);
      }

      if (mesh->mTextureCoords[0]) {
         pVertexElement->SetAttribute("texX", mesh->mTextureCoords[0][i].x);
         pVertexElement->SetAttribute("texY", mesh->mTextureCoords[0][i].y);
      }
   }

   // process indices
   tinyxml2::XMLElement* const pIndicesElement = skinnedMeshXmlDoc.NewElement("Indices");
   pSkinnedMeshElement->LinkEndChild(pIndicesElement);
   std::string indicesText;
   for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
         const std::string index = std::to_string(face.mIndices[j]);
         indicesText += index + " ";
      }
   }
   indicesText.pop_back();
   pIndicesElement->SetText(indicesText.c_str());

   pSkinnedMeshElement->LinkEndChild(modelLoaderExtractBoneWeightForVertices(&skinnedMeshXmlDoc, mesh));

   //  process material
   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* const material = scene->mMaterials[mesh->mMaterialIndex];
      matPath = modelLoadMaterial(material, scene);
   }

   const std::string savePath = gSaveFolder + "/" + mesh->mName.C_Str() + ".biskmesh";
   skinnedMeshXmlDoc.SaveFile(savePath.c_str());

   meshPath = gAssetPath + "/" + mesh->mName.C_Str() + ".biskmesh";
}

static void modelLoaderProcessMesh(aiMesh* mesh, const aiScene* scene, std::string& meshPath, std::string& matPath)
{
   tinyxml2::XMLDocument skinnedMeshXmlDoc;

   tinyxml2::XMLElement* const pSkinnedMeshElement = skinnedMeshXmlDoc.NewElement("Mesh");
   skinnedMeshXmlDoc.LinkEndChild(pSkinnedMeshElement);

   tinyxml2::XMLElement* const pVerticesElement = skinnedMeshXmlDoc.NewElement("Vertices");
   pSkinnedMeshElement->LinkEndChild(pVerticesElement);
   for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      tinyxml2::XMLElement* const pVertexElement = skinnedMeshXmlDoc.NewElement("Vertex");
      pVerticesElement->LinkEndChild(pVertexElement);

      // process vertex positions, normals and texture coordinates
      pVertexElement->SetAttribute("posX", mesh->mVertices[i].x);
      pVertexElement->SetAttribute("posY", mesh->mVertices[i].y);
      pVertexElement->SetAttribute("posZ", mesh->mVertices[i].z);

      if (mesh->HasNormals()) {
         pVertexElement->SetAttribute("normX", mesh->mNormals[i].x);
         pVertexElement->SetAttribute("normY", mesh->mNormals[i].y);
         pVertexElement->SetAttribute("normZ", mesh->mNormals[i].z);
      }

      if (mesh->HasTangentsAndBitangents()) {
         pVertexElement->SetAttribute("tanX", mesh->mTangents[i].x);
         pVertexElement->SetAttribute("tanY", mesh->mTangents[i].y);
         pVertexElement->SetAttribute("tanZ", mesh->mTangents[i].z);
      }

      if (mesh->mTextureCoords[0]) {
         pVertexElement->SetAttribute("texX", mesh->mTextureCoords[0][i].x);
         pVertexElement->SetAttribute("texY", mesh->mTextureCoords[0][i].y);
      }
   }

   // process indices
   tinyxml2::XMLElement* const pIndicesElement = skinnedMeshXmlDoc.NewElement("Indices");
   pSkinnedMeshElement->LinkEndChild(pIndicesElement);
   std::string indicesText;
   for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
         const std::string index = std::to_string(face.mIndices[j]);
         indicesText += index + " ";
      }
   }
   indicesText.pop_back();
   pIndicesElement->SetText(indicesText.c_str());

   //  process material
   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* const material = scene->mMaterials[mesh->mMaterialIndex];
      matPath = modelLoadMaterial(material, scene);
   }

   const std::string savePath = gSaveFolder + "/" + mesh->mName.C_Str() + ".bimesh";
   skinnedMeshXmlDoc.SaveFile(savePath.c_str());

   meshPath = gAssetPath + "/" + mesh->mName.C_Str() + ".bimesh";
}

static tinyxml2::XMLElement* modelLoaderCreateTransformComponent(tinyxml2::XMLDocument* const pDoc, const glm::mat4& transform)
{
   glm::vec3 localPos;
   glm::vec3 localRot;
   glm::vec3 localSize;

   glm::quat qRotation;
   glm::vec3 skew;
   glm::vec4 perspective;
   glm::decompose(transform, localSize, qRotation, localPos, skew, perspective);

   localRot = glm::eulerAngles(qRotation);
   localRot.x = glm::degrees(localRot.x);
   localRot.y = glm::degrees(localRot.y);
   localRot.z = glm::degrees(localRot.z);

   tinyxml2::XMLElement* const pTransformComponent = pDoc->NewElement("TransformComponent");

   tinyxml2::XMLElement* const pPosition = pDoc->NewElement("Position");
   pPosition->SetAttribute("x", std::to_string(localPos.x).c_str());
   pPosition->SetAttribute("y", std::to_string(localPos.y).c_str());
   pPosition->SetAttribute("z", std::to_string(localPos.z).c_str());
   pTransformComponent->LinkEndChild(pPosition);

   tinyxml2::XMLElement* const pSize = pDoc->NewElement("Size");
   pSize->SetAttribute("w", std::to_string(localSize.x).c_str());
   pSize->SetAttribute("h", std::to_string(localSize.y).c_str());
   pSize->SetAttribute("d", std::to_string(localSize.z).c_str());
   pTransformComponent->LinkEndChild(pSize);

   tinyxml2::XMLElement* const pRotation = pDoc->NewElement("Rotation");
   pRotation->SetAttribute("x", std::to_string(localRot.x).c_str());
   pRotation->SetAttribute("y", std::to_string(localRot.y).c_str());
   pRotation->SetAttribute("z", std::to_string(localRot.z).c_str());
   pTransformComponent->LinkEndChild(pRotation);

   return pTransformComponent;
}

static tinyxml2::XMLElement* modelLoaderCreateSkinnedMeshComponent(tinyxml2::XMLDocument* const pDoc, const std::string& meshPath, const std::string& matPath)
{
   tinyxml2::XMLElement* const pSkinnedMeshComponent = pDoc->NewElement("SkinnedMeshComponent");

   tinyxml2::XMLElement* const pMeshElement = pDoc->NewElement("Mesh");
   pMeshElement->SetAttribute("path", meshPath.c_str());
   pSkinnedMeshComponent->LinkEndChild(pMeshElement);

   tinyxml2::XMLElement* const pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("path", matPath.c_str());
   pSkinnedMeshComponent->LinkEndChild(pMaterialElement);

   return pSkinnedMeshComponent;
}

static tinyxml2::XMLElement* modelLoaderCreateMeshComponent(tinyxml2::XMLDocument* const pDoc, const std::string& meshPath, const std::string& matPath)
{
   tinyxml2::XMLElement* const pSkinnedMeshComponent = pDoc->NewElement("MeshComponent");

   tinyxml2::XMLElement* const pMeshElement = pDoc->NewElement("Mesh");
   pMeshElement->SetAttribute("path", meshPath.c_str());
   pSkinnedMeshComponent->LinkEndChild(pMeshElement);

   tinyxml2::XMLElement* const pMaterialElement = pDoc->NewElement("Material");
   pMaterialElement->SetAttribute("path", matPath.c_str());
   pSkinnedMeshComponent->LinkEndChild(pMaterialElement);

   return pSkinnedMeshComponent;
}

static tinyxml2::XMLElement* modelLoaderProcessNode(tinyxml2::XMLDocument* const pDoc, aiNode* node, const aiScene* scene)
{
   BIEngine::Logger::WriteLog(BIEngine::Logger::LogType::MESSAGE, "Process node: " + std::string(node->mName.C_Str()));

   if (node->mNumMeshes > 1) {
      BIEngine::Logger::WriteLog(BIEngine::Logger::LogType::ERROR, "Can't convert node with meshes num greater than 1;");
      return nullptr;
   }

   tinyxml2::XMLElement* const pActorElement = pDoc->NewElement("Actor");
   pActorElement->SetAttribute("name", node->mName.C_Str());

   tinyxml2::XMLElement* pComponentsElement = pDoc->NewElement("Components");
   pActorElement->LinkEndChild(pComponentsElement);


   const glm::mat4 transform = modelLoaderConvertMatrixToGLMFormat(node->mTransformation);
   pComponentsElement->LinkEndChild(modelLoaderCreateTransformComponent(pDoc, transform));


   if (node->mNumMeshes == 1 && scene->mMeshes[node->mMeshes[0]]->HasBones()) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
      std::string meshPath;
      std::string matPath;
      // modelLoaderProcessSkinnedMesh(mesh, scene, meshPath, matPath);
      // pComponentsElement->LinkEndChild(modelLoaderCreateSkinnedMeshComponent(pDoc, meshPath, matPath));
      modelLoaderProcessMesh(mesh, scene, meshPath, matPath);
      pComponentsElement->LinkEndChild(modelLoaderCreateMeshComponent(pDoc, meshPath, matPath));
   }

   if (node->mNumMeshes == 1 && scene->mMeshes[node->mMeshes[0]]->HasBones() == false) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
      std::string meshPath;
      std::string matPath;
      modelLoaderProcessMesh(mesh, scene, meshPath, matPath);
      pComponentsElement->LinkEndChild(modelLoaderCreateMeshComponent(pDoc, meshPath, matPath));
   }


   tinyxml2::XMLElement* pChildrenElement = pDoc->NewElement("Children");
   pActorElement->LinkEndChild(pChildrenElement);

   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      tinyxml2::XMLElement* const childElement = modelLoaderProcessNode(pDoc, node->mChildren[i], scene);

      if (childElement == nullptr) {
         continue;
      }
      pChildrenElement->LinkEndChild(childElement);
   }

   return pActorElement;
}

// Usage: BIModelConverter.exe <path_to_model_file> <path_to_save_folder> <asset_path>
int main(int argc, char* argv[])
{
   if (argc != 4) {
      BIEngine::Logger::WriteLog(BIEngine::Logger::LogType::ERROR, "Usage: BIModelConverter.exe <path_to_model_file> <path_to_save_folder> <asset_path>");
      return -1;
   }

   const std::string modelPath = argv[1];
   gSaveFolder = argv[2];
   gAssetPath = argv[3];

   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      BIEngine::Logger::WriteLog(BIEngine::Logger::LogType::ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
      return -1;
   }

   tinyxml2::XMLDocument actorXmlDoc;
   tinyxml2::XMLElement* const pActorElemetn = modelLoaderProcessNode(&actorXmlDoc, scene->mRootNode, scene);
   actorXmlDoc.LinkEndChild(pActorElemetn);

   const std::string saveActorPath = gSaveFolder + "/ExportedActor.xml";
   actorXmlDoc.SaveFile(saveActorPath.c_str());

   return 0;
}
