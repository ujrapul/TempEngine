// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "STDPCH.hpp"
#include "EngineUtils.hpp"
#include "FileWriter.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "MemoryManager.hpp"
#include "Scene.hpp"
#include "String.hpp"
#include "TextBox.hpp"

namespace Temp::LevelSerializer
{
  namespace Type
  {
    enum Type
    {
      NAME,
      SHADER,
      POSITION,
      SCALE,
      SCALE2D,
      SIZE,
      SIZE2D,
      TEXT,
      TEXTBOX,
      TEXTBOXCTOR,
      HOVERABLE,
      FONT,
      CHARACTERLIMIT,
      FILE,
      MAX
    };
  }

  // clang-format off
  template <uint8_t> struct SerializeType_t;
  template <> struct SerializeType_t<Type::NAME> { using type = String; };
  template <> struct SerializeType_t<Type::SHADER> { using type = int; };
  template <> struct SerializeType_t<Type::POSITION> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::SCALE> { using type = float; };
  template <> struct SerializeType_t<Type::SCALE2D> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::SIZE> { using type = float; };
  template <> struct SerializeType_t<Type::SIZE2D> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::TEXT> { using type = String; };
  template <> struct SerializeType_t<Type::TEXTBOX> { using type = TextBox::Data; };
  template <> struct SerializeType_t<Type::TEXTBOXCTOR> { using type = TextBox::ConstructData; };
  template <> struct SerializeType_t<Type::HOVERABLE> { using type = Component::Hoverable::Data; };
  template <> struct SerializeType_t<Type::FONT> { using type = int; };
  template <> struct SerializeType_t<Type::CHARACTERLIMIT> { using type = int; };
  template <> struct SerializeType_t<Type::FILE> { using type = String; };

  template <uint8_t T>
  using SerializeType = typename SerializeType_t<T>::type;

  template <uint8_t T> constexpr const char* SerializeString();
  template <> constexpr const char* SerializeString<Type::NAME>() { return "Name"; }
  template <> constexpr const char* SerializeString<Type::SHADER>() { return "Shader"; }
  template <> constexpr const char* SerializeString<Type::POSITION>() { return "Position"; }
  template <> constexpr const char* SerializeString<Type::SCALE>() { return "Scale"; }
  template <> constexpr const char* SerializeString<Type::SCALE2D>() { return "Scale"; }
  template <> constexpr const char* SerializeString<Type::SIZE>() { return "Size"; }
  template <> constexpr const char* SerializeString<Type::SIZE2D>() { return "Size"; }
  template <> constexpr const char* SerializeString<Type::TEXT>() { return "Text"; }
  template <> constexpr const char* SerializeString<Type::TEXTBOX>() { return "TextBox"; }
  template <> constexpr const char* SerializeString<Type::HOVERABLE>() { return "Hoverable"; }
  template <> constexpr const char* SerializeString<Type::FONT>() { return "Font"; }
  template <> constexpr const char* SerializeString<Type::CHARACTERLIMIT>() { return "CharacterLimit"; }
  template <> constexpr const char* SerializeString<Type::FILE>() { return "File"; }
  // clang-format on

  struct GlobalDeserializeData
  {
    Scene::Data& scene;
    char*& l;
    String& line;
    int& lineNumber;
  };

  struct GlobalSerializeData
  {
    Scene::Data& scene;
    FileWriter& f;
  };

  constexpr uint8_t ENUM_MIN = 0;
  constexpr uint8_t ENUM_MAX = Type::MAX - 1;

  template <uint8_t E>
  constexpr void DestructEnum(const Array<void*, Type::MAX>& data)
  {
    if (data[E] && E != Type::TEXTBOX && E != Type::TEXTBOXCTOR)
    {
      static_cast<SerializeType<E>*>(data[E])->~SerializeType<E>();
      MemoryManager::data.Free(MemoryManager::Data::TEMP, 0);
    }
  }

  template <uint8_t E>
  constexpr void CopyEnum(const Array<void*, Type::MAX>& other, Array<void*, Type::MAX>& data)
  {
    if (!other[E])
    {
      return;
    }

    if (E == Type::TEXTBOX || E == Type::TEXTBOXCTOR)
    {
      data[E] = MemoryManager::CreateScene<SerializeType<E>>(*static_cast<SerializeType<E>*>(other[E]));
    }
    else
    {
      data[E] = MemoryManager::CreateTemp<SerializeType<E>>(*static_cast<SerializeType<E>*>(other[E]));
    }
  }

  template <uint8_t E>
  struct EnumRange
  {
    constexpr static void DestructEnums(const Array<void*, Type::MAX>& data)
    {
      DestructEnum<E>(data);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::DestructEnums(data);
    }

    constexpr static void CopyEnums(const Array<void*, Type::MAX>& other, Array<void*, Type::MAX>& data)
    {
      CopyEnum<E>(other, data);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::CopyEnums(other, data);
    }
  };

