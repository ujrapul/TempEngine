// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Editor.hpp"
#include "ComponentType.hpp"
#include "Engine.hpp"
#include "EntityType.hpp"
#include "Event.hpp"
#include "FontLoader.hpp"
#include "Hoverable.hpp"
#include "LevelSerializer.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "MemoryManager.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "TextBox.hpp"
#include "TextButton.hpp"
#include "imgui.h"

#include "GameEditor.hpp"

namespace Temp::Editor
{
  namespace
  {
    void DestructDrawable(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      auto& drawable = Scene::Get<Component::Type::DRAWABLE>(scene, selectedObject.entity);
      Component::Drawable::Destruct(drawable);
      SceneObject::DrawDestruct(scene, selectedObject);
    }

    void PropertiesName(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      char name[4096];
      strcpy(name, selectedObject.name.buffer);
      name[selectedObject.name.size] = '\0';

      ImGui::InputText("Name", name, 4096, ImGuiInputTextFlags_None);
      if (Scene::ValidateObjectName(scene, name))
      {
        Scene::UpdateObjectName(scene, selectedObject, name);
      }
    }

    void PropertiesFile(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      String file(true, 256);
      String fileStr;
      switch (selectedObject.type)
      {
        case EntityType::SPRITE:
        {
          fileStr = static_cast<Sprite::Data*>(selectedObject.data)->fileName.c_str();
        }
        default:
          break;
      };
      strcpy(file.buffer, fileStr.c_str());
      file[fileStr.size] = '\0';

      if (ImGui::InputText("File", file.buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
      {
        // Logger::LogErr(String("Current stack usage: ") + String::ToString(get_stack_usage()));
        DestructDrawable(scene, selectedObject);
        switch (selectedObject.type)
        {
          case EntityType::SPRITE:
            static_cast<Sprite::Data*>(selectedObject.data)->fileName = file.c_str();
          default:
            break;
        };
        SceneObject::DrawConstruct(scene, selectedObject);
      }
    }

    void PropertiesShader(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      char shader[4096];
      String shaderFile(selectedObject.shaderType == -1
                          ? "[DEFAULT]"
                          : Render::ShaderFiles()[selectedObject.shaderType]);
      strcpy(shader, shaderFile.c_str());
      shader[shaderFile.size] = '\0';
      String originalShader(shader);

      if (ImGui::InputText("Shader", shader, 4096, ImGuiInputTextFlags_EnterReturnsTrue))
      {
        String shaderStr(shader);
        if (originalShader != shaderStr)
        {
          selectedObject.shaderType = -1;
          DestructDrawable(scene, selectedObject);
          for (size_t i = 0; i < Render::ShaderFiles().size; ++i)
          {
            if (shaderStr == Render::ShaderFiles()[i])
            {
              selectedObject.shaderType = (int)i;
              break;
            }
          }
          SceneObject::DrawConstruct(scene, selectedObject);
        }
      }
    }

    void PropertiesPosition(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      auto& position = Scene::Get<Component::Type::POSITION2D>(scene, selectedObject.entity);
      if (ImGui::DragFloat2("Position",
                        position.data,
                        0.5f,
                        -FLT_MAX,
                        FLT_MAX,
                        "%.3f",
                        ImGuiSliderFlags_None))
      {
        SceneObject::Translate(scene, selectedObject, {position.x, position.y, 0});
      }
    }

    void PropertiesText(TextBox::Data& textBox)
    {
      char text[4096];
      strcpy(text, textBox.text.c_str());

      ImGui::InputText("Text", text, 4096, ImGuiInputTextFlags_None);
      textBox.text.Replace(text);
    }

    void PropertiesScale(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      if (selectedObject.type >= EntityType::MAX)
      {
        return;
      }

      bool changed = false;
      auto& scale = Scene::Get<Component::Type::SCALE>(scene, selectedObject.entity);
      switch ((EntityType::EntityType)selectedObject.type)
      {
        case EntityType::SPRITE:
          changed = ImGui::DragFloat2("Scale", //
                            scale.data,
                            0.005f,
                            -FLT_MAX,
                            FLT_MAX,
                            "%.3f",
                            ImGuiSliderFlags_None);
          break;
        default:
          changed = ImGui::DragFloat("Scale", //
                           &scale.x,
                           0.005f,
                           -FLT_MAX,
                           FLT_MAX,
                           "%.3f",
                           ImGuiSliderFlags_None);
          break;
      }

      if (changed)
      {
        switch ((EntityType::EntityType)selectedObject.type)
        {
          case EntityType::TEXTBUTTON:
            TextButton::Scale(scene, *static_cast<TextButton::Data*>(selectedObject.data), scale.x);
            break;
          case EntityType::TEXTBOX:
            SceneObject::Scale(scene, selectedObject, scale.x);
            break;
          case EntityType::SPRITE:
            SceneObject::Scale(scene, selectedObject, {scale.x, scale.y});
            break;
          default:
            SceneObject::Scale(scene, selectedObject, scale.x);
            break;
        }
      }
    }

    void PropertiesFont(Scene::Data& scene, TextBox::Data& textBox)
    {
      auto& items = Font::Fonts;
      int item_current_idx = textBox.fontType; // Here we store our selection data as an index.
      if (ImGui::BeginCombo("Font", items[item_current_idx]))
      {
        for (int n = 0; n < Font::Type::MAX; n++)
        {
          const bool is_selected = (item_current_idx == n);
          if (ImGui::Selectable(items[n], is_selected))
          {
            item_current_idx = n;
            textBox.fontType = n;
            TextBox::DrawDestruct(scene, textBox);
            TextBox::DrawConstruct(scene, textBox);
          }

          // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }

    void PropertiesHoverable(Scene::Data& scene, SceneObject::Data& selectedObject)
    {
      if (!Test(Scene::ComponentBits(scene, selectedObject.entity), Component::Type::HOVERABLE))
      {
        return;
      }
      ImGui::Text("Hoverable");
      auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(scene, selectedObject.entity);
      float size[2] = {hoverable.width, hoverable.height};
      ImGui::DragFloat2("Size", //
                        size,
                        0.5f,
                        -FLT_MAX,
                        FLT_MAX,
                        "%.3f",
                        ImGuiSliderFlags_None);

      hoverable.width = size[0];
      hoverable.height = size[1];
    }

    void PropertiesTextBox(Scene::Data& scene, TextBox::Data& textBox)
    {
      ImGui::Text("TextBox");
      int charactersPerLine = textBox.maxCharactersPerLine;
      ImGui::DragInt("Characters\nPer Line", //
                     &charactersPerLine,
                     1.f,
                     0,
                     INT_MAX);
      if (textBox.maxCharactersPerLine != charactersPerLine)
      {
        textBox.maxCharactersPerLine = charactersPerLine;
        TextBox::Update(scene, textBox);
      }
    }
  }

  void RunImGui(Scene::Data& scene, Event::Data& EventData)
  {
    static float x = 0;
    static float y = 0;

    static bool loadSucceeded = false;
    static float loadSucceededTime = 0;

    static bool saveSucceeded = false;
    static float saveSucceededTime = 0;

    // Add your ImGui UI code here
    bool show_demo_window;
    ImGui::ShowDemoWindow(&show_demo_window);

    {
      static std::array<char, 4096> saveText;
      static std::array<char, 4096> loadText;

      ImGui::SetNextWindowSize({250, 250});
      ImGui::SetNextWindowPos({0, 0});
      ImGui::Begin("Menu");

      bool save = ImGui::Button("Save");
      ImGui::SameLine(50);
      ImGui::InputTextWithHint("##SaveInput", "", saveText.data(), 4096);

      bool load = ImGui::Button("Load");
      ImGui::SameLine(50);
      ImGui::InputTextWithHint("##LoadInput", "", loadText.data(), 4096);
      if (saveSucceeded)
      {
        ImGui::Spacing();
        ImGui::Text(
          "%s",
          (String("Successfully saved ") + String(scene.sceneFns->name.c_str()) + String(".level"))
            .c_str());
        saveSucceededTime += Global::DeltaTime();
        if (saveSucceededTime > 2.f)
        {
          saveSucceeded = false;
          saveSucceededTime = 0;
        }
      }
      if (loadSucceeded)
      {
        ImGui::Spacing();
        ImGui::Text(
          "%s",
          (String("Successfully loaded ") + String(scene.sceneFns->name.c_str()) + String(".level"))
            .c_str());
        loadSucceededTime += Global::DeltaTime();
        if (loadSucceededTime > 2.f)
        {
          loadSucceeded = false;
          loadSucceededTime = 0;
        }
      }

      if (save)
      {
        loadSucceeded = false;
        loadSucceededTime = 0;
        scene.sceneFns->name = saveText.data();
        LevelSerializer::Serialize(
          scene,
          (AssetsDirectory() / "Levels" / (String(scene.sceneFns->name.c_str()) + ".level").c_str()).buffer.c_str());
        saveSucceeded = true;
      }
      else if (load)
      {
        scene.sceneFns->name.Replace(loadText.data());
        if (LevelSerializer::LevelExists((String(scene.sceneFns->name.c_str()) + ".level").c_str()))
        {
          EventData.draggable = nullptr;
          EventData.selectedObject = nullptr;
          Scene::DrawDestruct(scene);
          Scene::Destruct(scene);
          if (LevelSerializer::Deserialize(scene, (String(scene.sceneFns->name.c_str()) + ".level").c_str()))
          {
            Scene::Construct(scene, false);
            Scene::DrawConstruct(scene);
            loadSucceeded = true;
            loadSucceededTime = 0;
            saveSucceeded = false;
            saveSucceededTime = 0;
          }
        }
      }

      ImGui::End();
    }

    ImGui::SetNextWindowSize({250, 250});
    ImGui::SetNextWindowPos({0, 250});
    ImGui::Begin("Create Entity");
    if (ImGui::Button("TextBox"))
    {
      auto* ctorData = new TextBox::ConstructData{x, y, 0.5f};
      auto* textBox = new TextBox::Data{.text = "Default Text"};
      SceneObject::Data object{
        textBox,
        ctorData,
        "TextObject",
        {},
        EntityType::TEXTBOX,
      };
      Global::SpawnObject(object);

      x += 10;
      y -= 10;
    }
    if (ImGui::Button("TextButton"))
    {
      auto hoverable = Component::Hoverable::Data{};
      hoverable.x = x;
      hoverable.y = y;
      hoverable.width = 410 * 2;
      hoverable.height = 56 * 2;
      hoverable.scale = {0.5f, 0.5f};
      float scale = 0.5f;
      auto* ctorData = new TextButton::ConstructData{hoverable, {x, y, scale}};

      auto* textButton = new TextButton::Data{TextBox::Data{.text = "Default Button"}};
      SceneObject::Data object{
        textButton,
        ctorData,
        "TextButtonObject",
        {},
        EntityType::TEXTBUTTON,
      };
      Global::SpawnObject(object);

      x += 10;
      y -= 10;
    }
    if (ImGui::Button("Sprite"))
    {
      auto* ctorData = MemoryManager::CreateScene<Sprite::ConstructData>(
        Math::Vec2f{-256.f, -256.f},
        Math::Vec2f{512.f, 512.f}
      );

      auto* sprite = MemoryManager::CreateScene<Sprite::Data>(
        "ground.tga"
      );
      SceneObject::Data object{
        sprite,
        ctorData,
        "SpriteObject",
        {},
        EntityType::SPRITE,
      };
      Global::SpawnObject(object);

      x += 10;
      y -= 10;
    }
    ExtensionImGuiCreateEntity(scene);
    ImGui::End();

    ImGui::SetNextWindowSize({250, 250});
    ImGui::SetNextWindowPos({0, 500});
    ImGui::Begin("Properties");
    if (EventData.selectedObject && !Global::IsSpawning())
    {
      PropertiesName(scene, *EventData.selectedObject);
      PropertiesShader(scene, *EventData.selectedObject);
      PropertiesScale(scene, *EventData.selectedObject);
      PropertiesPosition(scene, *EventData.selectedObject);
      switch (EventData.selectedObject->type)
      {
        case EntityType::TEXTBOX:
        {
          auto* textBox = static_cast<TextBox::Data*>(EventData.selectedObject->data);
          PropertiesText(*textBox);
          String temp(textBox->text.c_str());
          TextBox::UpdateText(scene, *textBox, temp.c_str());
          PropertiesTextBox(scene, *textBox);
          PropertiesFont(scene, *textBox);
        }
        break;
        case EntityType::TEXTBUTTON:
        {
          auto* textButton = static_cast<TextButton::Data*>(EventData.selectedObject->data);
          PropertiesText(textButton->textBox);
          String temp(textButton->textBox.text.c_str());
          TextButton::UpdateText(scene, *textButton, temp.c_str());
          PropertiesTextBox(scene, textButton->textBox);
          PropertiesFont(scene, textButton->textBox);
        }
        break;
        case EntityType::SPRITE:
        {
          PropertiesFile(scene, *EventData.selectedObject);
        }
        break;
        case EntityType::MOCK:
          break;
        case EntityType::MAX:
          break;
        default:
          break;
      }
      ExtensionImGuiProperties(scene, EventData.selectedObject);
      PropertiesHoverable(scene, *EventData.selectedObject);
    }
    ImGui::End();

    ImGui::SetNextWindowSize({250, 250});
    ImGui::SetNextWindowPos({0, 750});
    ImGui::Begin("Object List");
    for (auto& object : scene.objects)
    {
      ImGuiTreeNodeFlags ImGuiTreeNodeFlags = ImGuiTreeNodeFlags_Leaf;
      if (EventData.selectedObject == &object)
      {
        ImGuiTreeNodeFlags |= ImGuiTreeNodeFlags_Selected;
      }
      ImGui::TreeNodeEx((void*)(intptr_t)object.entity,
                        ImGuiTreeNodeFlags,
                        "%s",
                        object.name.c_str());
      if (ImGui::IsItemClicked())
      {
        EventData.selectedObject = &object;
      }
      ImGui::TreePop();
    }
    ImGui::End();
  }
}
