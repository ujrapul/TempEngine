// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "TextBox.hpp"

#include "Drawable.hpp"
#include "Engine.hpp"
#include "EngineUtils.hpp"
#include "FontLoader.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include <cstddef>
#include <exception>
#include <string>
#ifdef EDITOR
#include "Hoverable.hpp"
#endif
#include <vector>

// NOTE: Don't use inline global mutexes, it'll stall multiple instances of the same object
namespace Temp::TextBox
{
  namespace
  {
    constexpr float advanceY = 150.f;
#ifdef EDITOR
    constexpr float offsetY = -(float)Font::Padding() * 2;
#endif

    // Bring the cursor down to the next line
    void MoveCursor(float& x, float& y, int& characterCount)
    {
      x = 0;
      y -= advanceY;
      characterCount = 0;
    }

    void PopulateVerticesIndices(Data& textBox,
                                 std::string_view string,
                                 std::vector<float>& vertices,
                                 std::vector<unsigned int>& indices,
                                 float& x,
                                 float& y,
                                 int& characterIdx,
                                 int& characterCount)
    {
      // iterate through all characters
      for (unsigned int i = 0; i < string.length(); ++i)
      {
        char c = string[i];
        // Should only be possible at runtime.
        // Editor shouldn't support this.
        if (c == '\n')
        {
          MoveCursor(x, y, characterCount);
          characterCount = (int)string.length() - i + 1;
          continue;
        }
        Font::Character ch;
        try
        {
          ch = Font::Characters(textBox.fontType).at(c);
        }
        catch (std::exception&)
        {
          Logger::LogErr(std::string("[TextBox] Could not parse: ") + c);
          continue;
        }

        float xpos = x + ch.bearing.x;
        float ypos = y - ((float)ch.size.y - ch.bearing.y);

        float w = (float)ch.size.x;
        float h = (float)ch.size.y;

        textBox.size.x = Math::Max(textBox.size.x, xpos + w);
        textBox.size.y = Math::Max(textBox.size.y, h);

        unsigned int offset = 4 * characterIdx;

        // clang-format off
        vertices.insert(vertices.end(), 
        {
            xpos + w, ypos + h,  ch.rectRight, ch.top,        // top right
            xpos + w, ypos,      ch.rectRight, ch.rectBottom, // bottom right
            xpos,     ypos,      ch.left,     ch.rectBottom, // bottom left
            xpos,    ypos + h, ch.left,     ch.top         // top left
          });

        indices.insert(indices.end(), {
          0 + offset, 1 + offset, 3 + offset,
          1 + offset, 2 + offset, 3 + offset});
        // clang-format on

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        ++characterIdx;
      }
    }

#ifdef EDITOR
    void PopulateVerticesIndices(Scene::Data& scene, Data& textBox)
#else
    void PopulateVerticesIndices(Scene::Data&, Data& textBox)
#endif
    {
      textBox.size = {};
#ifdef EDITOR
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      if (!textBox.hasParent)
      {
        hoverable.offset.y = offsetY;
      }
#endif
      auto& vertices = textBox.vertices;
      auto& indices = textBox.indices;

      FreeContainer(vertices);
      FreeContainer(indices);

      vertices.reserve(textBox.text.length() * 16);
      indices.reserve(textBox.text.length() * 6);

      float x = 0;
      float y = 0;
      int characterIdx = 0;
      int characterCount = 0;

      auto splitStrings = SplitString(Trim(textBox.text), " ");

      for (size_t i = 0; i < splitStrings.size(); ++i)
      {
        auto string = splitStrings[i];
        characterCount += (int)string.size();
        // iterate through all characters
        if (!textBox.singleCharacterWriteMode && characterCount > textBox.maxCharactersPerLine)
        {
          MoveCursor(x, y, characterCount);
          characterCount = (int)string.size();
#ifdef EDITOR
          if (!textBox.hasParent)
          {
            hoverable.offset.y -= 150;
          }
#endif
          textBox.size.y += 150;
        }
        // clang-format off
        PopulateVerticesIndices(textBox, string, vertices, indices, x, y, characterIdx, characterCount);
        if (i < splitStrings.size() - 1)
        {
          PopulateVerticesIndices(textBox, " ", vertices, indices, x, y, characterIdx, characterCount);
          characterCount += 1;
        }
        // clang-format on
      }
    }

#ifdef EDITOR
    void Drag(Scene::Data& scene, Component::Hoverable::Data& hoverable, float x, float y)
    {
      auto& textBox = *static_cast<TextBox::Data*>(hoverable.callbackData);
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
      auto& position = Scene::Get<Temp::Component::Type::POSITION2D>(scene, textBox.entity);
      position.x = x;
      position.y = y;
      Component::Drawable::SetTranslate(drawable, {x, y, 0});
      hoverable.model = drawable.model;
    }

