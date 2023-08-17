// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "AudioSystem.hpp"

#include "Engine.hpp"
#include "EngineUtils.hpp"
#include "Logger.hpp"
#include "ThreadPool.hpp"
#ifdef __linux__
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#elif __APPLE__
#include <AudioToolbox/AUComponent.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xaudio2.h>
#endif
#include <atomic>
#include <climits>
#include <cstddef>
#include <filesystem>
#include <mutex>
#include <sndfile.h>
#include <string>
#include <thread>
#include <vector>

#ifdef __linux__
/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#endif

namespace Temp::AudioSystem
{
  namespace
  {
    std::atomic<float> masterVolume = 1.f;
    ThreadPool::Data threadPool;

    SNDFILE* OpenAudioFile(SF_INFO& sfInfo, const std::string& file)
    {
      SNDFILE* sndFile = nullptr;

      int retry = 0;
      while (!sndFile && retry < 100)
      {
        sndFile = sf_open(
          std::filesystem::path(AssetsDirectory() / "Audio" / file).string().c_str(),
          SFM_READ,
          &sfInfo);
        ++retry;
      }

      if (!sndFile)
      {
        Logger::LogErr("[Audio] Unable to open audio file.");
      }
      return sndFile;
    }

    void ReadAudioFilePart(const std::string& file,
                           sf_count_t position,
                           sf_count_t numSamples,
                           std::vector<short>& audioBuffer)
    {
      SF_INFO sfInfo;
      SNDFILE* sndFile = OpenAudioFile(sfInfo, file);
      if (!sndFile)
      {
        return;
      }

      sf_seek(sndFile, position, SEEK_SET);

      const sf_count_t bufferSize = 131072;
      sf_count_t bufferSamples = bufferSize;
      short buffer[bufferSize];
      sf_count_t numSamplesRead = 0;
      audioBuffer.reserve(numSamples);

      // Will currently get dropped samples for .ogg files (maybe others as well)
      // Prefer using .wav
      while (numSamples > 0)
      {
        numSamplesRead = sf_read_short(sndFile, //
                                       buffer,
                                       Math::Min(bufferSamples, numSamples));
        if (numSamplesRead == 0)
        {
          if (bufferSamples == 1)
          {
            break;
          }
          bufferSamples /= 2;
          continue;
        }
        audioBuffer.insert(audioBuffer.end(),
                           std::make_move_iterator(buffer),
                           std::make_move_iterator(buffer + numSamplesRead));
        numSamples -= numSamplesRead;
      }

      sf_close(sndFile);
    }

