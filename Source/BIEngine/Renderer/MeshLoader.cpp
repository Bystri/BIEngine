#include "MeshLoader.h"

#include <sstream>

#include <tinyxml2.h>

namespace BIEngine {

MeshExtraData::MeshExtraData()
   : m_pMesh(nullptr)
{
}

MeshExtraData::~MeshExtraData()
{
}

bool MeshResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResHandle> pHandle)
{
   std::shared_ptr<MeshExtraData> pExtra = std::make_shared<MeshExtraData>();

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

   pExtra->m_pMesh = std::make_shared<Mesh>(vertices, indices);
   pHandle->SetExtra(pExtra);

   return true;
}

unsigned int MeshResourceLoader::GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
   return rawSize;
}
} // namespace BIEngine
