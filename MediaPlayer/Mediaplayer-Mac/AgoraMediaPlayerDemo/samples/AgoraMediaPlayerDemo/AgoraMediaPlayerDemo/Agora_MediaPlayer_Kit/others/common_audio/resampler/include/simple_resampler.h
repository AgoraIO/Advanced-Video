/*
 * A wrapper of resampling for Agora audio code.
 */

#ifndef WEBRTC_COMMON_AUDIO_SIMPLE_RESAMPLER_H_
#define WEBRTC_COMMON_AUDIO_SIMPLE_RESAMPLER_H_

#include "typedefs.h"
#include "push_resampler.h"

namespace AgoraExternalResampler
{

class Simple_Resampler
{
public:
  static int ProcessResample(int16_t* in, int32_t inSamplePerChanel, int8_t inChannels, int32_t inSampleRate,
                             int16_t* out, int32_t outSamplePerChannel, int8_t outChannels, int32_t outSampleRate,
                             PushResampler& resampler);
  
  static bool CheckMixParameterValidity(uint32_t sample_rate, uint32_t channels, uint32_t length);
};

}  // namespace AgoraExternalResampler

#endif // WEBRTC_COMMON_AUDIO_SIMPLE_RESAMPLER_H_
