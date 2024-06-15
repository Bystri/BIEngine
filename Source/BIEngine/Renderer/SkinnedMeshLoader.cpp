#include "SkinnedMeshLoader.h"

#include <sstream>

#include <tinyxml2.h>

namespace BIEngine {

SkinnedMeshExtraData::SkinnedMeshExtraData()
   : m_pSkinnedMesh(nullptr)
{
}

SkinnedMeshExtraData::~SkinnedMeshExtraData()
{
}

void skinnedMeshLoaderSetVertexBoneData(SkinnedMesh::VertexBoneData& vertex, const std::string& boneName, const float weight)
{
   if (weight < std::numeric_limits<float>::epsilon()) {
      return;
   }

   for (int i = 0; i < SkinnedMesh::VertexBoneData::NUM_BONES_PER_VERTEX; ++i) {
      if (vertex.boneNames[i] == "") {
         vertex.Weights[i] = weight;
         vertex.boneNames[i] = boneName;
         return;
      }
   }

   Assert(false, "Need more bones per vertex!");
}

bool SkinnedMeshResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<SkinnedMeshExtraData> pExtra = std::make_shared<SkinnedMeshExtraData>();

   tinyxml2::XMLDocument xmlDoc;
   tinyxml2::XMLError error = xmlDoc.Parse(pRawBuffer, rawSize);

   if (error != tinyxml2::XMLError::XML_SUCCESS) {
      return false;
   }

   tinyxml2::XMLElement* pRoot = xmlDoc.RootElement();
   if (!pRoot) {
      return false;
   }

   tinyxml2::XMLElement* pVerticesElement = pRoot->FirstChildElement("Vertices");
   if (!pVerticesElement) {
      return false;
   }

   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;

   for (tinyxml2::XMLElement* pVertElement = pVerticesElement->FirstChildElement(); pVertElement; pVertElement = pVertElement->NextSiblingElement()) {
      Vertex vertex;

      // process vertex positions, normals and texture coordinates
      vertex.Position[0] = pVertElement->FloatAttribute("posX");
      vertex.Position[1] = pVertElement->FloatAttribute("posY");
      vertex.Position[2] = pVertElement->FloatAttribute("posZ");

      vertex.Normal[0] = pVertElement->FloatAttribute("normX");
      vertex.Normal[1] = pVertElement->FloatAttribute("normY");
      vertex.Normal[2] = pVertElement->FloatAttribute("normZ");

      vertex.Tangent[0] = pVertElement->FloatAttribute("tanX");
      vertex.Tangent[1] = pVertElement->FloatAttribute("tanY");
      vertex.Tangent[2] = pVertElement->FloatAttribute("tanZ");

      vertex.TexCoords[0] = pVertElement->FloatAttribute("texX");
      vertex.TexCoords[1] = pVertElement->FloatAttribute("texY");

      vertices.push_back(vertex);
   }


   tinyxml2::XMLElement* pIndicesElement = pRoot->FirstChildElement("Indices");
   if (!pIndicesElement) {
      return false;
   }

   std::istringstream indicesStream(pIndicesElement->GetText());
   // process indices
   int index;
   while (indicesStream >> index) {
      indices.push_back(index);
   }

   tinyxml2::XMLElement* pBoneWeightsElement = pRoot->FirstChildElement("BoneWeights");
   if (!pBoneWeightsElement) {
      return false;
   }

   std::vector<SkinnedMesh::VertexBoneData> bonesData(vertices.size());

   for (tinyxml2::XMLElement* pBoneElement = pBoneWeightsElement->FirstChildElement(); pBoneElement; pBoneElement = pBoneElement->NextSiblingElement()) {

      const std::string boneName = pBoneElement->Attribute("name");

      for (tinyxml2::XMLElement* pVertBoneElement = pBoneElement->FirstChildElement(); pVertBoneElement; pVertBoneElement = pVertBoneElement->NextSiblingElement()) {
         const int vertIdx = pVertBoneElement->IntAttribute("vertId");
         const float weight = pVertBoneElement->FloatAttribute("weight");

         skinnedMeshLoaderSetVertexBoneData(bonesData[vertIdx], boneName, weight);
      }
   }

   pExtra->m_pSkinnedMesh = std::make_shared<SkinnedMesh>(vertices, indices, bonesData);
   pHandle->SetExtra(pExtra);

   return true;
}

unsigned int SkinnedMeshResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
   return rawSize;
}
} // namespace BIEngine
