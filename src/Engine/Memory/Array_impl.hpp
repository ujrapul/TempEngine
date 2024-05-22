// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "Logger.hpp"
#include "MemoryManager.hpp"
#include "MemoryUtils.hpp"
#include "String.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <initializer_list>
#include <mutex>

namespace Temp
{
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr Array<T, SizeOfArray, Type>::Array() : size(SizeOfArray)
  {
    Initialize();
    Fill(T());
  }

  // Copy constructor
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  Array<T, SizeOfArray, Type>::Array(const Array& other) noexcept
  {
    if (!other.buffer)
    {
      if (buffer)
      {
        MemoryManager::data.Free(Type, prevOffset);
        prevOffset = 0;
        size = 0;
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
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, other.size * sizeof(T)));
    }
    else
    {
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, other.size * sizeof(T)));
    }

    for (size_t i = 0; i < other.size; ++i)
    {
      buffer[i] = other.buffer[i];
    }
    
    size = other.size;
  }

  // Move constructor
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr Array<T, SizeOfArray, Type>::Array(Array&& other) noexcept
    : Array()
  {
    Swap(*this, other);
  }

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  Array<T, SizeOfArray, Type>::~Array()
  {
    if (buffer)
    {
      for (int i = (size_t)size - 1; i >= 0; --i)
      {
        buffer[i].~T();
      }
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

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr void Array<T, SizeOfArray, Type>::Fill(const T& value)
  {
    for (size_t i = 0; i < size; ++i)
    {
      buffer[i] = value;
    }
  }

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr T& Array<T, SizeOfArray, Type>::operator[](size_t index)
  {
    return buffer[index];
  }

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr const T& Array<T, SizeOfArray, Type>::operator[](size_t index) const
  {
    return buffer[index];
  }

  // Does a copy operation
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr Array<T, SizeOfArray, Type>& Array<T, SizeOfArray, Type>::operator=(Array other)
  {
    Swap(*this, other);
    return *this;
  }

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr bool Array<T, SizeOfArray, Type>::operator==(const Array& other) const
  {
    bool membersSame = prevOffset == other.prevOffset
      && size == other.size;

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

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  inline void Array<T, SizeOfArray, Type>::Initialize()
  {
    if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
    {
      std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, size * sizeof(T)));
    }
    else
    {
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, size * sizeof(T)));
    }
  }

  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr void Array<T, SizeOfArray, Type>::Swap(Array& first, Array& second)
  {
    Utils::Swap(first.buffer, second.buffer);
    Utils::Swap(first.prevOffset, second.prevOffset);
    Utils::Swap(first.size, second.size);
  }
  
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type>
  constexpr void Array<T, SizeOfArray, Type>::swap(Array& other)
  {
    Swap(*this, other);
  }

  ////////////////////////////////////////////
  //////////// DYNAMIC ARRAY /////////////////
  ////////////////////////////////////////////

  // Lazy Initialize Dynamic Array
  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>::DynamicArray(bool initialize, size_t _capacity, T value) noexcept
    : capacity(_capacity)
  {
    if (initialize)
    {
      Initialize();
      //Fill(value);
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>::DynamicArray(T* bufferStart, T* bufferEnd) noexcept
      : size(bufferEnd - bufferStart),
        capacity(Max(size * 2, 8ul))
  {
    Initialize();
    for (size_t i = 0; i < size; ++i)
    {
      buffer[i] = bufferStart[i];
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>::DynamicArray(T* elements, size_t _size) noexcept
      : size(_size), capacity(Max(size * 2, 8ul))
  {
    Initialize();
    for (size_t i = 0; i < size; ++i)
    {
      buffer[i] = elements[i];
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>::DynamicArray(std::initializer_list<T> list) noexcept
      : size(list.size()), capacity(Max(size * 2, 8ul))
  {
    Initialize();
    size_t i = 0;
    for (const auto& item : list)
    {
      buffer[i++] = item;
    }
  }

  // Copy constructor
  template <typename T, MemoryManager::Data::Type Type>
  DynamicArray<T, Type>::DynamicArray(const DynamicArray& other) noexcept
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
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, other.capacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }
    else
    {
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, other.capacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }

    for (size_t i = 0; i < other.size; ++i)
    {
      buffer[i] = other.buffer[i];
    }
    
    capacity = other.capacity;
    size = other.size;
  }

  // Move constructor
  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>::DynamicArray(DynamicArray&& other) noexcept
    : DynamicArray()
  {
    Swap(*this, other);
  }

  template <typename T, MemoryManager::Data::Type Type>
  DynamicArray<T, Type>::~DynamicArray()
  {
    // If buffer is "moved" it shouldn't reach here
    if (buffer)
    {
      for (int i = (size_t)size - 1; i >= 0; --i)
      {
        buffer[i].~T();
      }
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

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::Fill(T value)
  { 
    for (size_t i = 0; i < capacity; ++i)
    {
      buffer[i] = value;
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T& DynamicArray<T, Type>::operator[](size_t index)
  {
    return buffer[index];
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr const T& DynamicArray<T, Type>::operator[](size_t index) const
  {
    return buffer[index];
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::Swap(DynamicArray& first, DynamicArray& second)
  {
    Utils::Swap(first.buffer, second.buffer);
    Utils::Swap(first.capacity, second.capacity);
    Utils::Swap(first.offset, second.offset);
    Utils::Swap(first.prevOffset, second.prevOffset);
    Utils::Swap(first.size, second.size);
  }

  // Does a copy operation
  template <typename T, MemoryManager::Data::Type Type>
  constexpr DynamicArray<T, Type>& DynamicArray<T, Type>::operator=(DynamicArray other)
  {
    Swap(*this, other);
    return *this;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr bool DynamicArray<T, Type>::operator==(const DynamicArray& other) const
  {
    bool membersSame = offset == other.offset
      && prevOffset == other.prevOffset
      && capacity == other.capacity
      && size == other.size;

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

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T* DynamicArray<T, Type>::begin()
  {
    return buffer;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T* DynamicArray<T, Type>::end()
  {
    return buffer + size;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr const T* DynamicArray<T, Type>::begin() const
  {
    return buffer;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr const T* DynamicArray<T, Type>::end() const
  {
    return buffer + size;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr const T* DynamicArray<T, Type>::cbegin() const
  {
    return buffer;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr const T* DynamicArray<T, Type>::cend() const
  {
    return buffer + size;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::Replace(T* elements, size_t elementsSize)
  {
    if (elementsSize > capacity)
    {
      Reserve((elementsSize) * 2);
    }

    for (size_t i = 0; i < elementsSize; ++i)
    {
      Buffer()[i] = std::move(elements[i]);
    }
    size = elementsSize;
  }

  template <typename T, MemoryManager::Data::Type Type>
  inline void DynamicArray<T, Type>::Initialize()
  {
    if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
    {
      std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, capacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }
    else
    {
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      buffer = static_cast<T*>(MemoryManager::data.Allocate(Type, capacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T* DynamicArray<T, Type>::Buffer()
  {
    if (!buffer)
    {
      Initialize();
      // Fill({});
    }
    return buffer;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::PushBack(T value)
  {
    if (size == capacity)
    {
      Reserve(capacity * 2);
    }
    Buffer()[size++] = std::move(value);
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::PushBackUnique(T value)
  {
    if (Find(value) != SIZE_MAX)
    {
      return;
    }
    if (size == capacity)
    {
      Reserve(capacity * 2);
    }
    Buffer()[size++] = std::move(value);
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::PopBack()
  {
    if (size == 0)
    {
      Logger::LogErr("Cannot pop back for this DynamicArray anymore!");
      throw std::exception();
    }
    back().~T();
    Buffer()[--size] = {};
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::InsertEnd(std::initializer_list<T> elements)
  {
    if ((elements.size() + size) > capacity)
    {
      Reserve((elements.size() + size) * 2);
    }
    
    for (const auto& element : elements)
    {
      Buffer()[size] = std::move(element);
      ++size;
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::InsertEnd(T* elements, size_t elementsSize)
  {
    if ((elementsSize + size) > capacity)
    {
      Reserve((elementsSize + size) * 2);
    }
    
    for (size_t i = 0; i < elementsSize; ++i)
    {
      Buffer()[size] = std::move(elements[i]);
      ++size;
    }
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::Remove(const T& element)
  {
    size_t pos = Find(element);
    if (pos == SIZE_MAX)
    {
      return;
    }
    for (size_t i = pos + 1; i < size; ++i)
    {
      Buffer()[i - 1] = std::move(Buffer()[i]);
    }
    Buffer()[--size] = {};
  }

  template <typename T, MemoryManager::Data::Type Type>
  inline void DynamicArray<T, Type>::Resize(size_t newSize)
  {
    if (newSize >= capacity)
    {
      Reserve(newSize * 2);
    }

    for (size_t i = size; i < newSize; ++i)
    {
      PushBack({});
    }
    size = newSize;
  }

  template <typename T, MemoryManager::Data::Type Type>
  inline void DynamicArray<T, Type>::Reserve(size_t newCapacity)
  {
    if (newCapacity == 0 || newCapacity <= capacity)
    {
      return;
    }

    if (buffer)
    {
      MemoryManager::data.Free(Type, prevOffset);
    }
    T* newBuffer;
    if constexpr (Type == MemoryManager::Data::Type::THREAD_TEMP)
    {
      std::lock_guard<std::mutex> lock(MemoryManager::data.mtx);
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      newBuffer = static_cast<T*>(MemoryManager::data.Allocate(Type, newCapacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }
    else
    {
      prevOffset = MemoryManager::data.GetCurrentOffset(Type);
      newBuffer = static_cast<T*>(MemoryManager::data.Allocate(Type, newCapacity * sizeof(T)));
      offset = MemoryManager::data.GetCurrentOffset(Type);
    }
    if (buffer)
    {
      for (size_t i = 0; i < size; ++i)
      {
        newBuffer[i] = std::move(buffer[i]);
      }
    }

    buffer = newBuffer;
    capacity = newCapacity;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T& DynamicArray<T, Type>::back()
  {
    return buffer[size - 1];
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr T& DynamicArray<T, Type>::front()
  {
    return buffer[0];
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::Clear()
  {
    for (size_t i = 0; i < size; ++i)
    {
      buffer[i].~T();
    }
    size = 0;
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr void DynamicArray<T, Type>::swap(DynamicArray& other)
  {
    Swap(*this, other);
  }

  template <typename T, MemoryManager::Data::Type Type>
  constexpr size_t DynamicArray<T, Type>::Find(const T& value) const
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (buffer[i] == value)
      {
        return i;
      }
    }
    return SIZE_MAX;
  }
}