    std::vector<std::vector<Math::Vec3f>> Triangles(Data& textBox)
    {
      std::vector<std::vector<Math::Vec3f>> out;
      const auto& vertices = textBox.vertices;
      for (size_t i = 0; i < textBox.vertices.size(); i += 16)
      {
        Math::Vec3f v0{vertices[0 + i], vertices[1 + i], 0.f};
        Math::Vec3f v1{vertices[4 + i], vertices[5 + i], 0.f};
        Math::Vec3f v2{vertices[8 + i], vertices[9 + i], 0.f};
        Math::Vec3f v3{vertices[12 + i], vertices[13 + i], 0.f};

        out.emplace_back(std::vector{v0, v1, v3});
        out.emplace_back(std::vector{v1, v2, v3});
      }
      return out;
    }
#endif
  }

  void DrawConstruct(Scene::Data& scene, Data& textBox, int shaderType)
  {
    using namespace Temp::Render;

    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
    drawable.texture = Font::Characters(textBox.fontType).at('0').texture;
#ifdef DEBUG
    PopulateVerticesIndices(scene, textBox);
#endif
#ifdef EDITOR
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      hoverable.triangles = Triangles(textBox);
      hoverable.width = textBox.size.x;
      hoverable.height = textBox.size.y;
      Component::Hoverable::ConstructDrawable(hoverable);
    }
#endif
    Component::Drawable::UpdateData(drawable, textBox.vertices, textBox.indices);

    Component::Drawable::ConstructFont(drawable, shaderType);
    OpenGLWrapper::UnbindBuffers();
  }

  void Construct(Scene::Data& scene,
                 Data& textBox,
                 const ConstructData& ctorData,
                 Entity::id entity)
  {
    textBox.entity = entity;
    Component::Drawable::Data drawable;
    drawable.entity = textBox.entity;
    drawable.offset = {0, ctorData.offsetY, 0};
#ifdef EDITOR
    // Hacked check to make sure textBox doesn't use hoverable when underneath textButton
    if (!textBox.hasParent)
    {
      Component::Hoverable::Data hoverable{};
      hoverable.scale = {ctorData.scale, ctorData.scale};
      hoverable.offset = {0, offsetY};
      hoverable.x = ctorData.x;
      hoverable.y = ctorData.y;
      hoverable.Drag = Drag;
      hoverable.isDrag = true;
      hoverable.model = drawable.model;
      hoverable.callbackData = &textBox;
      Scene::AddComponent<Component::Type::HOVERABLE>(scene, textBox.entity, hoverable);
    }
#endif
#ifndef DEBUG
    try
    {
      PopulateVerticesIndices(scene, textBox);
#ifdef EDITOR
      if (!textBox.hasParent)
      {
        auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
        hoverable.triangles = Triangles(textBox);
      }
#endif
    }
    catch (std::exception&)
    {
      Logger::LogErr("[TextBox] Could not create TextBox. Possibly could not load font.");
    }
#endif
    textBox.renderText = false;
    textBox.enableOutline = false;
    Component::Drawable::SetTranslate(drawable, {ctorData.x, ctorData.y, 0});
    Component::Drawable::SetScale(drawable, ctorData.scale);

    Scene::AddComponent<Component::Type::DRAWABLE>(scene, textBox.entity, drawable);
    Scene::AddComponent<Component::Type::POSITION2D>(scene,
                                                     textBox.entity,
                                                     {ctorData.x, ctorData.y});
    Scene::AddComponent<Component::Type::SCALE>(scene,
                                                textBox.entity,
                                                {ctorData.scale, ctorData.scale});
    Scene::AddComponent<Component::Type::TEXT>(scene, textBox.entity, textBox.text);
  }

  void UpdateText(Scene::Data& scene, Data& textBox, const std::string& newText)
  {
    Scene::Get<Temp::Component::Type::TEXT>(scene, textBox.entity) = newText;
    textBox.text = newText;
    Update(scene, textBox);
  }

  void Update(Scene::Data& scene, Data& textBox)
  {
    PopulateVerticesIndices(scene, textBox);
#ifdef EDITOR
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      hoverable.width = textBox.size.x;
      hoverable.height = textBox.size.y;
      hoverable.triangles = Triangles(textBox);
    }
