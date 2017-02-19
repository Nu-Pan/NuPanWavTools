
#include "stdafx.hpp"

#include "OptimalHarmonicPhaseEstimator.hpp"

using namespace std;
using namespace nup;

namespace
{
	// ２つの値の最大公約数を求める
	int64_t CalcGcd(int64_t m, int64_t n)
	{
		int64_t temp;
		for (;;)
		{
			temp = n;
			n = m % n;
			if (n == 0)
			{
				break;
			}
			m = temp;
		}

		return temp;
	}

	// ２つの値の最小公倍数を求める
	int64_t CalcLcm(int64_t m, int64_t n)
	{
		return m * n / CalcGcd(m, n);
	}

	// サンプルバッファの適切なサンプル数を計算
	int64_t CalcOptimalSampleCount(const vector<HARM>& harms)
	{
		// #TODO 無駄に大きい数字を返しているかもしれない
		// 素因数分解しまくったほうが良いかも

		int64_t tempSize = 1;
		for (const auto& harm : harms)
		{
			tempSize = CalcLcm(tempSize, harm.order);
		}
		return tempSize;
	}

	// サンプル -> 正規化位置
	float SampleToPosition(int64_t sample, int64_t size)
	{
		return (float)sample / (float)size;
	}

	// 正規化位置 -> ラジアン
	float PositionToRadian(float position)
	{
		 const float SAMPLE_TO_RADIAN = (float)2.0 * (float)M_PI;
		return position * SAMPLE_TO_RADIAN;
	}

	// サンプル -> ラジアン
	float SampleToRadian(int64_t sampleIndex, int64_t sampleSize)
	{
		return PositionToRadian(SampleToPosition(sampleIndex, sampleSize));
	}

	// サンプルオフセットを [0, size) の範囲に正規化
	int64_t _NormalizeSampleOffset(int64_t offset, int64_t size)
	{
		while (offset < 0)
		{
			offset += size;
		}
		while (size <= offset)
		{
			offset -= size;
		}
		return offset;
	}

	// dst = leftBegin + rightBegin
	// サンプル単位で src2 をずらす(+なら遅れる)
	template<typename IteratorType, typename ConstIteratorType>
	void _Add(IteratorType dstBegin, ConstIteratorType leftBegin, ConstIteratorType rightBegin, int64_t size, int64_t rightOffset, float amplitude)
	{
		rightOffset = -rightOffset;
		rightOffset = _NormalizeSampleOffset(rightOffset, size);

		auto dstEnd = dstBegin + size;
		auto rightEnd = rightBegin + size;
		auto dstIter = dstBegin;
		auto leftIter = leftBegin;
		auto rightIter = rightBegin + rightOffset;

		for (;;)
		{
			*dstIter = (*leftIter) + (*rightIter) * amplitude;
			++dstIter;
			++leftIter;
			++rightIter;
			if (dstIter == dstEnd)
			{
				break;
			}
			if (rightIter == rightEnd)
			{
				rightIter = rightBegin;
			}
		}
	}

	// 指定パラメータからサンプルバッファを生成する
	void _CreateSampleBuffer
	(
		vector<float>& outSampleBuffer,
		const vector<HARM>& harms,
		const vector<vector<float> >& sinTables,
		int64_t ignoreIndex = -1
	)
	{
		assert(!harms.empty());
		assert(!sinTables.empty());
		assert(harms.size()<=sinTables.size());

		// サンプルバッファを必要サイズにリサイズ
		outSampleBuffer.resize(sinTables[0].size());

		// ベースサンプルバッファを生成
		size_t outSampleBufferSizeInBytes = outSampleBuffer.size() * sizeof(float);
		memset(&outSampleBuffer[0], 0, outSampleBufferSizeInBytes);
		for (int64_t i = 0; i < static_cast<int64_t>(harms.size()); ++i)
		{
			if (i == ignoreIndex)
			{
				continue;
			}
			assert(sinTables[i].size()==outSampleBuffer.size());
			_Add(outSampleBuffer.begin(), outSampleBuffer.cbegin(), sinTables[i].cbegin(), outSampleBuffer.size(), harms[i].phase.sample, harms[i].amplitude);
		}
	}