    void ReadAudioFile(Data& data, const std::string& file, size_t index)
    {
      SF_INFO sfInfo;
      SNDFILE* sndFile = OpenAudioFile(sfInfo, file);
      if (!sndFile)
      {
        return;
      }

      size_t numThreads = std::thread::hardware_concurrency();
      std::vector<std::thread> threads;
      std::vector<std::vector<short>> audioBuffers;
      audioBuffers.resize(numThreads);
      sf_count_t totalFrames = sfInfo.frames;
      sf_count_t totalSamples = totalFrames * sfInfo.channels;
      sf_count_t samplesPerThread = totalSamples / numThreads;
      sf_count_t framesPerThread = totalFrames / numThreads;
      data.buffers[index].reserve(totalSamples);

      for (size_t i = 0; i < numThreads; ++i)
      {
        sf_count_t position = i * framesPerThread;
        sf_count_t numSamples = (i == numThreads - 1)
                                  ? (totalSamples - samplesPerThread * (numThreads - 1))
                                  : samplesPerThread;

        threads.emplace_back([&file, &audioBuffers, position, numSamples, i]() {
          ReadAudioFilePart(file, position, numSamples, audioBuffers[i]);
        });
      }
      for (auto& thread : threads)
      {
        thread.join();
      }
      for (auto& buffer : audioBuffers)
      {
        data.buffers[index].insert(data.buffers[index].end(),
                                   std::make_move_iterator(buffer.begin()),
                                   std::make_move_iterator(buffer.end()));
      }

      sf_close(sndFile);
    }

#ifdef __linux__
    void PlayAudioSingleLinux(Data& data, int i, std::atomic<bool>& stopHandle)
    {
      snd_pcm_t* handle;
      snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

      snd_pcm_hw_params_t* params;
      unsigned int val;
      snd_pcm_uframes_t frames;
      int rc;
      int dir;

      /* Allocate a hardware parameters object. */
      snd_pcm_hw_params_alloca(&params);

      /* Fill it in with default values. */
      snd_pcm_hw_params_any(handle, params);

      /* Set the desired hardware parameters. */

      /* Interleaved mode */
      snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

      /* Signed 16-bit little-endian format */
      snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

      /* Two channels (stereo) */
      snd_pcm_hw_params_set_channels(handle, params, 2);

      /* 44100 bits/second sampling rate (CD quality) */
      val = 44100;
      snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

      /* Set period size to 128 frames. */
      frames = 128;
      snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

      /* Write the parameters to the driver */
      rc = snd_pcm_hw_params(handle, params);
      if (rc < 0)
      {
        Logger::LogErr(std::string("[Audio] Unable to set hw parameters: %s\n", snd_strerror(rc)));
        return;
      }

      size_t offset = 0;
      const auto& audioBuffer = data.buffers[i];
      while (offset < audioBuffer.size() && !data.stop && !stopHandle)
      {
        // Calculate the remaining frames to write
        size_t remainingFrames = std::min(frames, (audioBuffer.size() - offset) / 2);
        std::vector<short> currentBuffer;
        currentBuffer.resize(remainingFrames * 2);

        {
          std::lock_guard<std::mutex> lock{data.mtx};
          for (size_t frame = offset, j = 0; j < currentBuffer.size(); ++frame, ++j)
          {
            currentBuffer[j] = audioBuffer[frame] * masterVolume * data.volumes[i];
          }
        }

        // Try to write the remaining frames to the PCM device
        rc = snd_pcm_writei(handle, currentBuffer.data(), remainingFrames);

        if (rc == -EPIPE)
        {
          /* EPIPE means underrun */
          Logger::LogErr("[Audio] Underrun occurred: index " + std::to_string(i));
          snd_pcm_prepare(handle);
        }
        else if (rc < 0)
        {
          Logger::LogErr(std::string("[Audio] Error from writei: %s\n", snd_strerror(rc)));
        }
        else
        {
          // Increment the offset by the number of frames written
          offset += rc * 2;
        }
      }

      if (!data.stop)
      {
        snd_pcm_drain(handle);
      }
      snd_pcm_close(handle);
    }

#elif __APPLE__
    //  Core Audio implementation built with reference from James Alvarez's Example Code
    //
    //  Created by James on 02/09/2016.
    //  Original work Copyright © 2016 James Alvarez. All rights reserved.
    //  Modified work Copyright © 2023 Ujwal Vujjini. All rights reserved.
    //
    //  MIT License
    //
    //  Copyright (c) 2016 jamesalvarez
    //
    //  Permission is hereby granted, free of charge, to any person obtaining a copy
    //  of this software and associated documentation files (the "Software"), to deal
    //  in the Software without restriction, including without limitation the rights
    //  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    //  copies of the Software, and to permit persons to whom the Software is
    //  furnished to do so, subject to the following conditions:
    //
    //  The above copyright notice and this permission notice shall be included in all
    //  copies or substantial portions of the Software.
    //
    //  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    //  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    //  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    //  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    //  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    //  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    //  SOFTWARE.
    //
    //  https://jamesalvarez.co.uk/blog/a-simple-introduction-to-core-audio/
    //  https://github.com/jamesalvarez/iosCoreAudioPlayer.git

#define CAP_SAMPLE_RATE 44100
#define CAP_CHANNELS 2
#define CAP_SAMPLE_SIZE sizeof(short)

