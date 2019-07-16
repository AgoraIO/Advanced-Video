/*
 * A wrapper of resampling for Agora audio code.
 */

#include "common_types.h"
#include "simple_resampler.h"
#include "scoped_ptr.h"
#include "trace.h"

static void MonoToStereo(const int16_t* src_audio,
                         int samples_per_channel,
                         int16_t* dst_audio) {
  for (int i = samples_per_channel - 1; i >= 0; --i) {
    dst_audio[2 * i + 1] = src_audio[i];
    dst_audio[2 * i] = src_audio[i];
  }
}

static void StereoToMono(const int16_t* src_audio,
                         int samples_per_channel,
                         int16_t* dst_audio) {
  for (int i = 0; i < samples_per_channel; i++) {
    dst_audio[i] = ((int32_t)src_audio[2 * i] + (int32_t)src_audio[2 * i + 1]) >> 1;
  }
}

namespace AgoraExternalResampler
{
  
  int Simple_Resampler::ProcessResample(int16_t* in, int32_t inSamplePerChannel, int8_t inChannels, int32_t inSampleRate,
                                        int16_t* out, int32_t outSamplePerChannel, int8_t outChannels, int32_t outSampleRate,
                                        PushResampler& resampler)
  {
    int16_t* inResample = in;
    int16_t* outResample = out;
    int8_t resample_channels = outChannels;
    scoped_array<int16_t> mono_audio;

    if (22050 == inSampleRate) {
      // Special precaution for fs = 22050
      inSampleRate = inSamplePerChannel * 100;
    }

    // If no stereo codecs are in use, we downmix a stereo stream from the
    // device early in the chain, before resampling.
    if (inChannels == 2 && outChannels == 1) {
      mono_audio.reset(new int16_t[inSamplePerChannel]);
      StereoToMono(in, inSamplePerChannel, mono_audio.get());
      inResample = mono_audio.get();
      outResample = out;
      resample_channels = 1;
    }
    else if (inChannels == 1 && outChannels == 2)
    {
      inResample = in;
      resample_channels = 1;
    }
    
    if (inSampleRate <= 0) {
     /* WEBRTC_TRACE(kTraceError, kTraceVoice, 0,
                   "Resample() source sample rate %d < 0",
                   inSampleRate);*/
      return -2;
    }
    if (outSampleRate <= 0) {
      /*WEBRTC_TRACE(kTraceError, kTraceVoice, 0,
                   "Resample() destination sample rate %d < 0",
                   outSampleRate);*/
      return -3;
    }
    
    if (resampler.InitializeIfNeeded(inSampleRate,
                                     outSampleRate,
                                     resample_channels) != 0) {
      /*WEBRTC_TRACE(kTraceError, kTraceVoice, 0,
                   "Resample() unable to resample, src splr %d dst splr %d chans %d",
                   inSampleRate, outSampleRate, resample_channels);*/
      return -4;
    }
    
    int out_length = resampler.Resample(inResample,
                                        inSamplePerChannel * resample_channels,
                                        outResample,
                                        3840);
    if (out_length == -1) {
      /*WEBRTC_TRACE(kTraceError, kTraceVoice, 0,
                   "Resample() resampling failed");*/
      return -5;
    }
    
    if(inChannels == 1 && outChannels == 2 )
    {
      MonoToStereo(outResample, out_length, out);

      out_length *= 2;
    }
    return out_length / resample_channels;
  }
  
  bool Simple_Resampler::CheckMixParameterValidity(uint32_t sample_rate, uint32_t channels, uint32_t length)
  {
    if (sample_rate != 8000 && sample_rate != 16000 && sample_rate != 11025 && sample_rate != 22050  && sample_rate != 24000 && sample_rate != 32000 && sample_rate != 44100 && sample_rate != 48000) {
      return false;
    }
    if (channels != 1 && channels != 2 && channels != 6) {
      return false;
    }
    if ((sample_rate * channels / 100) != length) {
      return false;
    }
    return true;
  }
}  // namespace AgoraExternalResampler
