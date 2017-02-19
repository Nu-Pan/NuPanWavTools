
#include "stdafx.hpp"

#include "RenderHarmonics.hpp"

using namespace std;
using namespace nup;

namespace
{
	/* #TODO
	- ���Z�̐��x�̖��Ƃ��āA���g��������̈������͂���ł����̂��B
	- ���������ɖ��ʂȌv�Z������
	- �����I�� double �Ōv�Z�����ق����ǂ�����
	*/

	// �m�[�g��������g�����v�Z
	float NoteNameToFrequency(NoteName noteName, int64_t octave)
	{
		static float FREQ_A4 = 440.0f;

		octave -= 4;
		const float baseFreq = FREQ_A4 * std::pow(2, octave);
		const float octaveRange = (baseFreq * 2.f) - baseFreq;
		return baseFreq + (octaveRange * ((float)noteName / 12.f));
	}

	// �T���v���̃C���f�b�N�X���琳���g�ɗ^����ʑ�[rad]���v�Z
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

/** �{���񃌃��_�����O�֐�
*/
std::vector<float> nup::RenderHarmonics(const RENDER_HARMONICS_PARAMETER& param)
{
	// #TODO �Ƃ肠���� F2 ������̉����� wav �o��

	// �o�b�t�@��p��
	vector<float> samples(param.bufferLengthInSamples);
	std::fill(samples.begin(), samples.end(), 0.f);

	// �^�[�Q�b�g�̎��g�����v�Z
	const float noteFreq = NoteNameToFrequency(param.noteNmae, param.octave);

	// #TODO �g�`�̓��� 1/128 �x���悤�Ɉʑ��I�t�Z�b�g���v�Z
	float phaseOffset = 0.f;

	// #TODO �{���ʑ��Ɣ{�����x�����ɔg�`������
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