	// 指定サンプルが極値か？
	bool _IsExtrema(float leftBegin, float center, float rightBegin)
	{
		return (leftBegin < center) != (center < rightBegin);
	}

	// サンプルバッファ中の全ての極小と極大を列挙
	vector<pair<int64_t, float> > _EnumerateExtremas(const vector<float>& samples)
	{
		vector<pair<int64_t, float> > extremas;

		// １サンプル目は特別扱い
		if (_IsExtrema(samples.back(), samples[0], samples[1]))
		{
			extremas.push_back(make_pair(0, samples[0]));
		}
		// 端っこ以外のサンプルは for で回す
		for (int64_t i = 1; i < static_cast<int64_t>(samples.size()) - 1; ++i)
		{
			if (_IsExtrema(samples[i - 1], samples[i], samples[i + 1]))
			{
				extremas.push_back(make_pair(i, samples[i]));
			}
		}
		// 最後のサンプルも特別扱い
		if (_IsExtrema(*(samples.end() - 2), *(samples.end() - 1), samples[0]))
		{
			extremas.push_back(make_pair(samples.size() - 1, samples.back()));
		}

		return extremas;
	}

	// サンプル列のスコアを計算
	float _Evaluate(const vector<float>& samples)
	{
		// サンプルバッファ中の全ての極小と極大を列挙
		vector<pair<int64_t, float> > extremas = _EnumerateExtremas(samples);

		//// 「隣り合う極小と極大の差の絶対値」の総和をスコアとする
		//// できるだけ激しくスピーカーが動くように選ぶ
		//float score = 0;
		//for (int64_t i = 1; i < minMaxArray.size(); ++i)
		//{
		//	score += std::abs(minMaxArray[i - 1] - minMaxArray[i]);
		//}
		//score += std::abs(minMaxArray.back() - minMaxArray.front());

		// 「最も小さい/大きい極値の落差」の符号を反転した値をスコアとする
		// ノーマライズした時にできるだけRMSが大きいヤツを選ぶ
		float score = 0;
		{
			float minExtrema = std::numeric_limits<float>::max();
			float maxExtrema = std::numeric_limits<float>::lowest();
			for (auto item : extremas)
			{
				if (item.second < minExtrema)
				{
					minExtrema = item.second;
				}
				if (maxExtrema < item.second)
				{
					maxExtrema = item.second;
				}
			}
			score = -std::abs(maxExtrema - minExtrema);
		}

		//// 「極値の絶対値」の総和をスコアとする
		//ScoreType score = 0;
		//for (int64_t i = 0; i < minMaxArray.size(); ++i)
		//{
		//	score += std::abs(minMaxArray[i]);
		//}

		return score;
	}

	// 開始位相が``適切''になるように修正する
	void _CorrectWaveShapeDelay
	(
		vector<HARM>& harms,
		const vector<vector<float> >& sinTables
	)
	{
		// 与えられたパラメータからサンプルバッファを生成
		vector<float> samples;
		_CreateSampleBuffer(samples, harms, sinTables);

		// サンプルバッファ中の全ての極小と極大を列挙
		vector<pair<int64_t, float> > extremas = _EnumerateExtremas(samples);

		// 「一番落差の激しい隣接する極値ペア」の「先頭側極値」を求める
		int64_t headSampleIndex = 0;
		{
			float maxDiff = std::numeric_limits<float>::lowest();
			for (int64_t extremaIndex = 1; extremaIndex < static_cast<int64_t>(extremas.size()); ++extremaIndex)
			{
				const float diffAbs = std::abs(extremas[extremaIndex -1].second - extremas[extremaIndex -0].second);
				if (maxDiff < diffAbs)
				{
					headSampleIndex = extremas[extremaIndex-1].first;
				}
			}
			// 先頭と末尾の極値は特別扱い
			{
				const float diffAbs = std::abs(extremas.front().second - extremas.back().second);
				if (maxDiff < diffAbs)
				{
					headSampleIndex = extremas.back().first;
				}
			}
		}

		// 求めた「先頭側極値」が「サンプルバッファ先頭」に来るようにハーモニックディレイを調整
		for (auto& harm : harms)
		{
			harm.phase.sample = _NormalizeSampleOffset(harm.phase.sample - headSampleIndex, sinTables[0].size());
		}
	}
};

