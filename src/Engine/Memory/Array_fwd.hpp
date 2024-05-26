// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "MemoryManager.hpp"

namespace Temp
{
  template <typename T, int SizeOfArray, MemoryManager::Data::Type Type = MemoryManager::Data::Type::TEMP>
  struct Array
  {
    T* buffer{nullptr};
    size_t size{};
    size_t prevOffset{};

    constexpr Array();

    // Copy constructor
    Array(const Array& other) noexcept;

    // Move constructor
    constexpr Array(Array&& other) noexcept;

    ~Array();

    constexpr void Fill(const T& value);

    constexpr T& operator[](size_t index);

    constexpr const T& operator[](size_t index) const;

    // Does a copy operation
    constexpr Array& operator=(Array other);

    constexpr bool operator==(const Array& other) const;

    inline void Initialize();

    constexpr void Swap(Array& first, Array& second);
   
    constexpr void swap(Array& other);

    constexpr T* begin();

    constexpr T* end();

    constexpr const T* begin() const;

    constexpr const T* end() const;

    constexpr const T* cbegin() const;

    constexpr const T* cend() const;
  };

  template<typename T, int SizeOfArray>
  using GlobalArray = Array<T, SizeOfArray, MemoryManager::Data::Type::GLOBAL_ARENA>;
  template<typename T, int SizeOfArray>
  using SceneArray = Array<T, SizeOfArray, MemoryManager::Data::Type::SCENE_ARENA>;
  template<typename T, int SizeOfArray>
  using ThreadedArray = Array<T, SizeOfArray, MemoryManager::Data::Type::THREAD_TEMP>;

  template <typename T, MemoryManager::Data::Type Type = MemoryManager::Data::Type::TEMP>
  struct DynamicArray
  {
    T* buffer{nullptr};
    size_t offset{0};
    size_t prevOffset{0};
    size_t size{0};
    size_t capacity{8};

    template<typename S>
    constexpr S Max(S a, S b)
    {
      return a > b ? a : b;
    }

    template<typename S>
    constexpr S Min(S a, S b)
    {
      return a < b ? a : b;
    }

    // Lazy Initialize Dynamic Array
    constexpr DynamicArray(bool initialize = false, size_t _capacity = 8, T value = {}) noexcept;

    constexpr DynamicArray(T* bufferStart, T* bufferEnd) noexcept;

    constexpr DynamicArray(T* elements, size_t _size) noexcept;

    constexpr DynamicArray(std::initializer_list<T> list) noexcept;

    // Copy constructor
    DynamicArray(const DynamicArray& other) noexcept;

    // Move constructor
    constexpr DynamicArray(DynamicArray&& other) noexcept;

    ~DynamicArray();

    constexpr void Fill(T value);

    constexpr T& operator[](size_t index);

    constexpr const T& operator[](size_t index) const;

    constexpr void Swap(DynamicArray& first, DynamicArray& second);

    // Does a copy operation
    constexpr DynamicArray& operator=(DynamicArray other);

    constexpr bool operator==(const DynamicArray& other) const;

    constexpr T* begin();

    constexpr T* end();

    constexpr const T* begin() const;

    constexpr const T* end() const;

    constexpr const T* cbegin() const;

    constexpr const T* cend() const;

    constexpr void Replace(T* elements, size_t elementsSize);

    inline void Initialize();

    constexpr T* Buffer();

    constexpr void PushBack(T value);

    constexpr void PushBackUnique(T value);

    constexpr void PopBack();

    constexpr void InsertEnd(std::initializer_list<T> elements);

    constexpr void InsertEnd(T* elements, size_t elementsSize);

    constexpr void Remove(const T& element);

    inline void Resize(size_t newSize);

    inline void Reserve(size_t newCapacity);

    constexpr T& back();

    constexpr T& front();

    constexpr void Clear();

    constexpr void swap(DynamicArray& other);

    constexpr size_t Find(const T& value) const;
  };

  template<typename T>
  using GlobalDynamicArray = DynamicArray<T, MemoryManager::Data::Type::GLOBAL_ARENA>;
  template<typename T>
  using SceneDynamicArray = DynamicArray<T, MemoryManager::Data::Type::SCENE_ARENA>;
  template<typename T>
  using ThreadedDynamicArray = DynamicArray<T, MemoryManager::Data::Type::THREAD_TEMP>;
}