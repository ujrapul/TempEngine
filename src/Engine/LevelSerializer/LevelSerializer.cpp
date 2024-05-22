// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "LevelSerializer.hpp"
#include "EntityType.hpp"
#include "HashMap.hpp"
#include "Logger.hpp"
#include "MemoryManager.hpp"
#include "Shader.hpp"
#include "TextBox.hpp"
#include "TextButton.hpp"

#include "GameLevelSerializer.hpp"
#include <string>

// clang-format off
namespace Temp::LevelSerializer
{
  namespace
  {
    /////////////////////////////////////////////////////////////////////////////////////////
    /// DESERIALIZATION /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////

    void SetObject(SceneObject::Data& object, std::tuple<DeserializeData, bool>& data)
    {
      const auto& dataGet = std::get<0>(data);
      object.name = dataGet.get<Type::NAME>().c_str();
      object.shaderType = dataGet.get<Type::SHADER>();
#ifdef EDITOR
      if (object.shaderType >= 0)
        object.shaderType += Render::EditorShaderIdx::MAX;
#endif
    }

    bool DeserializeSprite(GlobalDeserializeData& data)
    {
      auto out = Deserialize(data);
      if (!std::get<1>(out))
      {
        Logger::LogErr(String("[Deserialize] Invalid Sprite at line ") + String::ToString(data.lineNumber) + ": " +
                       data.line.c_str());
        return false;
      }
      SceneObject::Data object;
      auto* sprite = MemoryManager::CreateScene<Sprite::Data>();
      auto* spriteCtor = MemoryManager::CreateScene<Sprite::ConstructData>();
      const auto& spriteData = std::get<0>(out);
      SetObject(object, out);
      sprite->fileName = spriteData.get<Type::FILE>().c_str();
      spriteCtor->pos = spriteData.get<Type::POSITION>();
      spriteCtor->scale = spriteData.get<Type::SCALE2D>();
      object.constructData = spriteCtor;

      object.data = sprite;
      AddSceneObject(std::move(object), EntityType::SPRITE, data);
      return true;
    }

    bool DeserializeTextBox(GlobalDeserializeData& data)
    {
      TextBox::Data* textBox;
      TextBox::ConstructData* ctorTextBox;
      auto out = Deserialize(data, textBox, ctorTextBox);
      if (!std::get<1>(out))
      {
        Logger::LogErr(String("[Deserialize] Invalid TextBox at line ") + String::ToString(data.lineNumber) + ": " +
                       data.line.c_str());
        return false;
      }

      SceneObject::Data object;
      // const auto& textBoxData = std::get<0>(out);
      SetObject(object, out);
      object.data = textBox;
      object.constructData = ctorTextBox;
      AddSceneObject(std::move(object), EntityType::TEXTBOX, data);
      return true;
    }

    bool DeserializeTextButton(GlobalDeserializeData& data)
    {
      auto out = Deserialize(data);
      if (!std::get<1>(out))
      {
        Logger::LogErr(String("[Deserialize] Invalid TextButton at line ") + String::ToString(data.lineNumber) + ": " +
                       data.line.c_str());
        return false;
      }
      SceneObject::Data object;
      auto* textButton = MemoryManager::CreateScene<TextButton::Data>();
      auto* textButtonCtor = MemoryManager::CreateScene<TextButton::ConstructData>();
      const auto& textButtonData = std::get<0>(out);
      SetObject(object, out);
      textButton->textBox = textButtonData.get<Type::TEXTBOX>();
      textButtonCtor->hoverable = textButtonData.get<Type::HOVERABLE>();
      textButtonCtor->textBoxCtorData = textButtonData.get<Type::TEXTBOXCTOR>();
      object.constructData = static_cast<void*>(textButtonCtor);

      object.data = textButton;
      AddSceneObject(std::move(object), EntityType::TEXTBUTTON, data);
      return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    /// SERIALIZATION /////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    void SerializeObject(const SceneObject::Data& object,
                         GlobalSerializeData& data,
                         const std::string& indent = "")
    {
      data.f << indent << "  " << SerializeString<Type::NAME>() << ": " << object.name << "\n"
#ifdef EDITOR
             << indent << "  " << SerializeString<Type::SHADER>() << ": "
             << ((object.shaderType == -1) ? object.shaderType
                                           : object.shaderType - Render::EditorShaderIdx::MAX)
#else
             << indent << "  " << SerializeString<Type::SHADER>() << ": " << object.shaderType
#endif
             << "\n";
    }

    void SerializeTextBox(const SceneObject::Data& object,
                          const TextBox::Data* textBox,
                          GlobalSerializeData& data,
                          const std::string& indent = "")
    {
      const auto& position = Scene::Get<Component::Type::POSITION2D>(data.scene, textBox->entity);
      const auto& scale = Scene::Get<Component::Type::SCALE>(data.scene, textBox->entity);
      data.f << indent << "TextBox\n" << indent << "{\n";
      SerializeObject(object, data, indent);
      data.f << indent << "  " << SerializeString<Type::POSITION>() << ": " << position.x << " "
             << position.y << "\n"
             << indent << "  " << SerializeString<Type::SCALE>() << ": " << scale.x << "\n"
             << indent << "  " << SerializeString<Type::TEXT>() << ": " << textBox->text << "\n"
             << indent << "  " << SerializeString<Type::FONT>() << ": " << textBox->fontType << "\n"
             << indent << "  " << SerializeString<Type::CHARACTERLIMIT>() << ": "
             << textBox->maxCharactersPerLine << "\n"
             << indent << "}\n";
    }