    AudioStreamBasicDescription const CAPAudioDescription = {
      .mSampleRate = CAP_SAMPLE_RATE,
      .mFormatID = kAudioFormatLinearPCM,
      .mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked,
      .mBytesPerPacket = CAP_SAMPLE_SIZE * CAP_CHANNELS,
      .mFramesPerPacket = 1,
      .mBytesPerFrame = CAP_CHANNELS * CAP_SAMPLE_SIZE,
      .mChannelsPerFrame = CAP_CHANNELS,
      .mBitsPerChannel = 8 * CAP_SAMPLE_SIZE, // 8 bits per byte
      .mReserved = 0,
    };

    // A struct to hold data and playback status
    struct CAPAudioPlayer
    {
      AudioBufferList* bufferList;
      Data* data;
      float* volume;
      UInt32 frames;
      UInt32 currentFrame;
      bool finished{false};
    };

    // A struct to hold information about output status
    struct CAPAudioOutput
    {
      AudioUnit outputUnit;
      CAPAudioPlayer player;
    };

    // generic error handler - if err is nonzero, prints error message and exits program.
    static void CheckError(OSStatus error, const char* operation)
    {
      if (error == noErr)
        return;

      char str[20];
      // see if it appears to be a 4-char-code
      *(UInt32*)(str + 1) = CFSwapInt32HostToBig(error);
      if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4]))
      {
        str[0] = str[5] = '\'';
        str[6] = '\0';
      }

      Logger::LogErr(std::string("[Audio] Error: ") + std::to_string(error) + " " + operation +
                     " " + str);

      Global::Quit();
    }

    static OSStatus CAPRenderProc(void* inRefCon,
                                  AudioUnitRenderActionFlags* /*ioActionFlags*/,
                                  const AudioTimeStamp* /*inTimeStamp*/,
                                  UInt32 /*inBusNumber*/,
                                  UInt32 inNumberFrames,
                                  AudioBufferList* ioData)
    {

      CAPAudioOutput* audioOutput = (CAPAudioOutput*)inRefCon;
      CAPAudioPlayer* audioPlayer = &audioOutput->player;

      UInt32 currentFrame = audioPlayer->currentFrame;
      UInt32 maxFrames = audioPlayer->frames;

      short* outputData = (short*)ioData->mBuffers[0].mData;
      short* inputData = (short*)audioPlayer->bufferList->mBuffers[0].mData;

      for (UInt32 frame = 0; frame < inNumberFrames; ++frame)
      {
        UInt32 outSample = frame * 2;
        UInt32 inSample = currentFrame * 2;

        {
          std::lock_guard<std::mutex> lock{audioPlayer->data->mtx};
          outputData[outSample] = inputData[inSample] * masterVolume * *audioPlayer->volume;
          outputData[outSample + 1] = inputData[inSample + 1] * masterVolume * *audioPlayer->volume;
        }

        if (++currentFrame == maxFrames + 1)
        {
          audioPlayer->finished = true;
          return noErr;
        }
      }

      audioPlayer->currentFrame = currentFrame;

      return noErr;
    }

    void PlayAudioSingleOSX(Data& data, int i, std::atomic<bool>& stopHandle)
    {
      OSStatus status = noErr;

      CAPAudioOutput output;

      // BufferList and readFrames are the audio we loaded
      AudioBufferList bufferList;
      bufferList.mNumberBuffers = 1;
      bufferList.mBuffers[0].mData = data.buffers[i].data();
      bufferList.mBuffers[0].mNumberChannels = 2;
      bufferList.mBuffers[0].mDataByteSize = (unsigned int)data.buffers[i].size();
      output.player.bufferList = &bufferList;
      output.player.frames = (unsigned int)data.buffers[i].size() / 2;
      output.player.data = &data;
      output.player.volume = &data.volumes[i];

      // Description for the output AudioComponent
      AudioComponentDescription outputcd = {.componentType = kAudioUnitType_Output,
                                            .componentSubType = kAudioUnitSubType_DefaultOutput,
                                            .componentManufacturer = kAudioUnitManufacturer_Apple,
                                            .componentFlags = 0,
                                            .componentFlagsMask = 0};

      // Get the output AudioComponent
      AudioComponent comp = AudioComponentFindNext(nullptr, &outputcd);
      if (comp == nullptr)
      {
        Logger::LogErr("[Audio] Can't get output unit");
        return;
      }

      // Create a new instance of the AudioComponent = the AudioUnit
      status = AudioComponentInstanceNew(comp, &output.outputUnit);
      CheckError(status, "Couldn't open component for outputUnit");

      // Set the stream format
      status = AudioUnitSetProperty(output.outputUnit,
                                    kAudioUnitProperty_StreamFormat,
                                    kAudioUnitScope_Input,
                                    0,
                                    &CAPAudioDescription,
                                    sizeof(CAPAudioDescription));
      CheckError(status, "kAudioUnitProperty_StreamFormat");

      // Set the render callback
      AURenderCallbackStruct input = {.inputProc = CAPRenderProc, .inputProcRefCon = &output};

      status = AudioUnitSetProperty(output.outputUnit,
                                    kAudioUnitProperty_SetRenderCallback,
                                    kAudioUnitScope_Global,
                                    0,
                                    &input,
                                    sizeof(input));
      CheckError(status, "Could not set render callback");

      // Set the maximum frames per slice (not necessary)
      UInt32 framesPerSlice = 4096;
      status = AudioUnitSetProperty(output.outputUnit,
                                    kAudioUnitProperty_MaximumFramesPerSlice,
                                    kAudioUnitScope_Global,
                                    0,
                                    &framesPerSlice,
                                    sizeof(framesPerSlice));
      CheckError(status, "AudioUnitSetProperty(kAudioUnitProperty_MaximumFramesPerSlice");

      // Initialize the Audio Unit
      status = AudioUnitInitialize(output.outputUnit);
      CheckError(status, "Couldn't initialize output unit");

      // Start the Audio Unit (sound begins)
      status = AudioOutputUnitStart(output.outputUnit);
      CheckError(status, "Couldn't start output unit");

      while (!data.stop && !stopHandle && !output.player.finished)
      {
        continue;
      }

      AudioOutputUnitStop(output.outputUnit);
      AudioUnitUninitialize(output.outputUnit);
      AudioComponentInstanceDispose(output.outputUnit);
    }