void nup::EstimateOptimalHarmonicPhase(std::vector<HARM>& harms)
{
	// 適切なバッファサイズを計算
	const int64_t sampleBufferSize = CalcOptimalSampleCount(harms);

	// バッファのバイトサイズを事前計算
	const int64_t sampleBufferSizeInBytes = sizeof(float) * sampleBufferSize;

	// 正弦波テーブルを生成
	vector<vector<float> > sinTables;
	{
		sinTables.reserve(harms.size());
		for (const auto& harm : harms)
		{
			sinTables.push_back(vector<float>());
			vector<float>& sinTable = sinTables.back();
			sinTable.resize(sampleBufferSize);
			auto sampleIter = sinTable.begin();
			const int64_t freqRate = harm.order;
			for (int64_t sampleIndex = 0; sampleIndex < sampleBufferSize; ++sampleIndex)
			{
				*sampleIter = std::sin(SampleToRadian(freqRate* sampleIndex, sampleBufferSize));
				++sampleIter;
			}
		}
	}

	// 倍音ディレイ配列を組み立てる
	{
		// ベースとなるサンプルバッファ
		vector<float> baseSamples(sampleBufferSize);

		// 第一倍音は位相 0 で合成
		memset(&baseSamples[0], 0, sampleBufferSizeInBytes);
		_Add(baseSamples.begin(), baseSamples.begin(), sinTables[0].begin(), sampleBufferSize, harms[0].phase.sample, harms[0].amplitude);

		// 一時計算用サンプルバッファ
		vector<float> tempSamples(sampleBufferSize);

		// 最適な位相を順番に見つける
		for (int64_t harmIndex = 1; harmIndex < harms.size(); ++harmIndex)
		{
			// 現在注目する倍音を 1 サンプルづつずらしながら最適なディレイを探索
			harms[harmIndex].phase.sample = 0;
			float bestScore = std::numeric_limits<float>::lowest();
			for (int64_t sampleIndex = 0; sampleIndex < sampleBufferSize; ++sampleIndex)
			{
				_Add(tempSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, sampleIndex, harms[harmIndex].amplitude);
				const float tempScore = _Evaluate(tempSamples);
				if (bestScore < tempScore)
				{
					bestScore = tempScore;
					harms[harmIndex].phase.sample = sampleIndex;
				}
				else
				{
					break;
				}
			}

			// 最適位相でベースサンプルバッファに倍音を合成
			_Add(baseSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, harms[harmIndex].phase.sample, harms[harmIndex].amplitude);
		}
	}

	// 最急降下法で位相を最適化する
	// #XXX このアルゴリズムが必ず停止するのかは不明
	{
		float globalBestScore = std::numeric_limits<float>::lowest();
		for (;;)
		{
			for (int64_t harmIndex = 1; harmIndex < harms.size(); ++harmIndex)
			{
				// ベースサンプルバッファを生成
				vector<float> baseSamples(sampleBufferSize);
				_CreateSampleBuffer(baseSamples, harms, sinTables, harmIndex);

				// 一時計算用サンプルバッファを生成
				vector<float> tempSamples(sampleBufferSize);

				// 初期状態を生成
				int64_t bestIndex = harms[harmIndex].phase.sample;
				_Add(tempSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, bestIndex, harms[harmIndex].amplitude);
				float bestScore = _Evaluate(tempSamples);

				// 右方向を試す
				bool hasUpdated = false;
				for (;;)
				{
					_Add(tempSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, bestIndex + 1, harms[harmIndex].amplitude);
					float tempScore = _Evaluate(tempSamples);
					if (tempScore < bestScore)
					{
						break;
					}
					hasUpdated = true;
					++bestIndex;
					bestScore = tempScore;
				}

				// 左方向を試す
				if (!hasUpdated)
				{
					for (;;)
					{
						_Add(tempSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, bestIndex - 1, harms[harmIndex].amplitude);
						float tempScore = _Evaluate(tempSamples);
						if (tempScore < bestScore)
						{
							break;
						}
						hasUpdated = true;
						--bestIndex;
						bestScore = tempScore;
					}
				}

				// 計算結果のハーモニックディレイを保存
				harms[harmIndex].phase.sample = _NormalizeSampleOffset(bestIndex, sampleBufferSize);

				// #TODO 振幅の微調整もする
			}

			// 評価値を計算
			const float currentScore = [&]()
			{
				vector<float> samples(sampleBufferSize);
				_CreateSampleBuffer(samples, harms, sinTables);
				return _Evaluate(samples);
			}();

			// 評価値の更新がなければ停止
			if (currentScore <= globalBestScore)
			{
				break;
			}

			// スコア更新
			globalBestScore = currentScore;
		}
	}

	// #DEBUG 結果の波形をダンプするヤツ
	auto DumpWaveform = [&](const char* const fileName) {
		// ファイルオープン
		ofstream ofs(fileName);
		// 合成済みサンプルバッファを生成
		vector<float> composedSamples(sampleBufferSize);
		memcpy(&composedSamples[0], &sinTables[0][0], sampleBufferSizeInBytes);
		for (int64_t i = 1; i < static_cast<int64_t>(harms.size()); ++i)
		{
			_Add(composedSamples.begin(), composedSamples.begin(), sinTables[i].begin(), sampleBufferSize, harms[i].phase.sample, harms[i].amplitude);
		}
		// 倍音別バッファを生成
		vector<vector<float> > tempSamples(sampleBufferSize);
		for (int64_t i = 0; i < static_cast<int64_t>(harms.size()); ++i)
		{
			tempSamples[i].resize(sampleBufferSize);
			memset(&tempSamples[i][0], 0, sampleBufferSize);
			_Add(tempSamples[i].begin(), tempSamples[i].begin(), sinTables[i].begin(), sampleBufferSize, harms[i].phase.sample, harms[i].amplitude);
		}
		// 生成したバッファをファイル出力
		for (int64_t sampleIndex = 0; sampleIndex < sampleBufferSize; ++sampleIndex)
		{
			ofs << composedSamples[sampleIndex] << ", ";
			for (int64_t harmIndex = 0; harmIndex < harms.size(); ++harmIndex)
			{
				ofs << tempSamples[harmIndex][sampleIndex] << ", ";
			}
			ofs << endl;
		}
	};

	// #DEBUG
	for (auto& harm : harms)
	{
		harm.phase.sample = 0;
	}

	// #DEBUG 結果の波形をダンプするヤツ
	DumpWaveform("result_waveform_01.csv");

	// 波形の位相を修正
	_CorrectWaveShapeDelay(harms, sinTables);

	// #DEBUG 結果の波形をダンプするヤツ
	DumpWaveform("result_waveform_02.csv");

	// ディレイから位相に変換
	for (auto& harm : harms)
	{
		const int64_t samplePeriod = sampleBufferSize / harm.order;
		const int64_t delay = _NormalizeSampleOffset(harm.phase.sample, samplePeriod);
		const float position = (float)delay / (float)samplePeriod;
		harm.phase.radian = PositionToRadian(position);
	}
}
