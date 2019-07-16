/*
*  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include "audio_util.h"

#include "typedefs.h"

namespace AgoraExternalResampler {

  void Deinterleave(const int16_t* interleaved, int samples_per_channel,
    int num_channels, int16_t** deinterleaved) {
    for (int i = 0; i < num_channels; i++) {
      int16_t* channel = deinterleaved[i];
      int interleaved_idx = i;
      for (int j = 0; j < samples_per_channel; j++) {
        channel[j] = interleaved[interleaved_idx];
        interleaved_idx += num_channels;
      }
    }
  }

  void Interleave(const int16_t* const* deinterleaved, int samples_per_channel,
    int num_channels, int16_t* interleaved) {
    for (int i = 0; i < num_channels; ++i) {
      const int16_t* channel = deinterleaved[i];
      int interleaved_idx = i;
      for (int j = 0; j < samples_per_channel; j++) {
        interleaved[interleaved_idx] = channel[j];
        interleaved_idx += num_channels;
      }
    }
  }

  void FloatS16ToS16(const float* src, size_t size, int16_t* dest) {
    for (size_t i = 0; i < size; ++i) {
      dest[i] = FloatS16ToS16(src[i]);
    }
  }

  void FloatToS16(const float* src, size_t size, int16_t* dest) {
    for (size_t i = 0; i < size; ++i) {
      dest[i] = FloatToS16(src[i]);
    }
  }

  void S16ToFloat(const int16_t* src, size_t size, float* dest) {
    for (size_t i = 0; i < size; ++i) {
      dest[i] = S16ToFloat(src[i]);
    }
  }

  void SaturateAddS16(int16_t* src1, const int16_t* src2, size_t size) {
    for (size_t i = 0; i < size; ++i) {
      src1[i] = SaturateAddS16(src1[i], src2[i]);
    }
  }

  void SaturateAddS16_scale(int16_t* src1,  int16_t* src2, size_t size, int scale) {
	  for (size_t i = 0; i < size; ++i) {
		  src2[i] = src2[i] >> scale;
		  src1[i] = SaturateAddS16(src1[i], src2[i]);
	  }
  }

}  // namespace AgoraExternalResampler
