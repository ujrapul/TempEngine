// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "MemoryManager.hpp"
#include "EngineUtils.hpp"
#include "String.hpp"
#include <climits>
#include <cstring>
#include <type_traits>

namespace Temp
{
  template <typename K,
            typename V,
            typename Hash1,
            typename Hash2,
            MemoryManager::Data::Type Type = MemoryManager::Data::TEMP,
            size_t Size = 8192>
  struct HashMap
  {
    struct Pair
    {
      K key;
      V value;
      uint8_t hash{UINT8_MAX};
    };

    DynamicArray<Pair, Type> buffer;
    
    constexpr HashMap() {}

    // Copy constructor
    constexpr HashMap(const HashMap& other) noexcept
    {
      buffer = other.buffer;
    }

    // Move constructor
    constexpr HashMap(HashMap&& other) noexcept
      : HashMap()
    {
      Swap(*this, other);
    }

    // Does a copy operation
    constexpr HashMap& operator=(HashMap other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr void Swap(HashMap& first, HashMap& second)
    {
      Utils::Swap(first.buffer, second.buffer);
    }

    const DynamicArray<Pair, Type>& Buffer() const
    {
      return buffer;
    }

    DynamicArray<Pair, Type>& Buffer()
    {
      if (buffer.size == 0)
      {
        buffer.Resize(Size);
      }
      return buffer;
    }

    const V& operator[](const K& key) const
    {
      size_t pos1 = Hash(0, key);
      if (buffer[pos1].key == key)
      {
        return buffer[pos1].value;
      }
      size_t pos2 = Hash(1, key);
      if (buffer[pos2].key == key)
      {
        return buffer[pos2].value;
      }
      assert(false);
    }

    V& operator[](const K& key)
    {
      size_t pos1 = Hash(0, key);
      if (Buffer()[pos1].key == key)
      {
        return Buffer()[pos1].value;
      }
      size_t pos2 = Hash(1, key);
      if (Buffer()[pos2].key == key)
      {
        return Buffer()[pos2].value;
      }
      if (Insert(key, {}))
      {
        size_t pos1 = Hash(0, key);
        if (Buffer()[pos1].key == key)
        {
          return Buffer()[pos1].value;
        }
        size_t pos2 = Hash(1, key);
        if (Buffer()[pos2].key == key)
        {
          return Buffer()[pos2].value;
        }
      }
      assert(false);
    }

    bool Insert(K& key, V& value)
    {
      static size_t recursions = 0;
      assert(recursions <= Size);

      size_t pos1 = Hash(0, key);
      size_t pos2 = Hash(1, key);
      if (Buffer()[pos1].key == key)
      {
        Buffer()[pos1].value = value;
        recursions = 0;
        return true;
      }
      else if (Buffer()[pos2].key == key)
      {
        Buffer()[pos2].value = value;
        recursions = 0;
        return true;
      }

      if (Buffer()[pos1].hash == UINT8_MAX)
      {
        Buffer()[pos1] = Pair(key, value, 0);
        recursions = 0;
        return true;
      }
      else if (Buffer()[pos2].hash == UINT8_MAX)
      {
        Buffer()[pos2] = Pair(key, value, 1);
        recursions = 0;
        return true;
      }
      else
      {
        auto prevEntry = Buffer()[pos1];
        Buffer()[pos1] = Pair(key, value, 0);
        ++recursions;
        Insert(prevEntry.key, prevEntry.value);
      }

      recursions = 0;
      return false;
    }

    bool Contains(const K& key) const
    {
      size_t pos1 = Hash(0, key);
      size_t pos2 = Hash(1, key);
      return buffer.size != 0 && (Buffer()[pos1].hash != UINT_MAX || Buffer()[pos2].hash != UINT_MAX);
    }

    void Remove(const K& key)
    {
      size_t pos1 = Hash(0, key);
      if (Buffer()[pos1].key == key)
      {
        Buffer()[pos1] = {};
        return;
      }
      size_t pos2 = Hash(1, key);
      if (Buffer()[pos2].key == key)
      {
        Buffer()[pos2] = {};
        return;
      }
      assert(false);
    }

    void Clear()
    {
      for (size_t i = 0; i < Size; ++i)
      {
        Buffer()[i] = {};
      }
    }
    
    size_t Find(const K& key) const
    {
      size_t pos1 = Hash(0, key);
      if (Buffer()[pos1].key == key)
      {
        return pos1;
      }
      size_t pos2 = Hash(1, key);
      if (Buffer()[pos2].key == key)
      {
        return pos2;
      }
      return SIZE_MAX;
    }

    inline size_t Hash(size_t function, const K& key)
    {
      switch (function)
      {
        case 0:
          return Hash1(key) % Size / 2;
        default:
          return Hash2(key) % Size / 2 + Size / 2;
      }
    }
  };

