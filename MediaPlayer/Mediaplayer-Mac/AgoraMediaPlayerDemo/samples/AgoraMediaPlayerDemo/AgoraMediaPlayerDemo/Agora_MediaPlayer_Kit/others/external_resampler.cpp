
#include "external_resampler.h"
#include "push_resampler.h"
#include "simple_resampler.h"

using namespace AgoraExternalResampler;

external_resampler::external_resampler()
{
  PushResampler* pTmp = new PushResampler();
  resampler = reinterpret_cast<void*>(pTmp);
}

external_resampler::~external_resampler()
{
  PushResampler* pTmp = reinterpret_cast<PushResampler*>(resampler);
  delete pTmp;
  resampler = nullptr;
}

int external_resampler::do_resample(short* in,
                                    int in_samples,
                                    int in_channels,
                                    int in_samplerate,
                                    short* out,
                                    int out_samples,
                                    int out_channels,
                                    int out_samplerate)
{
  PushResampler* pTmp = reinterpret_cast<PushResampler*>(resampler);
  return Simple_Resampler::ProcessResample(in,
                                           in_samples,
                                           in_channels,
                                           in_samplerate,
                                           out,
                                           out_samples,
                                           out_channels,
                                           out_samplerate,
                                           *pTmp);
}

