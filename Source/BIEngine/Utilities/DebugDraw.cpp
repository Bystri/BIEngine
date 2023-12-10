#include "DebugDraw.h"

#include <queue>

namespace BIEngine {

std::shared_ptr<ShaderProgram> g_pDebugShader;
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
                                      "uniform vec3 color;\n"
                                      "void main()\n"
                                      "{\n"
                                      "   FragColor = vec4(color, 1.0f);\n"
                                      "}\n\0";

   int vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
   glCompileShader(vertexShader);

   int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   glCompileShader(fragmentShader);

   g_pDebugShader = std::make_shared<ShaderProgram>();
   g_pDebugShader->Compile(vertexShader, fragmentShader);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}

class DbgLine {
   unsigned int VBO, VAO;
   std::vector<float> vertices;
   glm::vec3 startPoint;
   glm::vec3 endPoint;
   glm::mat4 MVP;
   glm::vec3 lineColor;

public:
   DbgLine(const glm::vec3& start, const glm::vec3& end)
   {

      startPoint = start;
      endPoint = end;
      lineColor = glm::vec3(1, 1, 1);
      MVP = glm::mat4(1.0f);

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
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

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

   int SetColor(glm::vec3 color)
   {
      lineColor = color;
      return 1;
   }

   int Draw()
   {
      g_pDebugShader->Use();
      g_pDebugShader->SetVector3f("color", lineColor, false);

      glBindVertexArray(VAO);
      glDrawArrays(GL_LINES, 0, 2);
      return 1;
   }
};

struct LineInfo {
   LineInfo()
      : fromPoint(), toPoint(), color()
   {
   }

   glm::vec3 fromPoint;
   glm::vec3 toPoint;
   glm::vec3 color;
};

std::queue<LineInfo> m_drawReqQueue;

void DebugDraw::Line(const glm::vec3& fromPoint, const glm::vec3& toPoint, const glm::vec3& color)
{
   LineInfo info;
   info.fromPoint = fromPoint;
   info.toPoint = toPoint;
   info.color = color;

   m_drawReqQueue.push(info);
}

void DebugDraw::Draw()
{
   while (!m_drawReqQueue.empty()) {
      LineInfo info = m_drawReqQueue.front();
      m_drawReqQueue.pop();

      DbgLine line(info.fromPoint, info.toPoint);

      line.SetColor(info.color);
      line.Draw();
   }
}

} // namespace BIEngine
