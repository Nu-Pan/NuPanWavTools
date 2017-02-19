
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

	// 倍音設定を記述
	vector<HARM> harms;
	harms.resize(MAX_HARMONICS_SIZE);
	for (int harmIndex = 0; harmIndex < MAX_HARMONICS_SIZE; ++harmIndex)
	{
		HARM& harm = harms[harmIndex];
		harm.order = harmIndex + 1;
		harm.phase.sample = 0;
		harm.amplitude = 1.f / (float)harm.order;
	}

	// 各倍音の最適な位相を求める
	EstimateOptimalHarmonicPhase(harms);

	// #DEBUG 計算した倍音位相を出力
	{
		ofstream ofs("result_phase.csv");
		for (auto harm : harms)
		{
			ofs << harm.phase.radian << endl;
		}
	}

	// #TODO 複数の音程で生成
	// 最適化した倍音情報を元にサンプルバッファを生成
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

	// サンプルバッファを内部形式 WAV に変換
	LPCM_DATA<float> dataPack;
	dataPack.channels = 1;
	dataPack.dataSizeInBytes = samples.size() * sizeof(samples[0]);
	dataPack.pData = &samples[0];
	dataPack.sampleFormat = LpcmFormat::LCPM_FORMAT_F32;
	dataPack.sampleRate = SAMPLE_RATE;
	auto pInternal = CreateWavInternalFromLpcmDataPack(dataPack);

	// 内部形式 wav をファイル出力
	vector<uint8_t> wavFileBuffer;
	CreateWavFileBuffer(*pInternal, wavFileBuffer);
	WriteFileFromBuffer("result.wav", &wavFileBuffer[0], wavFileBuffer.size() * sizeof(wavFileBuffer[0]));

	return 0;
}
