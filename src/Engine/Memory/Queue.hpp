// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "MemoryUtils.hpp"

namespace Temp
{
  template <typename T, MemoryManager::Data::Type Type = MemoryManager::Data::TEMP>
  struct BaseQueue
  {
    DynamicArray<T, Type> buffer;
    size_t head{0};
    size_t tail{0};

    constexpr BaseQueue(bool initialize = false, size_t _capacity = 8)
    {
      if (initialize)
      {
        Reserve(_capacity);
      }
    }

    // Copy constructor
    constexpr BaseQueue(const BaseQueue& other) noexcept
    {
      buffer = other.buffer;
      head = other.head;
      tail = other.tail;
    }

    // Move constructor
    constexpr BaseQueue(BaseQueue&& other) noexcept
      : BaseQueue()
    {
      Swap(*this, other);
    }

    // Does a copy operation
    constexpr BaseQueue& operator=(BaseQueue other)
    {
      Swap(*this, other);
      return *this;
    }

    constexpr void Swap(BaseQueue& first, BaseQueue& second)
    {
      Utils::Swap(first.buffer, second.buffer);
      Utils::Swap(first.head, second.head);
      Utils::Swap(first.tail, second.tail);
    }

    constexpr bool operator==(const BaseQueue& other) const
    {
      return buffer == other.buffer && head == other.head && tail == other.tail;
    }

    inline DynamicArray<T, Type>& Buffer()
    {
      if (!buffer.buffer)
      {
        buffer.Initialize();
        head = 0;
        tail = 0;
      }
      return buffer;
    }

    inline void Reserve(size_t newCapacity)
    {
      Buffer().Reserve(newCapacity);
      head = 0;
      tail = 0;
    }

    inline void Push(T element)
    {
      Buffer()[tail] = std::move(element);
      if (tail + 1 == buffer.capacity)
      {
        size_t offset = head;
        if (head == 0)
        {
          buffer.size = tail + 1;
          buffer.Reserve(buffer.capacity * 2);
        }
        else
        {
          for (size_t i = 0; i < Size(); ++i)
          {
            buffer[i] = std::move(buffer[head + i]);
          }
          head = 0;
          tail -= offset;
        }
      }
      ++tail;
    }

    template<typename... Args>
    inline void Emplace(Args&&... args)
    {
      Push(std::forward<Args>(args)...);
    }

    inline void Pop()
    {
      ++head;
    }

    constexpr T& Back()
    {
      return buffer[tail - 1];
    }

    constexpr T& Back() const
    {
      return buffer[tail - 1];
    }

    constexpr T& Front()
    {
      return buffer[head];
    }

    constexpr T& Front() const
    {
      return buffer[head];
    }

    constexpr size_t Size()
    {
      return (tail - head) + 1;
    }

    constexpr bool Empty() const
    {
      return head == tail;
    }

    constexpr void Clear()
    {
      head = 0;
      tail = 0;
    }
  };

  template <typename T>
  using GlobalQueue = BaseQueue<T, MemoryManager::Data::Type::GLOBAL_ARENA>;
  template <typename T>
  using SceneQueue = BaseQueue<T, MemoryManager::Data::Type::SCENE_ARENA>;
  template <typename T>
  using Queue = BaseQueue<T, MemoryManager::Data::Type::TEMP>;
}