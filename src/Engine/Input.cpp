// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "Input.hpp"
#include "Event.hpp"
#include "Scene.hpp"

#include <algorithm>
#include <iostream>

// #define test_bit(bit, array)    ((array)[(bit)/8] & (1 << ((bit)%8)))

namespace Temp::Input
{
  namespace
  {
    constexpr void ActivateCallBack(KeyboardCode keyCode,
                                    std::array<std::vector<void (*)(KeyboardCode)>, 256>& keyEvents)
    {
      for (auto fn : keyEvents[static_cast<int>(keyCode)])
      {
        fn(keyCode);
      }
    }

    struct KeyQueue
    {
      std::queue<KeyboardCode> pressQueue;
      std::queue<KeyboardCode> releaseQueue;
    };

    KeyQueue keyQueue;
  }

  void PushPressKeyQueue(KeyboardCode keyCode)
  {
    keyQueue.pressQueue.push(keyCode);
  }

  void PushReleaseKeyQueue(KeyboardCode keyCode)
  {
    keyQueue.releaseQueue.push(keyCode);
  }

  KeyboardCode PopKeyQueue(std::queue<KeyboardCode>& _keyQueue)
  {
    if (_keyQueue.size() == 0)
    {
      return KeyboardCode::KB_MAX;
    }
    KeyboardCode keyCode = _keyQueue.front();
    _keyQueue.pop();
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
    std::vector<void (*)(KeyboardCode)>& keyEvents = data.pressKeyEvents[static_cast<int>(keyCode)];
    if (std::find(keyEvents.begin(), keyEvents.end(), FnPtr) != keyEvents.end())
    {
      return;
    }
    keyEvents.push_back(FnPtr);
  }

  void RemovePressCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    std::vector<void (*)(KeyboardCode)>& keyEvents = data.pressKeyEvents[static_cast<int>(keyCode)];
    auto iterator = std::find(keyEvents.begin(), keyEvents.end(), FnPtr);
    if (iterator != keyEvents.end())
    {
      keyEvents.erase(iterator);
    }
  }

  void AddReleaseCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    std::vector<void (*)(KeyboardCode)>& keyEvents = data
                                                       .releaseKeyEvents[static_cast<int>(keyCode)];
    if (std::find(keyEvents.begin(), keyEvents.end(), FnPtr) != keyEvents.end())
    {
      return;
    }
    keyEvents.push_back(FnPtr);
  }

  void RemoveReleaseCallback(void (*FnPtr)(KeyboardCode), Data& data, KeyboardCode keyCode)
  {
    std::vector<void (*)(KeyboardCode)>& keyEvents = data
                                                       .releaseKeyEvents[static_cast<int>(keyCode)];
    auto iterator = std::find(keyEvents.begin(), keyEvents.end(), FnPtr);
    if (iterator != keyEvents.end())
    {
      keyEvents.erase(iterator);
    }
  }
}
