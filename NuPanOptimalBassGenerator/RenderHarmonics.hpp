
#pragma once

#include "Harmonics.hpp"

namespace nup
{
	/** 倍音列レンダリング関数に渡すパラメータ
	*/
	struct RENDER_HARMONICS_PARAMETER
	{
		std::vector<HARM> harmonics;
		int64_t bufferLengthInSamples;
		int64_t noteLengthInSamples;
		NoteName noteNmae;	// #TODO typo
		int64_t octave;
		float tempo;
		int64_t sampleRate;
	};

	/** 倍音列レンダリング関数
	*/
	std::vector<float> RenderHarmonics(const RENDER_HARMONICS_PARAMETER& param);
};