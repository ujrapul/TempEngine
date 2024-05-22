// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "MemoryManager.hpp"
#include "MemoryUtils.hpp"
#include <cstdint>
#include <cstring>

namespace Temp
{
  template <MemoryManager::Data::Type Type = MemoryManager::Data::Type::TEMP>
  struct BaseString
  {
    char* buffer{nullptr};
    size_t offset{0};
    size_t prevOffset{0};
    size_t size{0};
    size_t capacity{8};

    template<typename S>
    constexpr S Max(S a, S b) const
    {
      return a > b ? a : b;
    }

    template<typename S>
    constexpr S Min(S a, S b) const
    {
      return a < b ? a : b;
    }

    // Lazy Initialize String
    constexpr BaseString() noexcept
    {
      // prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      // buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, size * sizeof(char)));
      // offset = MemoryManager::data.GetCurrentOffset(Type);
      // Fill({});
    }

    constexpr BaseString(char* bufferStart, char* bufferEnd) noexcept
      : size(bufferEnd - bufferStart),
        capacity(Max(size * 2, 8ul))
    {
      Initialize();
      strncpy(buffer, bufferStart, size);
    }

    constexpr BaseString(char* elements, size_t _size) noexcept
      : size(_size),
        capacity(Max(size * 2, 8ul))
    {
      Initialize();
      strncpy(buffer, elements, _size);
    }

    constexpr BaseString(const char* other) noexcept
      : size(strlen(other)),
        capacity(Max(size * 2, 8ul))
    {
      Initialize();
      strcpy(buffer, other);
    }

    // Copy constructor
    BaseString(const BaseString& other) noexcept
     : BaseString()
    {
      if (!other.buffer)
      {
        if (buffer)
        {
          MemoryManager::data.Free(Type, prevOffset);
          offset = 0;
          prevOffset = 0;
          size = 0;
          capacity = 8;
          buffer = nullptr;
        }
        return;
      }

      if (*this == other)
      {
        return;
      }

      if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
      {
        std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, other.capacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      else
      {
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, other.capacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }

      strncpy(buffer, other.buffer, other.size);

      capacity = other.capacity;
      size = other.size;
    }

    // Move constructor
    constexpr BaseString(BaseString&& other) noexcept
      : BaseString()
    {
      Swap(*this, other);
    }

    ~BaseString()
    {
      // If buffer is "moved" it shouldn't reach here
      if (buffer)
      {
        if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
        {
          std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
          MemoryManager::data.Free(Type, prevOffset);
        }
        else
        {
          MemoryManager::data.Free(Type, prevOffset);
        }
        buffer = nullptr;
      }
    }

    constexpr char& operator[](size_t index)
    {
      return buffer[index];
    }

    constexpr const char& operator[](size_t index) const
    {
      return buffer[index];
    }

    constexpr BaseString operator+(const BaseString& other)
    {
      return operator+(other.c_str());
    }

    inline BaseString operator+(const char* other)
    {
      BaseString out;
      out.size = size + strlen(other);
      out.capacity = out.size * 2;
      if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
      {
        std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
        out.prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        out.buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, out.capacity * sizeof(char)));
        out.offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      else
      {
        out.prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        out.buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, out.capacity * sizeof(char)));
        out.offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      if (buffer)
      {
        strncpy(out.buffer, buffer, size);
      }
      strcpy(out.buffer + size, other);
      return out;
    }

