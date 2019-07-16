/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_
#define WEBRTC_COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_

#include "typedefs.h"
#include <cstddef>
#include <limits>
#include <cstddef>

namespace AgoraExternalResampler {

  typedef std::numeric_limits<int16_t> limits_int16;

 template <typename T>
 static inline T coerce(T const& value, T const& min_value, T const& max_value) {
    T ret_value;
    ret_value = (value > min_value) ? (value) : (min_value);
    return ((ret_value < max_value) ? (ret_value) : (max_value));
  }

  // Clamp the floating |value| to the range representable by an int16_t.
  static inline float ClampInt16(float value) {
    const float kMaxInt16 = 32767.f;
    const float kMinInt16 = -32768.f;
    return value < kMinInt16 ? kMinInt16 :
    (value > kMaxInt16 ? kMaxInt16 : value);
  }

  // Return a rounded int16_t of the floating |value|. Doesn't handle overflow;
  // use ClampInt16 if necessary.
  static inline int16_t RoundToInt16(float value) {
    return static_cast<int16_t>(value < 0.f ? value - 0.5f : value + 0.5f);
  }

  // Deinterleave audio from |interleaved| to the channel buffers pointed to
  // by |deinterleaved|. There must be sufficient space allocated in the
  // |deinterleaved| buffers (|num_channel| buffers with |samples_per_channel|
  // per buffer).
  void Deinterleave(const int16_t* interleaved, int samples_per_channel,
                    int num_channels, int16_t** deinterleaved);

  // Interleave audio from the channel buffers pointed to by |deinterleaved| to
  // |interleaved|. There must be sufficient space allocated in |interleaved|
  // (|samples_per_channel| * |num_channels|).
  void Interleave(const int16_t* const* deinterleaved, int samples_per_channel,
                  int num_channels, int16_t* interleaved);

  // The conversion functions use the following naming convention:
  // S16:      int16_t [-32768, 32767]
  // Float:    float   [-1.0, 1.0]
  // FloatS16: float   [-32768.0, 32767.0]
  static inline int16_t FloatToS16(float v) {
    if (v > 0)
      return v >= 1 ? limits_int16::max()
      : static_cast<int16_t>(v * limits_int16::max() + 0.5f);
    return v <= -1 ? limits_int16::min()
    : static_cast<int16_t>(-v * limits_int16::min() - 0.5f);
  }

  static inline float S16ToFloat(int16_t v) {
    static const float kMaxInt16Inverse = 1.f / limits_int16::max();
    static const float kMinInt16Inverse = 1.f / limits_int16::min();
    return v * (v > 0 ? kMaxInt16Inverse : -kMinInt16Inverse);
  }

  static inline int16_t FloatS16ToS16(float v) {
    static const float kMaxRound = limits_int16::max() - 0.5f;
    static const float kMinRound = limits_int16::min() + 0.5f;
    if (v > 0)
      return v >= kMaxRound ? limits_int16::max()
      : static_cast<int16_t>(v + 0.5f);
    return v <= kMinRound ? limits_int16::min() : static_cast<int16_t>(v - 0.5f);
  }

  static inline int16_t SaturateAddS16(int16_t var1, int16_t var2) {
    static const int32_t kMaxInt16 = 32767;
    static const int32_t kMinInt16 = -32768;
    int32_t tmp = (int32_t)var1 + (int32_t)var2;
    int16_t out16;

    if (tmp > kMaxInt16) {
      out16 = kMaxInt16;
    }
    else if (tmp < kMinInt16) {
      out16 = kMinInt16;
    }
    else {
      out16 = (int16_t)tmp;
    }

    return out16;
  }


  static inline float sinf5(float x) {
    static const float c = 0.00729036f;
    static const float b = -0.164983f;
    static const float a = 0.999314f;

    static const float pi2 = 6.2831853f;
    static const float pi = 3.1415926f;
    static const float pi3_2 = 4.71238898f;
    static const float pi_2 = 1.5707963f;

    // Wrap the input value to [0, pi/2]
    if (x > pi3_2) {
      x = x - pi2;
    } else if (x > pi_2) {
      x = pi - x;
    }

    float x2 = x*x;
    float tmp = c * x2 + b;
    tmp = tmp * x2 + a;
    return tmp * x;
  }

  static inline float cosf5(float x) {
    static const float c = 0.00729036f;
    static const float b = -0.164983f;
    static const float a = 0.999314f;

    static const float pi = 3.1415926f;
    static const float pi3_2 = 4.71238898f;
    static const float pi_2 = 1.5707963f;

    // Wrap the input value to [0, pi/2]
    if (x > pi) {
      x = x - pi3_2;
    } else {
      x = pi_2 - x;
    }

    float x2 = x*x;
    float tmp = c * x2 + b;
    tmp = tmp * x2 + a;
    return tmp * x;
  }

  void FloatS16ToS16(const float* src, size_t size, int16_t* dest);
  void FloatToS16(const float* src, size_t size, int16_t* dest);
  void S16ToFloat(const int16_t* src, size_t size, float* dest);
  void SaturateAddS16(int16_t* src1, const int16_t* src2, size_t size);
  void SaturateAddS16_scale(int16_t* src1, int16_t* src2, size_t size, int scale);

}  // namespace AgoraExternalResampler

#endif  // WEBRTC_COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_

