
#pragma once

#include "Harmonics.hpp"

namespace nup
{
	/** �{���񃌃��_�����O�֐��ɓn���p�����[�^
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

	/** �{���񃌃��_�����O�֐�
	*/
	std::vector<float> RenderHarmonics(const RENDER_HARMONICS_PARAMETER& param);
};