// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include <atomic>
#include <unordered_set>
#include <string>
#include <vector>
#include <mutex>

namespace Temp::AudioSystem
{
  struct Data
  {
    std::vector<std::vector<short>> buffers;
    std::vector<float> volumes;
    std::unordered_set<int> currentlyPlayedAudio{};
    std::atomic<bool> stop{false};
    std::mutex mtx{};
  };

  void ReadAudioFiles(Data& data, const std::vector<std::string>& files);
  void PlayAudio(Data& data, int i, std::atomic<bool>& stopHandle);
  void PlayAudioLoop(Data& data, int i, std::atomic<bool>& stopHandle);
  // Should be given as decimal between 0 and 1
  void ChangeMasterVolume(float volume);
  // Should be given as decimal between 0 and 1
  void ChangeVolume(Data& data, int i, float volume);
  void Destruct(Data& data);
}