#endif
    textBox.renderText = true;
  }

#ifdef EDITOR
  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& textBox)
#else
  void DrawUpdate(Scene::Data&, Component::Drawable::Data& drawable, Data& textBox)
#endif
  {
    using namespace Temp::Render;

    if (!textBox.renderText)
    {
      return;
    }

    Component::Drawable::UpdateData(drawable, textBox.vertices, textBox.indices);
    // drawable.disableDepth = true;

    // NOTE: This is referring to updating the drawable buffers manually using the OpenGLWrapper
    // Don't know why this won't update properly if I pass textBox vertices and indices directly
    // Removing std::move from DrawConstruct doesn't work...
    Temp::Component::Drawable::UpdateVertexIndexBuffers(drawable, GL_DYNAMIC_DRAW);
    textBox.renderText = false;
    // drawable.disableDepth = false;

#ifdef EDITOR
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      Component::Hoverable::UpdateDrawable(hoverable);
    }
#endif
  }

  struct EnableOutlineData
  {
    Data* textBox;
    bool enable{false};
  };

  void RenderFunction(Scene::Data& scene, void* renderData)
  {
    auto* enableOutlineData = static_cast<EnableOutlineData*>(renderData);
    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(
      scene,
      enableOutlineData->textBox->entity);
    Render::OpenGLWrapper::Set1BoolShaderProperty(drawable.shaderProgram,
                                                  "u_useOutline",
                                                  enableOutlineData->enable);
    Render::OpenGLWrapper::Set1FloatShaderProperty(drawable.shaderProgram,
                                                   "u_thickness",
                                                   enableOutlineData->enable ? 0.75f : 0.5f);
    // Render::OpenGLWrapper::Set1FloatShaderProperty(drawable.shaderProgram, "u_outline_thickness",
    // enableOutlineData->enable ? 0.9 : 0.5);
    delete enableOutlineData;
  }

  void EnableOutline(Scene::Data& scene, Data& textBox, bool enable)
  {
    Scene::EnqueueRender(scene, RenderFunction, new EnableOutlineData{&textBox, enable});
  }

#ifdef EDITOR
  void DrawDestruct(Scene::Data& scene, Data& textBox)
  {
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      Component::Hoverable::DestructDrawable(hoverable);
    }
  }
#else
  void DrawDestruct(Scene::Data&, Data&) {}
#endif

  void Destruct(Scene::Data&, Data& textBox)
  {
    FreeContainer(textBox.vertices);
    FreeContainer(textBox.indices);
  }

  std::string FormatText(Data& textBox, std::string_view text, std::string_view sectionBreak)
  {
    std::string copy{};
    auto splitText = SplitString(text, " ");
    int characterCount = 0;
    for (size_t i = 0; i < splitText.size(); ++i)
    {
      bool isSectionBreak = splitText[i].find(sectionBreak) != std::string::npos;
      bool isNewLine = splitText[i].find("\n") != std::string::npos;
      if (isSectionBreak || isNewLine)
      {
        std::vector<std::string_view> splitText2;
        if (isSectionBreak)
        {
          splitText2 = SplitString(splitText[i], sectionBreak.data());
        }
        else if (isNewLine)
        {
          splitText2 = SplitString(splitText[i], "\n");
        }
        copy += splitText[i];
        copy += " ";
        characterCount = (int)splitText2.back().size() + 1;
      }
      else
      {
        characterCount += (int)splitText[i].size();
        if ((characterCount + 1) > textBox.maxCharactersPerLine)
        {
          copy += "\n";
          characterCount = (int)splitText[i].size();
        }
        copy += splitText[i];
        copy += " ";
        ++characterCount;
      }
    }
    return copy;
  }
}