    void SerializeHoverable(const Component::Hoverable::Data* hoverable,
                            GlobalSerializeData& data,
                            const std::string& indent = "")
    {
      data.f << indent << "Hoverable\n"
             << indent << "{\n"
             << indent << "  " << SerializeString<Type::POSITION>() << ": " << hoverable->x << " "
             << hoverable->y << "\n"
             << indent << "  " << SerializeString<Type::SIZE>() << ": " << hoverable->width << " "
             << hoverable->height << "\n"
             << indent << "  " << SerializeString<Type::SCALE>() << ": " << hoverable->scale.x << " " << hoverable->scale.y << "\n"
             << indent << "}\n";
    }

    void SerializeSprite(const SceneObject::Data& object, GlobalSerializeData& data)
    {
      auto* sprite = static_cast<Sprite::Data*>(object.data);
      const auto& position = Scene::Get<Component::Type::POSITION2D>(data.scene, sprite->entity);
      const auto& scale = Scene::Get<Component::Type::SCALE>(data.scene, sprite->entity);
      data.f << "Sprite\n"
             << "{\n";
      SerializeObject(object, data);
      data.f << "  " << SerializeString<Type::POSITION>() << ": " << position.x << " " << position.y << "\n"
             << "  " << SerializeString<Type::SCALE>() << ": " << scale.x << " " << scale.y << "\n"
             << "  " << SerializeString<Type::FILE>() << ": " << sprite->fileName << "\n"
             << "}\n";
    }

    void SerializeTextBox(const SceneObject::Data& object,
                          GlobalSerializeData& data,
                          const std::string& indent = "")
    {
      auto* textBox = static_cast<TextBox::Data*>(object.data);
      SerializeTextBox(object, textBox, data, indent);
    }

    void SerializeTextButton(const SceneObject::Data& object, GlobalSerializeData& data)
    {
      auto indent = "  ";
      auto* textButton = static_cast<TextButton::Data*>(object.data);
      auto& hoverable = Scene::Get<Component::Type::HOVERABLE>(data.scene, textButton->entity);
      data.f << "TextButton\n"
             << "{\n";
      SerializeObject(object, data);
      SerializeTextBox(object, &textButton->textBox, data, indent);
      data.f << "\n";
      SerializeHoverable(&hoverable, data, indent);
      data.f << "}\n";
    }
  }

  bool Deserialize(Scene::Data& scene, const char* file)
  {
    StringHashMap<int> DeserializeTable;
    DeserializeTable["TextBox"] = EntityType::TEXTBOX;
    DeserializeTable["TextButton"] = EntityType::TEXTBUTTON;
    DeserializeTable["Sprite"] = EntityType::SPRITE;
    DeserializeTable["Max"] = EntityType::MAX;

    int lineNumber = 0;
    scene.objects.Clear();
    scene.objectsNameIdxTable.Clear();
    auto path = AssetsDirectory() / "Levels" / file;
    String contents;
    try
    {
      ReadFile(contents, path.c_str());
    }
    catch (const std::exception&)
    {
      Logger::LogErr(String("[Deserialize] Failed to deserialize file: ") + file);
      return false;
    }

    char* l = contents.Buffer();
    String line;
    line.Resize(4096);
    GlobalDeserializeData data{scene, l, line, lineNumber};
    while (GetLine(data.line, data.l, lineNumber))
    {
      // std::cout << line << std::endl;
      if (IgnoreLine(data.line))
      {
        continue;
      }
      if (DeserializeTable.Find(data.line.c_str()) == SIZE_MAX)
      {
        if (!ExtensionDeserializer(data))
        {
          Logger::LogErr(String("[Deserialize] Invalid object name at line ") + String::ToString(lineNumber) + ": " + data.line.c_str());
          return false;
        }
        else
        {
          continue;
        }
      }
      switch (DeserializeTable[data.line.c_str()])
      {
        case EntityType::TEXTBOX:
          if (!DeserializeTextBox(data))
          {
            return false;
          }
          break;
        case EntityType::TEXTBUTTON:
          if (!DeserializeTextButton(data))
          {
            return false;
          }
          break;
        case EntityType::SPRITE:
          if (!DeserializeSprite(data))
          {
            return false;
          }
          break;
        default:
          break;
      }
    }
    return true;
  }

  void Serialize(Scene::Data& scene, const char* file)
  {
    std::ofstream f(file);
    GlobalSerializeData data{scene, f};
    f << "// " << scene.sceneFns->name << ".level\n";
    for (const auto& object : scene.objects)
    {
      f << "\n";
      switch (object.type)
      {
        case EntityType::TEXTBOX:
          SerializeTextBox(object, data);
          continue;
        case EntityType::TEXTBUTTON:
          SerializeTextButton(object, data);
          continue;
        case EntityType::SPRITE:
          SerializeSprite(object, data);
          continue;
        default:
          break;
      }
      ExtensionSerializer(data, object);
    }
  }

  bool LevelExists(const char* file)
  {
    auto path = AssetsDirectory() / "Levels" / file;
    String contents;
    try
    {
      ReadFile(contents, path.c_str());
    }
    catch (const std::exception&)
    {
      Logger::LogErr(String("[Deserialize] Failed to deserialize file: ") + file);
      return false;
    }
    return true;
  }
}
