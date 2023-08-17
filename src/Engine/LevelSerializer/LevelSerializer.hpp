// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "EngineUtils.hpp"
#include "EntityType.hpp"
#include "Math.hpp"
#include "Scene.hpp"
#include "TextBox.hpp"
#include <string>
#include <unordered_map>
#include <vector>

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
  template <> struct SerializeType_t<Type::NAME> { using type = std::string; };
  template <> struct SerializeType_t<Type::SHADER> { using type = int; };
  template <> struct SerializeType_t<Type::POSITION> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::SCALE> { using type = float; };
  template <> struct SerializeType_t<Type::SCALE2D> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::SIZE> { using type = float; };
  template <> struct SerializeType_t<Type::SIZE2D> { using type = Math::Vec2f; };
  template <> struct SerializeType_t<Type::TEXT> { using type = std::string; };
  template <> struct SerializeType_t<Type::TEXTBOX> { using type = TextBox::Data; };
  template <> struct SerializeType_t<Type::TEXTBOXCTOR> { using type = TextBox::ConstructData; };
  template <> struct SerializeType_t<Type::HOVERABLE> { using type = Component::Hoverable::Data; };
  template <> struct SerializeType_t<Type::FONT> { using type = int; };
  template <> struct SerializeType_t<Type::CHARACTERLIMIT> { using type = int; };
  template <> struct SerializeType_t<Type::FILE> { using type = std::string; };

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

  // Pass SceneObjectMap
  struct GlobalDeserializeData
  {
    Scene::Data& scene;
    std::istringstream& f;
    std::string& line;
    int& lineNumber;
  };

  struct GlobalSerializeData
  {
    Scene::Data& scene;
    std::ofstream& f;
  };

  constexpr uint8_t ENUM_MIN = 0;
  constexpr uint8_t ENUM_MAX = Type::MAX - 1;

  template <uint8_t E>
  constexpr void DestructEnum(const std::vector<void*>& data)
  {
    delete static_cast<SerializeType<E>*>(data[E]);
  }

  template <uint8_t E>
  constexpr void CopyEnum(const std::vector<void*>& other, std::vector<void*>& data)
  {
    if (other[E])
    {
      data[E] = new SerializeType<E>(*static_cast<SerializeType<E>*>(other[E]));
    }
  }

  template <uint8_t E>
  struct EnumRange
  {
    constexpr static void DestructEnums(const std::vector<void*>& data)
    {
      DestructEnum<E>(data);
      if constexpr (E < ENUM_MAX)
        EnumRange<E + 1>::DestructEnums(data);
    }

    constexpr static void CopyEnums(const std::vector<void*>& other, std::vector<void*>& data)
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
    std::vector<void*> data;

    DeserializeData() { data.resize(Type::MAX, nullptr); }

    DeserializeData(const DeserializeData& other)
    {
      data.resize(Type::MAX, nullptr);
      EnumRange<ENUM_MIN>::CopyEnums(other.data, data);
    }

    constexpr void operator=(const DeserializeData& other)
    {
      data.resize(Type::MAX, nullptr);
      EnumRange<ENUM_MIN>::CopyEnums(other.data, data);
    }

    ~DeserializeData() { EnumRange<ENUM_MIN>::DestructEnums(data); }

    template <uint8_t T>
    constexpr SerializeType<T> defaultValue()
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
    constexpr SerializeType<T> get()
    {
      return data[T] ? *static_cast<SerializeType<T>*>(data[T]) : defaultValue<T>();
    }

    template <uint8_t T>
    constexpr SerializeType<T>* copy() const
    {
      return new SerializeType<T>(*static_cast<SerializeType<T>*>(data[T]));
    }
  };

  inline const std::unordered_map<std::string_view, int> DeserializeTable = {
    {"TextBox", EntityType::TEXTBOX},
    {"TextButton", EntityType::TEXTBUTTON},
    {"Sprite", EntityType::SPRITE},
    {"Max", EntityType::MAX}};

  inline void AddSceneObject(SceneObject::Data& object, int type, GlobalDeserializeData& data)
  {
    std::string name = object.name;
    object.type = type;
    data.scene.objects.push_back(object);
    data.scene.objectsNameIdxTable[name] = (int)data.scene.objects.size() - 1;
  }

  inline bool GetLine(std::istringstream& f, std::string& l, int& lnum)
  {
    ++lnum;
    return std::getline(f, l) ? true : false;
  }

  inline bool IgnoreLine(const std::string_view line)
  {
    return line.starts_with("//") || line == "";
  }

  inline bool LogInvalidDelimiter(GlobalDeserializeData& data)
  {
    Logger::LogErr("[Deserialize] Invalid delimiter at line " + std::to_string(data.lineNumber) +
                   ": " + data.line);
    return false;
  }

  inline bool LogInvalidPosition(GlobalDeserializeData& data)
  {
    Logger::LogErr("[Deserialize] Invalid position at line " + std::to_string(data.lineNumber) +
                   ": " + data.line);
    return false;
  }

  inline bool LogNoStartBrace(GlobalDeserializeData& data)
  {
    Logger::LogErr("[Deserialize] No start brace '{' at line " + std::to_string(data.lineNumber) +
                   ": " + data.line);
    return false;
  }

  inline bool LogNoEndBrace(GlobalDeserializeData& data)
  {
    Logger::LogErr("[Deserialize] No end brace '}' at line " + std::to_string(data.lineNumber) +
                   ": " + data.line);
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
    textBox = new TextBox::Data();
    textBoxCtor = new TextBox::ConstructData();
    // textBox->name = textBoxData.get<Type::NAME>();
    textBoxCtor->x = textBoxData.get<Type::POSITION>().x;
    textBoxCtor->y = textBoxData.get<Type::POSITION>().y;
    textBoxCtor->scale = textBoxData.get<Type::SCALE>();
    textBox->text = textBoxData.get<Type::TEXT>();
    textBox->fontType = textBoxData.get<Type::FONT>();
    textBox->maxCharactersPerLine = textBoxData.get<Type::CHARACTERLIMIT>();
    return outTB;
  }

  // Make sure to clean up data
  inline std::tuple<DeserializeData, bool> Deserialize(GlobalDeserializeData& data)
  {
    DeserializeData out;
    if (GetLine(data.f, data.line, data.lineNumber) && data.line.find('{') == std::string::npos)
    {
      return {out, LogNoStartBrace(data)};
    }
    while (GetLine(data.f, data.line, data.lineNumber))
    {
      data.line = Trim(data.line);
      if (data.line.find('}') != std::string::npos)
      {
        return {out, true};
      }
      if (IgnoreLine(data.line))
      {
        continue;
      }
      auto tokens = SplitString(data.line, ": ", 1);
      if (tokens.size() != 2)
      {
        if (tokens[0].find(SerializeString<Type::TEXTBOX>()) != std::string::npos)
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
        else if (tokens[0].find(SerializeString<Type::HOVERABLE>()) != std::string::npos)
        {
          auto outH = Deserialize(data);
          if (!std::get<1>(outH))
          {
            return {out, false};
          }
          auto hoverableData = std::get<0>(outH);
          auto hoverable = new Component::Hoverable::Data();
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
      else if (tokens[0].find(SerializeString<Type::NAME>()) != std::string::npos)
      {
        out.data[Type::NAME] = static_cast<void*>(new std::string(tokens[1]));
      }
      else if (tokens[0].find(SerializeString<Type::SHADER>()) != std::string::npos)
      {
        out.data[Type::SHADER] = static_cast<void*>(new int(std::atoi(tokens[1].data())));
      }
      else if (tokens[0].find(SerializeString<Type::POSITION>()) != std::string::npos)
      {
        auto position = SplitString(tokens[1], " ");
        if (position.size() != 2)
        {
          return {out, LogInvalidPosition(data)};
        }
        out.data[Type::POSITION] = static_cast<void*>(
          new Math::Vec2f(std::strtof(position[0].data(), nullptr),
                          std::strtof(position[1].data(), nullptr)));
      }
      else if (tokens[0].find(SerializeString<Type::SCALE>()) != std::string::npos)
      {
        auto scale = SplitString(Trim(tokens[1]), " ");
        if (scale.size() == 2)
        {
          out.data[Type::SCALE2D] = static_cast<void*>(
            new Math::Vec2f(std::strtof(scale[0].data(), nullptr),
                            std::strtof(scale[1].data(), nullptr)));
        }
        else
        {
          out.data[Type::SCALE] = static_cast<void*>(
            new float(std::strtof(tokens[1].data(), nullptr)));
        }
      }
      else if (tokens[0].find(SerializeString<Type::SIZE>()) != std::string::npos)
      {
        auto size = SplitString(Trim(tokens[1]), " ");
        if (size.size() == 2)
        {
          out.data[Type::SIZE2D] = static_cast<void*>(
            new Math::Vec2f(std::strtof(size[0].data(), nullptr),
                            std::strtof(size[1].data(), nullptr)));
        }
        else
        {
          out.data[Type::SIZE] = static_cast<void*>(new float((float)std::atof(tokens[1].data())));
        }
      }
      else if (tokens[0].find(SerializeString<Type::TEXT>()) != std::string::npos)
      {
        out.data[Type::TEXT] = static_cast<void*>(new std::string(tokens[1]));
      }
      else if (tokens[0].find(SerializeString<Type::FONT>()) != std::string::npos)
      {
        out.data[Type::FONT] = static_cast<void*>(new int(std::atoi(tokens[1].data())));
      }
      else if (tokens[0].find(SerializeString<Type::CHARACTERLIMIT>()) != std::string::npos)
      {
        out.data[Type::CHARACTERLIMIT] = static_cast<void*>(new int(std::atoi(tokens[1].data())));
      }
      else if (tokens[0].find(SerializeString<Type::FILE>()) != std::string::npos)
      {
        out.data[Type::FILE] = static_cast<void*>(new std::string(tokens[1]));
      }
    }
    return {out, LogNoEndBrace(data)};
  }

  // Make sure memory that's used for objects is not leaked!
  // Use ExtensionParser for games that have custom types
  bool Deserialize(Scene::Data& scene, const std::string& file);
  void Serialize(Scene::Data& scene, const std::string& file);
  bool LevelExists(const std::string& file);
}
