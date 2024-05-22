// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

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

    constexpr BaseQueue() {}

    // Copy constructor
    constexpr BaseQueue(const BaseQueue& other) noexcept
    {
      buffer = other.buffer;
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
    }

    constexpr bool operator==(const BaseQueue& other) const
    {
      return buffer == other.buffer;
    }

    inline DynamicArray<T, Type>& Buffer()
    {
      if (!buffer.buffer)
      {
        buffer.Initialize();
        head = buffer.capacity - 1;
        tail = head;
      }
      return buffer;
    }

    inline void Push(T element)
    {
      Buffer()[tail] = std::move(element);
      if (--tail == SIZE_MAX)
      {
        size_t offset = head - tail;
        if (head == buffer.capacity - 1)
        {
          buffer.Reserve(buffer.capacity * 2);
        }
        for (size_t i = 0, end = buffer.capacity - 1; i <= head; ++i, --end)
        {
          buffer[end] = std::move(buffer[i]);
        }
        head = buffer.capacity - 1;
        tail = head - offset;
      }
    }

    template<typename... Args>
    inline void Emplace(Args&&... args)
    {
      Push(std::forward<Args>(args)...);
    }

    inline void Pop()
    {
      assert(head > tail);
      --head;
    }

    constexpr T& Back()
    {
      return buffer[tail];
    }

    constexpr T& Back() const
    {
      return buffer[tail];
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
      return (head - tail) + 1;
    }

    constexpr bool Empty() const
    {
      return head == tail;
    }
  };

  template <typename T>
  using GlobalQueue = BaseQueue<T, MemoryManager::Data::Type::GLOBAL_ARENA>;
  template <typename T>
  using SceneQueue = BaseQueue<T, MemoryManager::Data::Type::SCENE_ARENA>;
  template <typename T>
  using Queue = BaseQueue<T, MemoryManager::Data::Type::TEMP>;
}