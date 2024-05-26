// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Input.hpp"
#include "Queue.hpp"

// #define test_bit(bit, array)    ((array)[(bit)/8] & (1 << ((bit)%8)))

namespace Temp::Input
{
  namespace
  {
    constexpr void ActivateCallBack(KeyboardCode keyCode,
                                    GlobalDynamicArray<void (*)(KeyboardCode)>* keyEvents)
    {
      for (auto fn : keyEvents[static_cast<int>(keyCode)])
      {
        fn(keyCode);
      }
    }

    struct KeyQueue
    {
      GlobalQueue<KeyboardCode> pressQueue;
      GlobalQueue<KeyboardCode> releaseQueue;
    };

    KeyQueue keyQueue;
  }

  void PushPressKeyQueue(KeyboardCode keyCode)
  {
    keyQueue.pressQueue.Push(keyCode);
  }

  void PushReleaseKeyQueue(KeyboardCode keyCode)
  {
    keyQueue.releaseQueue.Push(keyCode);
  }

  KeyboardCode PopKeyQueue(GlobalQueue<KeyboardCode>& _keyQueue)
  {
    if (_keyQueue.Empty())
    {
      return KeyboardCode::KB_MAX;
    }
    KeyboardCode keyCode = _keyQueue.Front();
    _keyQueue.Pop();
    return keyCode;
  }

  void Process(Data& data)
  {
    KeyboardCode keyCode = PopKeyQueue(keyQueue.pressQueue);
    if (keyCode != KeyboardCode::KB_MAX)
    {
      ActivateCallBack(keyCode, data.pressKeyEvents);
    }

    keyCode = PopKeyQueue(keyQueue.releaseQueue);
    if (keyCode != KeyboardCode::KB_MAX)
    {
      ActivateCallBack(keyCode, data.releaseKeyEvents);
    }
  }

  void AddPressCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    GlobalDynamicArray<void (*)(KeyboardCode)>& keyEvents = data.pressKeyEvents[static_cast<int>(keyCode)];
    if (std::find(keyEvents.begin(), keyEvents.end(), FnPtr) != keyEvents.end())
    {
      return;
    }
    keyEvents.PushBack(FnPtr);
  }

  void RemovePressCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    GlobalDynamicArray<void (*)(KeyboardCode)>& keyEvents = data.pressKeyEvents[static_cast<int>(keyCode)];
    auto pos = keyEvents.Find(FnPtr);
    if (pos != SIZE_MAX)
    {
      if (pos < (keyEvents.size - 1))
      {
        keyEvents[pos] = std::move(keyEvents.back());
      }
      keyEvents.PopBack();
    }
  }

  void AddReleaseCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    GlobalDynamicArray<void (*)(KeyboardCode)>& keyEvents = data.releaseKeyEvents[static_cast<int>(keyCode)];
    auto pos = keyEvents.Find(FnPtr);
    if (pos != SIZE_MAX)
    {
      return;
    }
    keyEvents.PushBack(FnPtr);
  }

  void RemoveReleaseCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    GlobalDynamicArray<void (*)(KeyboardCode)>& keyEvents = data.releaseKeyEvents[static_cast<int>(keyCode)];
    auto pos = keyEvents.Find(FnPtr);
    if (pos != SIZE_MAX)
    {
      if (pos < (keyEvents.size - 1))
      {
        keyEvents[pos] = std::move(keyEvents.back());
      }
      keyEvents.PopBack();
    }
  }
}
