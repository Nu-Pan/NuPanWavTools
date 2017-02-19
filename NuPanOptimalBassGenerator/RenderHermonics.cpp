
#include "stdafx.hpp"

#include "RenderHarmonics.hpp"

using namespace std;
using namespace nup;

namespace
{
	/* #TODO
	- 演算の精度の問題として、周波数や周期の扱い方はこれでいいのか。
	- あちこちに無駄な計算がある
	- 部分的に double で計算したほうが良いかも
	*/

	// ノート名から周波数を計算
	float NoteNameToFrequency(NoteName noteName, int64_t octave)
	{
		static float FREQ_A4 = 440.0f;

		octave -= 4;
		const float baseFreq = FREQ_A4 * std::pow(2, octave);
		const float octaveRange = (baseFreq * 2.f) - baseFreq;
		return baseFreq + (octaveRange * ((float)noteName / 12.f));
	}

	// サンプルのインデックスから正弦波に与える位相[rad]を計算
	float SampleIndexToPhaseInRadian(float freq, int64_t sampleIndex, int64_t sampleRate)
	{
		const float tick = 1.f / freq;
		const float offsetInSec = (float)sampleIndex / (float)sampleRate;
		float normOffsetInSec = offsetInSec;
		while (tick < normOffsetInSec)
		{
			normOffsetInSec -= tick;
		}
		return 2.f * M_PI * (normOffsetInSec / tick);
	}
}

/** 倍音列レンダリング関数
*/
std::vector<float> nup::RenderHarmonics(const RENDER_HARMONICS_PARAMETER& param)
{
	// #TODO とりあえず F2 あたりの音程で wav 出力

	// バッファを用意
	vector<float> samples(param.bufferLengthInSamples);
	std::fill(samples.begin(), samples.end(), 0.f);

	// ターゲットの周波数を計算
	const float noteFreq = NoteNameToFrequency(param.noteNmae, param.octave);

	// #TODO 波形の頭が 1/128 遅れるように位相オフセットを計算
	float phaseOffset = 0.f;

	// #TODO 倍音位相と倍音強度を元に波形を合成
	for (const auto& harm : param.harmonics)
	{
		for (int64_t sampleIndex = 0; sampleIndex < param.noteLengthInSamples; ++sampleIndex)
		{
			const float harmFreq = noteFreq * harm.order;
			const float theta = SampleIndexToPhaseInRadian(harmFreq, sampleIndex, param.sampleRate);
			samples[sampleIndex] += std::sin(theta) * harm.amplitude;
		}
	}

	return samples;
}