  // Careful with how this is used! All data should be copied from here!
  // There should never be a reference or a pointer to this data!
  struct DeserializeData
  {
    Array<void*, Type::MAX> data;

    DeserializeData() {}

    DeserializeData(const DeserializeData& other)
    {
      EnumRange<ENUM_MIN>::CopyEnums(other.data, data);
    }

    constexpr void operator=(const DeserializeData& other)
    {
      EnumRange<ENUM_MIN>::CopyEnums(other.data, data);
    }

    ~DeserializeData()
    {
      EnumRange<ENUM_MIN>::DestructEnums(data);
    }

    template <uint8_t T>
    constexpr SerializeType<T> defaultValue() const
    {
      if constexpr (T == Type::CHARACTERLIMIT)
      {
        return INT_MAX;
      }
      else if constexpr (T == Type::SHADER)
      {
        return -1;
      }
      else
      {
        return SerializeType<T>();
      }
    }

    template <uint8_t T>
    constexpr SerializeType<T> get() const
    {
      return data[T] ? *static_cast<SerializeType<T>*>(data[T]) : defaultValue<T>();
    }
  };

  inline void AddSceneObject(SceneObject::Data object, int type, GlobalDeserializeData& data)
  {
    String name = object.name.c_str();
    object.type = type;
    data.scene.objects.PushBack(std::move(object));
    data.scene.objectsNameIdxTable[name.c_str()] = (int)data.scene.objects.size - 1;
  }

  inline bool GetLine(String& line, char*& l, int& lnum)
  {
    ++lnum;
    char* nextline = strchr(l, '\n');
    size_t length = nextline ? nextline - l : strlen(l);
    line = String(l, l + length);
    Trim(line);
    l = nextline ? nextline + 1 : nullptr;
    return l;
  }

  inline bool IgnoreLine(const String& line)
  {
    return line.starts_with("//") || line == "";
  }

  inline bool LogInvalidDelimiter(GlobalDeserializeData& data)
  {
    Logger::LogErr(String("[Deserialize] Invalid delimiter at line ") + String::ToString(data.lineNumber) +
                   ": " + data.line.c_str());
    return false;
  }

  inline bool LogInvalidPosition(GlobalDeserializeData& data)
  {
    Logger::LogErr(String("[Deserialize] Invalid position at line ") + String::ToString(data.lineNumber) +
                   ": " + data.line.c_str());
    return false;
  }

  inline bool LogNoStartBrace(GlobalDeserializeData& data)
  {
    Logger::LogErr(String("[Deserialize] No start brace '{' at line ") + String::ToString(data.lineNumber) +
                   ": " + data.line.c_str());
    return false;
  }

  inline bool LogNoEndBrace(GlobalDeserializeData& data)
  {
    Logger::LogErr(String("[Deserialize] No end brace '}' at line ") + String::ToString(data.lineNumber) +
                   ": " + data.line.c_str());
    return false;
  }

  inline std::tuple<DeserializeData, bool> Deserialize(GlobalDeserializeData& data);

  inline std::tuple<DeserializeData, bool> Deserialize(GlobalDeserializeData& data,
                                                       TextBox::Data*& textBox,
                                                       TextBox::ConstructData*& textBoxCtor)
  {
    auto outTB = Deserialize(data);
    if (!std::get<1>(outTB))
    {
      return outTB;
    }
    auto textBoxData = std::get<0>(outTB);
    textBox = MemoryManager::CreateScene<TextBox::Data>();
    textBoxCtor = MemoryManager::CreateScene<TextBox::ConstructData>();
    // textBox->name = textBoxData.get<Type::NAME>();
    textBoxCtor->x = textBoxData.get<Type::POSITION>().x;
    textBoxCtor->y = textBoxData.get<Type::POSITION>().y;
    textBoxCtor->scale = textBoxData.get<Type::SCALE>();
    textBox->text = textBoxData.get<Type::TEXT>().c_str();
    textBox->fontType = textBoxData.get<Type::FONT>();
    textBox->maxCharactersPerLine = textBoxData.get<Type::CHARACTERLIMIT>();
    return outTB;
  }