    inline BaseString operator+(char c)
    {
      BaseString out;
      out.size = size + 1;
      out.capacity = out.size * 2;
      if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
      {
        std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
        out.prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        out.buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, out.capacity * sizeof(char)));
        out.offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      else
      {
        out.prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        out.buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, out.capacity * sizeof(char)));
        out.offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      if (buffer)
      {
        strncpy(out.buffer, buffer, size);
      }
      out.buffer[size] = c;
      return out;
    }

    constexpr void Swap(BaseString& first, BaseString& second)
    {
      Utils::Swap(first.buffer, second.buffer);
      Utils::Swap(first.capacity, second.capacity);
      Utils::Swap(first.offset, second.offset);
      Utils::Swap(first.prevOffset, second.prevOffset);
      Utils::Swap(first.size, second.size);
    }

    // Does a copy operation
    constexpr BaseString& operator=(BaseString other) noexcept
    {
      Swap(*this, other);
      return *this;
    }

    constexpr bool operator==(const BaseString& other) const
    {
      bool membersSame = size == other.size;

      if (!membersSame)
      {
        return false;
      }
      
      for (size_t i = 0; i < size; ++i)
      {
        if (buffer[i] != other.buffer[i])
        {
          return false;
        }
      }

      return true;
    }

    inline void Initialize()
    {
      if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
      {
        std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, capacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      else
      {
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        buffer = static_cast<char*>(MemoryManager::data.Allocate(Type, capacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }
    }

    constexpr char* Buffer()
    {
      if (!buffer)
      {
        Initialize();
        memset(buffer, 0, capacity * sizeof(char));
      }
      return buffer;
    }

    constexpr void InsertEnd(std::initializer_list<char> elements)
    {
      if ((elements.size() + size) >= capacity)
      {
        Resize((elements.size() + size) * 2);
      }
      
      for (const auto& element : elements)
      {
        Buffer()[size] = element;
        ++size;
      }
    }

    inline void Resize(size_t newCapacity)
    {
      if (newCapacity == 0 || newCapacity <= capacity)
      {
        return;
      }

      if (buffer)
      {
        MemoryManager::data.Free(Type, prevOffset);
      }

      char* newBuffer;
      if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
      {
        std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        newBuffer = static_cast<char*>(MemoryManager::data.Allocate(Type, newCapacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      else
      {
        prevOffset = MemoryManager::data.GetCurrentOffset(Type);
        newBuffer = static_cast<char*>(MemoryManager::data.Allocate(Type, newCapacity * sizeof(char)));
        offset = MemoryManager::data.GetCurrentOffset(Type);
      }
      if (buffer)
      {
        strncpy(newBuffer, buffer, size);
      }

      buffer = newBuffer;
      capacity = newCapacity;
    }

    constexpr void Clear()
    {
      size = 0;
    }

    constexpr void TrimEnd(size_t num)
    {
      if (!buffer)
      {
        return;
      }
      size -= num;
      buffer[size] = '\0';
    }

    constexpr void Replace(const char* str)
    {
      if ((strlen(str) + size) >= capacity)
      {
        Resize(Max((size + strlen(str)) * 2, 8ul));
      }
      memset(Buffer(), 0, capacity * sizeof(char));
      size = strlen(str);
      strcpy(buffer, str);
    }

    constexpr const char* c_str() const
    {
      return buffer;
    }

    static BaseString ToString(int i)
    {
      BaseString out;
      int length = snprintf(NULL, 0, "%d", i);
      out.size = length;
      out.Resize((size_t)length + 1);
      sprintf(out.Buffer(), "%d", i);
      return out;
    }

    static BaseString ToString(unsigned int i)
    {
      BaseString out;
      int length = snprintf(NULL, 0, "%u", i);
      out.size = length;
      out.Resize((size_t)length + 1);
      sprintf(out.Buffer(), "%u", i);
      return out;
    }

    static BaseString ToString(signed long int i)
    {
      BaseString out;
      int length = snprintf(NULL, 0, "%ld", i);
      out.size = length;
      out.Resize((size_t)length + 1);
      sprintf(out.Buffer(), "%ld", i);
      return out;
    }

    static BaseString ToString(float i)
    {
      BaseString out;
      int length = snprintf(NULL, 0, "%f", i);
      out.size = length;
      out.Resize((size_t)length + 1);
      sprintf(out.Buffer(), "%f", i);
      return out;
    }

    constexpr size_t find_first_not_of(const char* str) const
    {
      for (size_t i = 0; i < size; ++i)
      {
        char c = buffer[i];
        bool found = true;
        for (size_t j = 0; j < strlen(str); ++j)
        {
          if (c == str[j])
          {
            found = false;
            break;
          }
        }
        if (found)
        {
          return i;
        }
      }
      return size;
    }

    // Remove number of characters at front
    inline void remove_prefix(size_t characters)
    {
      if (characters == 0)
      {
        return;
      }
      else if (characters >= size)
      {
        memset(buffer, 0, size * sizeof(char));
        size = 0;
        return;
      }

      size -= characters;
      memmove(buffer, buffer + characters, size);
      buffer[size] = '\0';
    }

    constexpr size_t find_last_not_of(const char* str) const
    {
      for (int i = size - 1; i >= 0; --i)
      {
        char c = buffer[i];
        bool found = true;
        for (size_t j = 0; j < strlen(str); ++j)
        {
          if (c == str[j])
          {
            found = false;
            break;
          }
        }
        if (found)
        {
          return (size_t)i;
        }
      }
      return size;
    }

    // Remove number of characters at end
    constexpr void remove_suffix(size_t characters)
    {
      if (characters > size)
      {
        printf("[String] Trying to remove more characters than size of buffer!\n");
        return;
      }
      for (size_t i = 0; i < characters; ++i)
      {
        buffer[--size] = '\0';
      }
    }

    constexpr size_t find(const char* str, size_t pos = 0) const
    {
      if (!buffer)
      {
        return SIZE_MAX;
      }
      else if (strlen(str) == 0)
      {
        return SIZE_MAX;
      }
      for (size_t i = pos; i < size; ++i)
      {
        bool found = true;
        if (size - pos < strlen(str))
        {
          return SIZE_MAX;
        }
        else if (buffer[i] != str[0])
        {
          continue;
        }
        for (size_t j = 1; j < strlen(str); ++j)
        {
          if (buffer[i+j] != str[j])
          {
            found = false;
            break;
          }
        }
        if (found)
        {
          return i;
        }
      }
      return SIZE_MAX;
    }

    constexpr size_t count(const char* str) const
    {
      size_t count = 0;
      size_t start = 0;
      while (true)
      {
        start = find(str, start);
        if (start == SIZE_MAX)
        {
          return count;
        }
        start += 1;
        ++count;
      }
      return count;
    }

    constexpr BaseString substr(size_t start) const
    {
      if (size == 0)
      {
        return BaseString();
      }
      return BaseString(buffer + start, buffer + size);
    }

    constexpr BaseString substr(size_t start, size_t end) const
    {
      if (size == 0)
      {
        return BaseString();
      }
      return BaseString(buffer + start, buffer + Min(end, size));
    }

    constexpr bool empty() const
    {
      return size == 0;
    }

    constexpr bool starts_with(const char* key) const
    {
      if (size < strlen(key))
      {
        return false;
      }
      for (size_t i = 0; i < strlen(key); ++i)
      {
        if (buffer[i] != key[i])
        {
          return false;
        }
      }
      return true;
    }
  };

  template<MemoryManager::Data::Type Type = MemoryManager::Data::Type::TEMP>
  inline std::ostream& operator<<(std::ostream& os, const BaseString<Type>& string)
  {
    os << string.buffer;
    return os;
  }

  typedef BaseString<MemoryManager::Data::Type::SCENE_ARENA> SceneString;
  typedef BaseString<MemoryManager::Data::Type::GLOBAL_ARENA> GlobalString;
  typedef BaseString<MemoryManager::Data::Type::THREAD_TEMP> ThreadedString;
  typedef BaseString<> String;
}