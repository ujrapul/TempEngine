// SPDX-FileCopyrightText: 2024 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// Made with reference from: https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

#pragma once

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

typedef long unsigned int size_t;

namespace Temp::LinearAllocator
{
  struct Data
  {
    unsigned char* buffer{nullptr};
    size_t length{0};
    size_t offset{0};
  };

  struct TempData
  {
    Data* data{nullptr};
    size_t offset{SIZE_MAX};
  };

  inline bool IsPowerOfTwo(uintptr_t x)
  {
    return (x & (x-1)) == 0;
  }

  template<typename T>
  constexpr T Max(T a, T b) { return a > b ? a : b; }

  template<typename T>
  constexpr T Min(T a, T b) { return a < b ? a : b; }

  inline uintptr_t AlignForward(uintptr_t ptr, size_t align)
  {
    uintptr_t p, a, modulo;

    assert(IsPowerOfTwo(align));

    p = ptr;
    a = (uintptr_t)align;
    // Same as (p % a) but faster as 'a' is a power of two
    modulo = p & (a-1);

    if (modulo != 0) {
      // If 'p' address is not aligned, push the address to the
      // next value which is aligned
      p += a - modulo;
    }
    return p;
  }

  inline uintptr_t AlignBackward(uintptr_t ptr, size_t align)
  {
    uintptr_t p, a, modulo;

    assert(IsPowerOfTwo(align));

    p = ptr;
    a = (uintptr_t)align;
    // Same as (p % a) but faster as 'a' is a power of two
    modulo = p & (a-1);

    if (modulo != 0) {
      // If 'p' address is not aligned, push the address to the
      // previous value which is aligned
      p -= modulo;
    }
    return p;
  }

  inline void* Alloc(LinearAllocator::Data& a, size_t size, size_t align = 2 * sizeof(void *))
  {
    assert(size > 0);

    // Align 'curr_offset' forward to the specified alignment
    uintptr_t curr_ptr = (uintptr_t)a.buffer + (uintptr_t)a.offset;
    uintptr_t offset = AlignForward(curr_ptr, align);

    offset -= (uintptr_t)a.buffer; // Change to relative offset

    // Check to see if the backing memory has space left
    if (offset+size <= a.length) {
      void *ptr = &a.buffer[offset];
      a.offset = offset+size;

      // Zero new memory by default
      memset(ptr, 0, size);
      return ptr;
    }

    printf("Arena out of memory for Arena Allocation!\n");
    assert(false);
    // Return NULL if the arena is out of memory (or handle differently)
    return NULL;
  }

  inline void FreeAll(LinearAllocator::Data& a)
  {
    a.offset = 0;
  }

  inline TempData TempMemoryBegin(LinearAllocator::Data& a)
  {
    TempData temp;
    temp.data = &a;
    temp.offset = a.offset;
    return temp;
  }

  inline void TempMemoryEnd(TempData& temp)
  {
    if (temp.data)
    {
      temp.data->offset = Min(temp.offset, temp.data->offset);
    }
  }

  inline void Init(LinearAllocator::Data& a, void* backingBuffer, size_t bufferLength)
  {
    a.buffer = (unsigned char *)backingBuffer;
    a.length = bufferLength;
    a.offset = 0;
  }
}