#elif _WIN32
    void PlayAudioSingleWin(Data& data, int i, std::atomic<bool>& stopHandle)
    {
      using namespace std::chrono;

      // Initialize XAudio2
      if (!SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
      {
        Logger::LogErr("[Audio] Failed to initialize xaudio2!");
        return;
      }
      IXAudio2* pXAudio2 = nullptr;
      XAudio2Create(&pXAudio2);

      // Create mastering voice
      IXAudio2MasteringVoice* pMasteringVoice = nullptr;
      pXAudio2->CreateMasteringVoice(&pMasteringVoice);

      // Define audio format
      WAVEFORMATEX wfx = {};
      wfx.wFormatTag = WAVE_FORMAT_PCM;
      wfx.nChannels = 2;          // stereo audio
      wfx.nSamplesPerSec = 44100; // Sample rate
      wfx.wBitsPerSample = sizeof(short) * 8;
      wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
      wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

      // Your vector of shorts (mono audio)
      auto& audioData = data.buffers[i]; // Fill this with your audio data

      // Create source voice
      IXAudio2SourceVoice* pSourceVoice = nullptr;
      pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx);

      size_t bufferSize = 4096;
      size_t offset = 0;

      // Submit audio data to source voice
      XAUDIO2_BUFFER buffer = {};
      buffer.pAudioData = reinterpret_cast<BYTE*>(audioData.data());
      buffer.Flags = XAUDIO2_END_OF_STREAM;
      buffer.AudioBytes = static_cast<UINT32>(bufferSize * sizeof(short));
      offset += bufferSize;

      pSourceVoice->SubmitSourceBuffer(&buffer);

      // Start playing audio
      pSourceVoice->Start(0);

      XAUDIO2_VOICE_STATE voiceState;
      pSourceVoice->GetState(&voiceState);

      // Wait for audio to finish (or handle application exit)
      std::queue<std::vector<short>> bufferQueue;
      while (voiceState.BuffersQueued > 0 && !data.stop && !stopHandle)
      {
        pSourceVoice->GetState(&voiceState);
        //std::cout << voiceState.BuffersQueued << std::endl;

        if (offset < audioData.size() && voiceState.BuffersQueued < 2)
        {
          bufferSize = Math::Min(bufferSize, audioData.size() - offset);
          buffer = {};
          std::vector<short> currentBuffer;
          currentBuffer.resize(bufferSize);

          {
            std::lock_guard<std::mutex> lock{data.mtx};
            for (size_t frame = offset, j = 0; j < currentBuffer.size(); ++frame, ++j)
            {
              currentBuffer[j] = (short)(audioData[frame] * masterVolume * data.volumes[i]);
            }
          }
          bufferQueue.push(currentBuffer);
          if (bufferQueue.size() > 2)
          {
            bufferQueue.pop();
          }
          buffer.pAudioData = reinterpret_cast<BYTE*>(bufferQueue.back().data());
          if (offset + bufferSize >= audioData.size())
          {
            buffer.Flags = XAUDIO2_END_OF_STREAM;
          }
          buffer.AudioBytes = static_cast<UINT32>(bufferSize * sizeof(short));

          pSourceVoice->SubmitSourceBuffer(&buffer);
          offset += bufferSize;
        }
      }

      // Cleanup
      pSourceVoice->Stop(0);
      pSourceVoice->DestroyVoice();
      pMasteringVoice->DestroyVoice();
      pXAudio2->Release();
      CoUninitialize();
    }
