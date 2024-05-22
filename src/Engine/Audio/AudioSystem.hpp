// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Array_fwd.hpp"
#include "String.hpp"
#include "STDPCH.hpp"

const int MAX_AUDIO_FILES = 100;

namespace Temp::AudioSystem
{
  struct Data
  {
    GlobalArray<GlobalDynamicArray<short>, MAX_AUDIO_FILES> buffers;
    GlobalArray<float, MAX_AUDIO_FILES> volumes;
    GlobalArray<bool, MAX_AUDIO_FILES> currentlyPlayedAudio{};
    std::atomic<bool> stop{false};
    std::mutex mtx{};
  };

  void ReadAudioFiles(Data& data, const GlobalDynamicArray<GlobalString>& files);
  void PlayAudio(Data& data, int i, std::atomic<bool>& stopHandle);
  void PlayAudioLoop(Data& data, int i, std::atomic<bool>& stopHandle);
  // Should be given as decimal between 0 and 1
  void ChangeMasterVolume(float volume);
  // Should be given as decimal between 0 and 1
  void ChangeVolume(Data& data, int i, float volume);
  void Destruct(Data& data);
}
