// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "TextBox.hpp"

#include "Array_fwd.hpp"
#include "Drawable.hpp"
#include "Engine.hpp"
#include "EngineUtils.hpp"
#include "FontLoader.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "Math_fwd.hpp"
#include "Math_impl.hpp"
#include "MemoryManager.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include <string>
#ifdef EDITOR
#include "Hoverable.hpp"
#endif

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
                                 const String& string,
                                 DynamicArray<float, MemoryManager::Data::Type::SCENE_ARENA>& vertices,
                                 DynamicArray<unsigned int, MemoryManager::Data::Type::SCENE_ARENA>& indices,
                                 float& x,
                                 float& y,
                                 int& characterIdx,
                                 int& characterCount)
    {
      // iterate through all characters
      for (unsigned int i = 0; i < string.size; ++i)
      {
        char c = string[i];
        // Should only be possible at runtime.
        // Editor shouldn't support this.
        if (c == '\n')
        {
          MoveCursor(x, y, characterCount);
          characterCount = (int)string.size - i + 1;
          continue;
        }
        Font::Character ch;
        try
        {
          ch = Font::Characters(textBox.fontType)[c];
        }
        catch (std::exception&)
        {
          Logger::LogErr(String("[TextBox] Could not parse: ") + c);
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
        vertices.InsertEnd(
        {
            xpos + w, ypos + h,  ch.rectRight, ch.top,        // top right
            xpos + w, ypos,      ch.rectRight, ch.rectBottom, // bottom right
            xpos,     ypos,      ch.left,     ch.rectBottom, // bottom left
            xpos,    ypos + h, ch.left,     ch.top         // top left
          });

        indices.InsertEnd({
          0 + offset, 1 + offset, 3 + offset,
          1 + offset, 2 + offset, 3 + offset});
        // clang-format on

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        ++characterIdx;
      }
    }

    void PopulateVerticesIndices(Scene::Data& scene, Data& textBox)
    {
      textBox.size = {};
#ifdef EDITOR
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      if (!textBox.hasParent)
      {
        hoverable.offset.y = offsetY;
      }
#endif
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
      auto& vertices = drawable.vertices;
      auto& indices = drawable.indices;

      vertices.Clear();
      indices.Clear();

      float x = 0;
      float y = 0;
      int characterIdx = 0;
      int characterCount = 0;

      String textTempStr = textBox.text.c_str();
      auto splitStrings = SplitString(Trim(textTempStr), " ");

      for (size_t i = 0; i < splitStrings.size; ++i)
      {
        auto string = splitStrings[i];
        characterCount += (int)string.size;
        // iterate through all characters
        if (!textBox.singleCharacterWriteMode && characterCount > textBox.maxCharactersPerLine)
        {
          MoveCursor(x, y, characterCount);
          characterCount = (int)string.size;
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
        if (i < splitStrings.size - 1)
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

    void InitTriangles(Scene::Data& scene, Data& textBox)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
      const auto& vertices = drawable.vertices;
      size_t oldSize = hoverable.triangles.size;
      size_t newCapacity = Math::Ceil(vertices.size / 16.f) * 2.f;
      if (oldSize >= newCapacity)
      {
        return;
      }

      for (size_t i = oldSize; i < newCapacity; ++i)
      {
        hoverable.triangles.PushBack(
          DynamicArray<Math::Vec3f, MemoryManager::Data::SCENE_ARENA>(true));
        for (int i = 0; i < 3; ++i)
        {
          hoverable.triangles.back().PushBack({});
        }
      }
    }

    void Triangles(Scene::Data& scene, Data& textBox)
    {
      InitTriangles(scene, textBox);
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
      const auto& vertices = drawable.vertices;
      size_t currIdx = 0;
      for (size_t i = 0; i < vertices.size; i += 16, currIdx += 2)
      {
        Math::Vec3f v0{vertices[0 + i], vertices[1 + i], 0.f};
        Math::Vec3f v1{vertices[4 + i], vertices[5 + i], 0.f};
        Math::Vec3f v2{vertices[8 + i], vertices[9 + i], 0.f};
        Math::Vec3f v3{vertices[12 + i], vertices[13 + i], 0.f};

        // hoverable.triangles.InsertEnd({{v0, v1, v3}, {v1, v2, v3}});
        hoverable.triangles[currIdx][0] = v0;
        hoverable.triangles[currIdx][1] = v1;
        hoverable.triangles[currIdx][2] = v3;
        hoverable.triangles[currIdx+1][0] = v1;
        hoverable.triangles[currIdx+1][1] = v2;
        hoverable.triangles[currIdx+1][2] = v3;
      }
    }
#endif
  }

  void DrawConstruct(Scene::Data& scene, Data& textBox, int shaderType)
  {
    using namespace Temp::Render;

    auto& drawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
    drawable.texture = Font::Characters(textBox.fontType)['0'].texture;
#ifdef DEBUG
    PopulateVerticesIndices(scene, textBox);
#endif
#ifdef EDITOR
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      Triangles(scene, textBox);
      hoverable.width = textBox.size.x;
      hoverable.height = textBox.size.y;
      Component::Hoverable::ConstructDrawable(hoverable);
    }
#endif
    Component::Drawable::UpdateData(drawable);

    Component::Drawable::ConstructFont(drawable, shaderType);
    OpenGLWrapper::UnbindBuffers();
  }

  void Construct(Scene::Data& scene,
                 Data& textBox,
                 const ConstructData& ctorData,
                 Entity::id entity)
  {
    textBox.entity = entity;
    textBox.maxCharacters = ctorData.maxCharacters;
    Component::Drawable::Data drawable;
    drawable.entity = textBox.entity;
    drawable.offset = {0, ctorData.offsetY, 0};

    textBox.enableOutline = false;
    Component::Drawable::SetTranslate(drawable, {ctorData.x, ctorData.y, 0});
    Component::Drawable::SetScale(drawable, ctorData.scale);

    Scene::AddComponent<Component::Type::DRAWABLE>(scene, textBox.entity, std::move(drawable));
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
        hoverable.triangles = Triangles(scene, textBox);
      }
#endif
    }
    catch (std::exception&)
    {
      Logger::LogErr("[TextBox] Could not create TextBox. Possibly could not load font.");
    }