#endif

    void PlayAudioSingle(Data& data, int i, std::atomic<bool>& stopHandle)
    {
      if (data.buffers.size() == 0)
      {
        return;
      }

      {
        std::lock_guard<std::mutex> lock{data.mtx};
        if (data.currentlyPlayedAudio.contains(i))
        {
          return;
        }
        data.currentlyPlayedAudio.insert(i);
      }
#ifdef __linux__
      PlayAudioSingleLinux(data, i, stopHandle);
#elif __APPLE__
      PlayAudioSingleOSX(data, i, stopHandle);
#elif _WIN32
      PlayAudioSingleWin(data, i, stopHandle);
#endif
      {
        std::lock_guard<std::mutex> lock{data.mtx};
        data.currentlyPlayedAudio.erase(i);
      }
    }
  }

  void ReadAudioFiles(Data& data, const std::vector<std::string>& files)
  {
    data.buffers.resize(files.size());
    data.volumes.resize(files.size(), 1.f);
    ThreadPool::Initialize(threadPool);
    for (size_t i = 0; i < files.size(); ++i)
    {
      auto f = [&data, &files, i]() { AudioSystem::ReadAudioFile(data, files[i], i); };
      ThreadPool::Enqueue(threadPool, std::move(f));
    }
    ThreadPool::Wait(threadPool);
  }

  void PlayAudio(Data& data, int i, std::atomic<bool>& stopHandle)
  {
    auto f = [&data, i, &stopHandle]() { PlayAudioSingle(data, i, stopHandle); };
    ThreadPool::Enqueue(threadPool, std::move(f));
  }

  void PlayAudioLoop(Data& data, int i, std::atomic<bool>& stopHandle)
  {
    auto f = [&data, i, &stopHandle]() {
      while (!data.stop && !stopHandle)
      {
        PlayAudioSingle(data, i, stopHandle);
      }
    };
    ThreadPool::Enqueue(threadPool, std::move(f));
  }

  void ChangeMasterVolume(float volume) { Temp::AudioSystem::masterVolume = volume; }

  void ChangeVolume(Data& data, int i, float volume)
  {
    std::lock_guard<std::mutex> lock{data.mtx};
    data.volumes[i] = volume;
  }

  void Destruct(Data& data)
  {
    data.stop = true;
    ThreadPool::Destruct(threadPool);
  }
}