  // Make sure to clean up data
  inline std::tuple<DeserializeData, bool> Deserialize(GlobalDeserializeData& data)
  {
    DeserializeData out;
    if (GetLine(data.line, data.l, data.lineNumber) && data.line.find("{") == SIZE_MAX)
    {
      return {out, LogNoStartBrace(data)};
    }
    while (GetLine(data.line, data.l, data.lineNumber))
    {
      if (data.line.find("}") != SIZE_MAX)
      {
        return {out, true};
      }
      if (IgnoreLine(data.line))
      {
        continue;
      }
      auto tokens = SplitString(data.line, ": ", 1);
      if (tokens.size != 2)
      {
        if (tokens[0].find(SerializeString<Type::TEXTBOX>()) != SIZE_MAX)
        {
          TextBox::Data* textBox;
          TextBox::ConstructData* textBoxCtor;
          if (!std::get<1>(Deserialize(data, textBox, textBoxCtor)))
          {
            return {out, false};
          }
          out.data[Type::TEXTBOX] = static_cast<void*>(textBox);
          out.data[Type::TEXTBOXCTOR] = static_cast<void*>(textBoxCtor);
        }
        else if (tokens[0].find(SerializeString<Type::HOVERABLE>()) != SIZE_MAX)
        {
          auto outH = Deserialize(data);
          if (!std::get<1>(outH))
          {
            return {out, false};
          }
          auto hoverableData = std::get<0>(outH);
          auto hoverable = MemoryManager::CreateTemp<Component::Hoverable::Data>();
          hoverable->x = hoverableData.get<Type::POSITION>().x;
          hoverable->y = hoverableData.get<Type::POSITION>().y;
          hoverable->width = hoverableData.get<Type::SIZE2D>().x;
          hoverable->height = hoverableData.get<Type::SIZE2D>().y;
          if (hoverableData.data[Type::SCALE2D])
            hoverable->scale = hoverableData.get<Type::SCALE2D>();
          else
            hoverable->scale = {hoverableData.get<Type::SCALE>(), hoverableData.get<Type::SCALE>()};
          // std::cout << hoverable->x << " " << hoverable->y << " " << hoverable->width << " " <<
          // hoverable->height << std::endl;
          out.data[Type::HOVERABLE] = static_cast<void*>(hoverable);
        }
        else
        {
          return {out, LogInvalidDelimiter(data)};
        }
      }
      else if (tokens[0].find(SerializeString<Type::NAME>()) != SIZE_MAX)
      {
        out.data[Type::NAME] = MemoryManager::CreateTemp<String>(std::move(tokens[1]));
      }
      else if (tokens[0].find(SerializeString<Type::SHADER>()) != SIZE_MAX)
      {
        out.data[Type::SHADER] = MemoryManager::CreateTemp<int>(std::atoi(tokens[1].c_str()));
      }
      else if (tokens[0].find(SerializeString<Type::POSITION>()) != SIZE_MAX)
      {
        auto position = SplitString(tokens[1], " ");
        if (position.size != 2)
        {
          return {out, LogInvalidPosition(data)};
        }
        out.data[Type::POSITION] = MemoryManager::CreateTemp<Math::Vec2f>(
          std::strtof(position[0].c_str(), nullptr),
          std::strtof(position[1].c_str(), nullptr));
      }
      else if (tokens[0].find(SerializeString<Type::SCALE>()) != SIZE_MAX)
      {
        auto trimmed = Trim(tokens[1]);
        auto scale = SplitString(trimmed, " ");
        if (scale.size == 2)
        {
          out.data[Type::SCALE2D] = MemoryManager::CreateTemp<Math::Vec2f>(
            std::strtof(scale[0].c_str(), nullptr),
            std::strtof(scale[1].c_str(), nullptr));
        }
        else
        {
          out.data[Type::SCALE] = MemoryManager::CreateTemp<float>(
            std::strtof(tokens[1].c_str(), nullptr));
        }
      }
      else if (tokens[0].find(SerializeString<Type::SIZE>()) != SIZE_MAX)
      {
        auto size = SplitString(Trim(tokens[1]), " ");
        if (size.size == 2)
        {
          out.data[Type::SIZE2D] = MemoryManager::CreateTemp<Math::Vec2f>(
            std::strtof(size[0].c_str(), nullptr),
            std::strtof(size[1].c_str(), nullptr));
        }
        else
        {
          out.data[Type::SIZE] = MemoryManager::CreateTemp<float>(
            (float)std::atof(tokens[1].c_str()));
        }
      }
      else if (tokens[0].find(SerializeString<Type::TEXT>()) != SIZE_MAX)
      {
        out.data[Type::TEXT] = MemoryManager::CreateTemp<String>(std::move(tokens[1]));
      }
      else if (tokens[0].find(SerializeString<Type::FONT>()) != SIZE_MAX)
      {
        out.data[Type::FONT] = MemoryManager::CreateTemp<int>(std::atoi(tokens[1].c_str()));
      }
      else if (tokens[0].find(SerializeString<Type::CHARACTERLIMIT>()) != SIZE_MAX)
      {
        out.data[Type::CHARACTERLIMIT] = MemoryManager::CreateTemp<int>(std::atoi(tokens[1].c_str()));
      }
      else if (tokens[0].find(SerializeString<Type::FILE>()) != SIZE_MAX)
      {
        out.data[Type::FILE] = MemoryManager::CreateTemp<String>(std::move(tokens[1]));
      }
    }
    return {out, LogNoEndBrace(data)};
  }

  // Make sure memory that's used for objects is not leaked!
  // Use ExtensionParser for games that have custom types
  bool Deserialize(Scene::Data& scene, const char* file);
  void Serialize(Scene::Data& scene, const char* file);
  bool LevelExists(const char* file);
}