#endif

    Scene::AddComponent<Component::Type::POSITION2D>(scene,
                                                     textBox.entity,
                                                     {ctorData.x, ctorData.y});
    Scene::AddComponent<Component::Type::SCALE>(scene,
                                                textBox.entity,
                                                {ctorData.scale, ctorData.scale});
    Scene::AddComponent<Component::Type::TEXT>(scene, textBox.entity, textBox.text.c_str());

    auto& addedDrawable = Scene::Get<Temp::Component::Type::DRAWABLE>(scene, textBox.entity);
    if (textBox.maxCharacters != SIZE_MAX)
    {
      addedDrawable.vertices.Reserve(textBox.maxCharacters * 16 * 1000);
      addedDrawable.indices.Reserve(textBox.maxCharacters * 6 * 1000);
    }
  }

  void UpdateText(Scene::Data& scene, Data& textBox, const char* newText)
  {
    // Do this to avoid unnecessary allocations
    Scene::Get<Temp::Component::Type::TEXT>(scene, textBox.entity).Replace(newText);
    textBox.text.Replace(newText);
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
      Triangles(scene, textBox);
    }
#endif
  }

#ifdef EDITOR
  void DrawUpdate(Scene::Data& scene, Component::Drawable::Data& drawable, Data& textBox)
#else
  void DrawUpdate(Scene::Data&, Component::Drawable::Data& drawable, Data& textBox)
#endif
  {
    using namespace Temp::Render;

    Component::Drawable::UpdateData(drawable);
    Render::OpenGLWrapper::Set1BoolShaderProperty(drawable.shaderProgram,
                                                  "u_useOutline",
                                                  textBox.enableOutline);
    Render::OpenGLWrapper::Set1FloatShaderProperty(drawable.shaderProgram,
                                                   "u_thickness",
                                                   textBox.enableOutline ? 0.75f : 0.5f);
    // drawable.disableDepth = true;

    // NOTE: This is referring to updating the drawable buffers manually using the OpenGLWrapper
    // Don't know why this won't update properly if I pass textBox vertices and indices directly
    // Removing std::move from DrawConstruct doesn't work...
    Temp::Component::Drawable::UpdateVertexIndexBuffers(drawable, GL_DYNAMIC_DRAW);
    // drawable.disableDepth = false;

#ifdef EDITOR
    if (!textBox.hasParent)
    {
      auto& hoverable = Scene::Get<Temp::Component::Type::HOVERABLE>(scene, textBox.entity);
      Component::Hoverable::UpdateDrawable(hoverable);
    }
#endif
  }

  void EnableOutline(Scene::Data& scene, Data& textBox, bool enable)
  {
    textBox.enableOutline = enable;
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

  void Destruct(Scene::Data&, Data& /*textBox*/)
  {
    // FreeContainer(textBox.vertices);
    // FreeContainer(textBox.indices);
  }

  std::string FormatText(Data& textBox, std::string_view text, std::string_view sectionBreak)
  {
    std::string copy{};
    String tempStr = std::string(text).c_str();
    auto splitText = SplitString(tempStr, " ");
    int characterCount = 0;
    for (size_t i = 0; i < splitText.size; ++i)
    {
      bool isSectionBreak = splitText[i].find(std::string(sectionBreak).c_str()) != std::string::npos;
      bool isNewLine = splitText[i].find("\n") != std::string::npos;
      if (isSectionBreak || isNewLine)
      {
        DynamicArray<String> splitText2;
        if (isSectionBreak)
        {
          splitText2 = SplitString(splitText[i], sectionBreak.data());
        }
        else if (isNewLine)
        {
          splitText2 = SplitString(splitText[i], "\n");
        }
        copy += splitText[i].c_str();
        copy += " ";
        characterCount = (int)splitText2.back().size + 1;
      }
      else
      {
        characterCount += (int)splitText[i].size;
        if ((characterCount + 1) > textBox.maxCharactersPerLine)
        {
          copy += "\n";
          characterCount = (int)splitText[i].size;
        }
        if (splitText[i].c_str())
        {
          copy += splitText[i].c_str();
        }
        copy += " ";
        ++characterCount;
      }
    }
    return copy;
  }
}