  template <typename K,
            typename V,
            size_t Hash1(const char*),
            size_t Hash2(const char*),
            MemoryManager::Data::Type Type = MemoryManager::Data::TEMP,
            size_t Size = 8192>
  struct BaseStringHashMap
  {
    struct Pair
    {
      K key;
      V value;
      uint8_t hash{UINT8_MAX};

      constexpr bool operator==(const Pair& other) const
      {
        return key == other.key
          && value == other.value
          && hash == other.hash;
      }
    };

    DynamicArray<Pair, Type> buffer;
    
    constexpr BaseStringHashMap() {}

    constexpr ~BaseStringHashMap() {}

    // Copy constructor
    constexpr BaseStringHashMap(const BaseStringHashMap& other) noexcept
    {
      buffer = other.buffer;
    }

    // Move constructor
    constexpr BaseStringHashMap(BaseStringHashMap&& other) noexcept
      : BaseStringHashMap()
    {
      Swap(*this, other);
    }

    // Does a copy operation
    constexpr BaseStringHashMap& operator=(BaseStringHashMap other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr bool operator==(const BaseStringHashMap& other) const
    {
      return buffer == other.buffer;
    }

    constexpr void Swap(BaseStringHashMap& first, BaseStringHashMap& second)
    {
      first.buffer.Swap(first.buffer, second.buffer);
    }

    const DynamicArray<Pair, Type>& Buffer() const
    {
      return buffer;
    }

    DynamicArray<Pair, Type>& Buffer()
    {
      if (buffer.size == 0)
      {
        buffer.Resize(Size);
      }
      return buffer;
    }

    K& GetKey(size_t pos)
    {
      if (!Buffer()[pos].key.buffer)
      {
        Buffer()[pos].key.Replace("");
      }
      return Buffer()[pos].key;
    }

    const V& operator[](const char* key) const
    {
      size_t pos1 = Hash(0, key);
      if (String(Buffer()[pos1].key.c_str()) == String(key))
      {
        return Buffer()[pos1].value;
      }
      size_t pos2 = Hash(1, key);
      if (String(Buffer()[pos2].key.c_str()) == String(key))
      {
        return Buffer()[pos2].value;
      }
      assert(false);
      return Buffer()[0].value;
    }

    V& operator[](const char* key)
    {
      String other = String(key);

      size_t pos1 = Hash(0, key);
      String a1 = String(GetKey(pos1).c_str());
      if (a1 == other)
      {
        return Buffer()[pos1].value;
      }
      size_t pos2 = Hash(1, key);
      String a2 = String(GetKey(pos2).c_str());
      if (a2 == other)
      {
        return Buffer()[pos2].value;
      }
      if (Insert(key, {}))
      {
        size_t pos1 = Hash(0, key);
        if (String(GetKey(pos1).c_str()) == String(key))
        {
          return Buffer()[pos1].value;
        }
        size_t pos2 = Hash(1, key);
        if (String(GetKey(pos2).c_str()) == String(key))
        {
          return Buffer()[pos2].value;
        }
      }
      assert(false);
      return Buffer()[0].value;
    }

    bool Insert(const char* key, V value)
    {
      static size_t recursions = 0;
      assert(recursions <= Size);

      size_t pos1 = Hash(0, key);
      size_t pos2 = Hash(1, key);
      if (String(GetKey(pos1).c_str()) == String(key))
      {
        buffer[pos1].value = value;
        recursions = 0;
        return true;
      }
      else if (String(GetKey(pos2).c_str()) == String(key))
      {
        buffer[pos2].value = value;
        recursions = 0;
        return true;
      }

      if (buffer[pos1].hash == UINT8_MAX)
      {
        buffer[pos1].key.Replace(key);
        buffer[pos1].value = value;
        buffer[pos1].hash = 0;
        recursions = 0;
        return true;
      }
      else if (buffer[pos2].hash == UINT8_MAX)
      {
        buffer[pos2].key.Replace(key);
        buffer[pos2].value = value;
        buffer[pos2].hash = 1;
        recursions = 0;
        return true;
      }
      else
      {
        auto prevEntry = buffer[pos1];
        buffer[pos1] = Pair(key, value, 0);
        ++recursions;
        Insert(prevEntry.key.c_str(), prevEntry.value);
      }

      recursions = 0;
      return false;
    }

    bool Contains(const char* key) const
    {
      size_t pos1 = Hash(0, key);
      size_t pos2 = Hash(1, key);
      return buffer.size != 0 && (Buffer()[pos1].hash != UINT_MAX || Buffer()[pos2].hash != UINT_MAX);
    }

    void Remove(const char* key)
    {
      size_t pos1 = Hash(0, key);
      if (String(Buffer()[pos1].key.c_str()) == String(key))
      {
        buffer[pos1].key.Replace("");
        buffer[pos1].value = {};
        buffer[pos1].hash = UINT8_MAX;
        return;
      }
      size_t pos2 = Hash(1, key);
      if (String(Buffer()[pos2].key.c_str()) == String(key))
      {
        buffer[pos2].key.Replace("");
        buffer[pos2].value = {};
        buffer[pos2].hash = UINT8_MAX;
        return;
      }
      assert(false);
    }

    void Clear()
    {
      for (size_t i = 0; i < Size; ++i)
      {
        if (Buffer()[i].key.buffer)
        {
          Buffer()[i].key.Replace("");
        }
        Buffer()[i].value = {};
        Buffer()[i].hash = UINT8_MAX;
      }
    }
    
    size_t Find(const char* key) const
    {
      size_t pos1 = Hash(0, key);
      const char* buffer1 = Buffer()[pos1].key.c_str();
      if (buffer1)
      {
        if (String(buffer1) == String(key))
        {
          return pos1;
        }
      }
      size_t pos2 = Hash(1, key);
      const char* buffer2 = Buffer()[pos1].key.c_str();
      if (buffer2)
      {
        if (String(buffer2) == String(key))
        {
          return pos2;
        }
      }
      return SIZE_MAX;
    }

    inline size_t Hash(size_t function, const char* key) const
    {
      switch (function)
      {
        case 0:
          return Hash1(key) % Size / 2;
        default:
          return Hash2(key) % Size / 2 + Size / 2;
      }
    }
  };

  inline size_t BaseStringHash1(const char* v)
  {
    size_t hash = 7;
    size_t size = strlen(v);
    for (size_t i = 0; i < size; ++i)
    {
      hash = hash * 31 + v[i];
    }
    return hash;
  }

  inline size_t BaseStringHash2(const char* v)
  {
    size_t hash = 13;
    size_t size = strlen(v);
    for (size_t i = 0; i < size; ++i)
    {
      hash = hash * 157 + v[i];
    }
    return hash;
  }

  template <typename V, size_t Size = 8192>
  using GlobalStringHashMap = BaseStringHashMap<GlobalString, V, BaseStringHash1, BaseStringHash2, MemoryManager::Data::Type::GLOBAL_ARENA, Size>;
  template <typename V, size_t Size = 8192>
  using SceneStringHashMap = BaseStringHashMap<SceneString, V, BaseStringHash1, BaseStringHash2, MemoryManager::Data::Type::SCENE_ARENA, Size>;
  template <typename V, size_t Size = 8192>
  using StringHashMap = BaseStringHashMap<String, V, BaseStringHash1, BaseStringHash2, MemoryManager::Data::Type::TEMP, Size>;
}