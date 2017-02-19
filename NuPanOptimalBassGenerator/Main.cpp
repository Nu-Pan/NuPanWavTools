
#include "stdafx.hpp"

#include "OptimalHarmonicPhaseEstimator.hpp"
#include "RenderHarmonics.hpp"
#include "NuPanWavIo/WavInternal.hpp"
#include "NuPanWavIo/WavInternalFunctions.hpp"
#include "NuPanWavIo/LpcmData.hpp"
#include "NuPanWavIo/LpcmDataFunction.hpp"

using namespace std;
using namespace nup;

int main(int argc, char *argv[])
{
	typedef float SampleType;
	typedef float PrecisionType;

	static const size_t MAX_HARMONICS_SIZE = 8;
	static const int64_t SAMPLE_RATE = 48000;
	static const float TEMPO = 170;

	// �{���ݒ���L�q
	vector<HARM> harms;
	harms.resize(MAX_HARMONICS_SIZE);
	for (int harmIndex = 0; harmIndex < MAX_HARMONICS_SIZE; ++harmIndex)
	{
		HARM& harm = harms[harmIndex];
		harm.order = harmIndex + 1;
		harm.phase.sample = 0;
		harm.amplitude = 1.f / (float)harm.order;
	}

	// �e�{���̍œK�Ȉʑ������߂�
	EstimateOptimalHarmonicPhase(harms);

	// #DEBUG �v�Z�����{���ʑ����o��
	{
		ofstream ofs("result_phase.csv");
		for (auto harm : harms)
		{
			ofs << harm.phase.radian << endl;
		}
	}

	// #TODO �����̉����Ő���
	// �œK�������{���������ɃT���v���o�b�t�@�𐶐�
	vector<float> samples = [&]()
	{
		RENDER_HARMONICS_PARAMETER param;
		param.harmonics = harms;
		param.bufferLengthInSamples = SAMPLE_RATE * 5;
		param.noteLengthInSamples = SAMPLE_RATE * 4;
		param.noteNmae = NOTE_F;
		param.octave = 0;
		param.tempo = TEMPO;
		param.sampleRate = SAMPLE_RATE;
		return RenderHarmonics(param);
	}();

	// �T���v���o�b�t�@������`�� WAV �ɕϊ�
	LPCM_DATA<float> dataPack;
	dataPack.channels = 1;
	dataPack.dataSizeInBytes = samples.size() * sizeof(samples[0]);
	dataPack.pData = &samples[0];
	dataPack.sampleFormat = LpcmFormat::LCPM_FORMAT_F32;
	dataPack.sampleRate = SAMPLE_RATE;
	auto pInternal = CreateWavInternalFromLpcmDataPack(dataPack);

	// �����`�� wav ���t�@�C���o��
	vector<uint8_t> wavFileBuffer;
	CreateWavFileBuffer(*pInternal, wavFileBuffer);
	WriteFileFromBuffer("result.wav", &wavFileBuffer[0], wavFileBuffer.size() * sizeof(wavFileBuffer[0]));

	return 0;
}
