#include "DebugDraw.h"

#include "../StdLib/Queue.h"
#include "../Renderer/MeshGeometryGenerator.h"

namespace BIEngine {

SharedPtr<ShaderProgram> g_pDebugShader;
glm::mat4 projMatrix;
glm::mat4 viewMatrix;

void DebugDraw::Init()
{
   const char* vertexShaderSource = "#version 420 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "layout(std140, binding = 0) uniform Global\n"
                                    "{\n"
                                    "   mat4 projection;\n"
                                    "   mat4 view;\n"
                                    "};\n"
                                    "void main()\n"
                                    "{\n"
                                    "   gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                    "}\0";
   const char* fragmentShaderSource = "#version 420 core\n"
                                      "out vec4 FragColor;\n"
                                      "uniform vec4 color;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   FragColor = color;\n"
                                      "}\n\0";

   int vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
   glCompileShader(vertexShader);

   int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   glCompileShader(fragmentShader);

   g_pDebugShader = MakeShared<ShaderProgram>();
   g_pDebugShader->Compile(vertexShader, fragmentShader);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}

class DbgLine {
   unsigned int VBO, VAO;
   DynamicArray<float> vertices;
   glm::vec3 startPoint;
   glm::vec3 endPoint;
   ColorRgba lineColor;

public:
   DbgLine(const glm::vec3& start, const glm::vec3& end)
   {
      startPoint = start;
      endPoint = end;
      lineColor = COLOR_WHITE;

      vertices = {
         start.x,
         start.y,
         start.z,
         end.x,
         end.y,
         end.z,
      };

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vertices.Size() * sizeof(float), vertices.Data(), GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
   }

   ~DbgLine()
   {
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
   }

   int SetColor(const ColorRgba& color)
   {
      lineColor = color;
      return 1;
   }

   int Draw()
   {
      g_pDebugShader->Use();
      g_pDebugShader->SetColorRgba("color", lineColor, false);

      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, 2);
      return 1;
   }
};

class DbgPoly {
   unsigned int VBO, VAO;
   DynamicArray<glm::vec3> m_vertices;
   ColorRgba m_color;

public:
   DbgPoly(const DynamicArray<glm::vec3>& verts)
   {
      m_vertices.Reserve(verts.Size());

      for (int i = 1; i < verts.Size() - 1; ++i) {
         m_vertices.PushBack(verts[0]);
         m_vertices.PushBack(verts[i]);
         m_vertices.PushBack(verts[i + 1]);
      }

      m_color = COLOR_WHITE;

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_vertices.Size(), m_vertices.Data(), GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
   }

   ~DbgPoly()
   {
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
   }

   int SetColor(const ColorRgba& color)
   {
      m_color = color;
      return 1;
   }

   int Draw()
   {
      g_pDebugShader->Use();
      g_pDebugShader->SetColorRgba("color", m_color, false);

      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, m_vertices.Size());
      return 1;
   }
};

class DbgSphere {
   unsigned int VBO, EBO, VAO;
   DynamicArray<glm::vec3> m_vertices;
   DynamicArray<unsigned int> m_indices;
   ColorRgba m_color;
   bool m_isLit;

public:
   DbgSphere(const glm::vec3 center, const float radius, const bool isLit)
   {
      const Mesh sphereMesh = MeshGeometryGenerator::CreateSphere(radius, 8.0f, 8.0f);

      const DynamicArray<Vertex>& sphereVerts = sphereMesh.GetVertices();
      for (int i = 0; i < sphereVerts.Size(); ++i) {
         m_vertices.PushBack(sphereVerts[i].Position + center);
   }

      const DynamicArray<unsigned int>& sphereIndices = sphereMesh.GetIndices();
      for (int i = 0; i < sphereIndices.Size(); ++i) {
         m_indices.PushBack(sphereIndices[i]);
      }

      m_color = COLOR_WHITE;

      m_isLit = isLit;

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);
      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_vertices.Size(), m_vertices.Data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.Size(), m_indices.Data(), GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
   }

   ~DbgSphere()
   {
      glDeleteBuffers(1, &EBO);
      glDeleteBuffers(1, &VBO);
      glDeleteVertexArrays(1, &VAO);
   }

   int SetColor(const ColorRgba& color)
   {
      m_color = color;
      return 1;
   }

   int Draw()
   {
      g_pDebugShader->Use();
      g_pDebugShader->SetColorRgba("color", m_color, false);

      glBindVertexArray(VAO);
      if (!m_isLit) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      glDrawElements(GL_TRIANGLES, m_indices.Size(), GL_UNSIGNED_INT, 0);

      if (!m_isLit) {
         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      return 1;
   }
};

struct LineInfo {
   glm::vec3 fromPoint;
   glm::vec3 toPoint;
   ColorRgba color;
   float time = 0.0f;
};

DynamicArray<LineInfo> m_drawReqQueue;

struct PolyInfo {
   DynamicArray<glm::vec3> verts;
   ColorRgba color;
   float time = 0.0f;
};

DynamicArray<PolyInfo> m_drawPolyQueue;

struct SphereInfo {
   glm::vec3 center;
   float radius = 0.0f;
   ColorRgba color;
   bool isLit = false;
   float time = 0.0f;
};

DynamicArray<SphereInfo> m_drawSphereQueue;

void DebugDraw::Sphere(const glm::vec3& center, const float radius, const ColorRgba& color, float time, bool isLit)
{
   SphereInfo info;
   info.center = center;
   info.radius = radius;
   info.color = color;
   info.time = time;
   info.isLit = isLit;

   m_drawSphereQueue.PushBack(info);
}

void DebugDraw::Line(const glm::vec3& fromPoint, const glm::vec3& toPoint, const ColorRgba& color, float time)
{
   LineInfo info;
   info.fromPoint = fromPoint;
   info.toPoint = toPoint;
   info.color = color;
   info.time = time;

   m_drawReqQueue.PushBack(info);
}

void DebugDraw::Poly(const DynamicArray<glm::vec3>& verts, const ColorRgba& color, float time)
{
   PolyInfo poly;
   poly.verts = verts;
   poly.color = color;
   poly.time = time;

   m_drawPolyQueue.PushBack(poly);
}

void DebugDraw::Draw(const GameTimer& gt)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   for (int i = m_drawSphereQueue.Size() - 1; i >= 0; --i) {
      SphereInfo& info = m_drawSphereQueue[i];

      DbgSphere sphere(info.center, info.radius, info.isLit);

      sphere.SetColor(info.color);
      sphere.Draw();

      info.time -= gt.DeltaTime();
      if (info.time <= 0.0f) {
         m_drawSphereQueue.Erase(m_drawSphereQueue.Begin() + i);
      }
   }

   for (int i = m_drawPolyQueue.Size() - 1; i >= 0; --i) {
      PolyInfo& info = m_drawPolyQueue[i];

      DbgPoly poly(info.verts);

      poly.SetColor(info.color);
      poly.Draw();

      info.time -= gt.DeltaTime();
      if (info.time <= 0.0f) {
         m_drawPolyQueue.Erase(m_drawPolyQueue.Begin() + i);
      }
   }

   for (int i = m_drawReqQueue.Size() - 1; i >= 0; --i) {
      LineInfo& info = m_drawReqQueue[i];

      DbgLine line(info.fromPoint, info.toPoint);

      line.SetColor(info.color);
      line.Draw();

      info.time -= gt.DeltaTime();
      if (info.time <= 0.0f) {
         m_drawReqQueue.Erase(m_drawReqQueue.Begin() + i);
      }
   }

   glDisable(GL_BLEND);
}

} // namespace BIEngine
