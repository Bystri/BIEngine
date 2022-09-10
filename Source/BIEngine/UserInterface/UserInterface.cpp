#include "UserInterface.h"

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Graphics2D/Texture.h"
#include "../Graphics2D/Shader.h"
#include "../Graphics2D/Opengl/ShadersLoaderOpenGL.h"
#include "../Utilities/Logger.h"

namespace BIEngine
{

    //Содержит всю информацию о символе загруженного используя FreeType
    struct Character {
        unsigned int TextureID; //ID текстуры с глифой
        glm::ivec2   Size;      //Размер глифы
        glm::ivec2   Bearing;   //Отступ от основания глифы до верхнего-левого угла
        unsigned int Advance;   //Горизонтальный отступ до следующей глифы
    };


    //Класс отвечающий за отрисовку текста. Может единоразово хранить информация для отрисовки только одного шрифта и размера
    class TextRenderer
    {
    public:
        TextRenderer(unsigned int width, unsigned int height);

        //Создает набор текстур символов для заданного шрифта
        void Load(std::string font, unsigned int fontSize);

        void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
    private:
        unsigned int VAO, VBO;

        Shader m_textShader;

        std::map<char, Character> m_characters;
    };

    TextRenderer::TextRenderer(unsigned int width, unsigned int height)
    {
        //Загрузка шейдеров
        std::shared_ptr<OpenglShaderData> vertTextShaderData = std::static_pointer_cast<OpenglShaderData>(ResCache::Get()->GetHandle("Effects/text_2d.vs")->GetExtra());
        std::shared_ptr<OpenglShaderData> fragTextShaderxData = std::static_pointer_cast<OpenglShaderData>(ResCache::Get()->GetHandle("Effects/text_2d.frag")->GetExtra());

        m_textShader.Compile(vertTextShaderData->GetShaderIndex(), fragTextShaderxData->GetShaderIndex());

        m_textShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
        m_textShader.SetInteger("text", 0);

        //Создаем буферы для текстур
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void TextRenderer::Load(std::string font, unsigned int fontSize)
    {

        m_characters.clear();


        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            Logger::WriteLog(Logger::LogType::ERROR, "FREETYPE: Could not init FreeType Library");

        FT_Face face;
        std::shared_ptr<ResHandle> resHandle = ResCache::Get()->GetHandle("Fonts/arial.ttf");
        if (FT_New_Memory_Face(ft, (FT_Byte*)resHandle->Buffer(), resHandle->Size(), 0, &face))
            Logger::WriteLog(Logger::LogType::ERROR, "FREETYPE: Failed to load font");


        FT_Set_Pixel_Sizes(face, 0, fontSize);

        //Отключить ограничение байтового выравнивания
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //Предзагружаем текстуры для первых 128 ASKII-символов
        for (GLubyte c = 0; c < 128; ++c)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                Logger::WriteLog(Logger::LogType::ERROR, "FREETYTPE: Failed to load Glyph");
                continue;
            }
            

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            m_characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
    {

        m_textShader.Use();
        m_textShader.SetVector3f("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(this->VAO);

   
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = m_characters[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y + (m_characters['H'].Bearing.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f },
                { xpos,     ypos,       0.0f, 0.0f },

                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f }
            };
            

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //Перемещаем курсор к следующему символу
            x += (ch.Advance >> 6) * scale;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TextRenderer* g_TextRenderer = nullptr;


    /***Static***/
    Text::Text(int id)
        : IElementUI(id)
    {

    }

    void Text::OnRender(float dt)
    {
        g_TextRenderer->RenderText(m_text, m_x, m_y, m_scale);
    }
    

    /***User Interface***/
    UserInterface::UserInterface()
        : m_nextId(0)
        , m_elements()
    {

    }

    bool UserInterface::Init(unsigned int width, unsigned int height)
    {
        if (!g_TextRenderer)
        {
            g_TextRenderer = new TextRenderer(width, height);

            if (!g_TextRenderer)
            {
                Logger::WriteLog(Logger::LogType::ERROR, "Unable to create instance of TextRenderer");
                return false;
            }
        }

        g_TextRenderer->Load("fonts/arial.TTF", 24);

        return true;
    }

    void UserInterface::Shutdown()
    {
        for (const auto& element : m_elements)
        {
            if (element.second)
                delete element.second;
        }

        m_elements.clear();

        if (g_TextRenderer)
            delete g_TextRenderer;
    }

    int UserInterface::CreateStatic(float x, float y, float scale, const std::string& text)
    {
        Text* staticElement = new Text(m_nextId);

        staticElement->SetCoord(x, y, scale);
        staticElement->SetText(text);

        m_elements.insert({ m_nextId, staticElement });
        ++m_nextId;

        return (m_nextId - 1);
    }

    Text* UserInterface::GetStatic(int id)
    {
        auto itr = m_elements.find(id);
        if (itr == m_elements.end()) 
        {
            Logger::WriteLog(Logger::LogType::ERROR, "Attempt to get a nonexistent UI element");
            return nullptr;
        }

        Text* staticElement = dynamic_cast<Text*>(itr->second);
        if (staticElement == nullptr)
        {
            Logger::WriteLog(Logger::LogType::ERROR, "UI Element with id " + std::to_string(id) + "  is not Static");
            return nullptr;
        }

        return staticElement;
    }

    void UserInterface::OnRender(float dt)
    {
        for (const auto& element : m_elements)
        {
            element.second->OnRender(dt);
        }
    }
}