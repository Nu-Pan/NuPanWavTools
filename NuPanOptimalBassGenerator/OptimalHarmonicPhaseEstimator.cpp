
#include "stdafx.hpp"

#include "OptimalHarmonicPhaseEstimator.hpp"

using namespace std;
using namespace nup;

namespace
{
	// �Q�̒l�̍ő���񐔂����߂�
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

	// �Q�̒l�̍ŏ����{�������߂�
	int64_t CalcLcm(int64_t m, int64_t n)
	{
		return m * n / CalcGcd(m, n);
	}

	// �T���v���o�b�t�@�̓K�؂ȃT���v�������v�Z
	int64_t CalcOptimalSampleCount(const vector<HARM>& harms)
	{
		// #TODO ���ʂɑ傫��������Ԃ��Ă��邩������Ȃ�
		// �f�����������܂������ق����ǂ�����

		int64_t tempSize = 1;
		for (const auto& harm : harms)
		{
			tempSize = CalcLcm(tempSize, harm.order);
		}
		return tempSize;
	}

	// �T���v�� -> ���K���ʒu
	float SampleToPosition(int64_t sample, int64_t size)
	{
		return (float)sample / (float)size;
	}

	// ���K���ʒu -> ���W�A��
	float PositionToRadian(float position)
	{
		 const float SAMPLE_TO_RADIAN = (float)2.0 * (float)M_PI;
		return position * SAMPLE_TO_RADIAN;
	}

	// �T���v�� -> ���W�A��
	float SampleToRadian(int64_t sampleIndex, int64_t sampleSize)
	{
		return PositionToRadian(SampleToPosition(sampleIndex, sampleSize));
	}

	// �T���v���I�t�Z�b�g�� [0, size) �͈̔͂ɐ��K��
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
	// �T���v���P�ʂ� src2 �����炷(+�Ȃ�x���)
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

	// �w��p�����[�^����T���v���o�b�t�@�𐶐�����
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

		// �T���v���o�b�t�@��K�v�T�C�Y�Ƀ��T�C�Y
		outSampleBuffer.resize(sinTables[0].size());

		// �x�[�X�T���v���o�b�t�@�𐶐�
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

	// �w��T���v�����ɒl���H
	bool _IsExtrema(float leftBegin, float center, float rightBegin)
	{
		return (leftBegin < center) != (center < rightBegin);
	}

	// �T���v���o�b�t�@���̑S�Ă̋ɏ��Ƌɑ���
	vector<pair<int64_t, float> > _EnumerateExtremas(const vector<float>& samples)
	{
		vector<pair<int64_t, float> > extremas;

		// �P�T���v���ڂ͓��ʈ���
		if (_IsExtrema(samples.back(), samples[0], samples[1]))
		{
			extremas.push_back(make_pair(0, samples[0]));
		}
		// �[�����ȊO�̃T���v���� for �ŉ�
		for (int64_t i = 1; i < static_cast<int64_t>(samples.size()) - 1; ++i)
		{
			if (_IsExtrema(samples[i - 1], samples[i], samples[i + 1]))
			{
				extremas.push_back(make_pair(i, samples[i]));
			}
		}
		// �Ō�̃T���v�������ʈ���
		if (_IsExtrema(*(samples.end() - 2), *(samples.end() - 1), samples[0]))
		{
			extremas.push_back(make_pair(samples.size() - 1, samples.back()));
		}

		return extremas;
	}

	// �T���v����̃X�R�A���v�Z
	float _Evaluate(const vector<float>& samples)
	{
		// �T���v���o�b�t�@���̑S�Ă̋ɏ��Ƌɑ���
		vector<pair<int64_t, float> > extremas = _EnumerateExtremas(samples);

		//// �u�ׂ荇���ɏ��Ƌɑ�̍��̐�Βl�v�̑��a���X�R�A�Ƃ���
		//// �ł��邾���������X�s�[�J�[�������悤�ɑI��
		//float score = 0;
		//for (int64_t i = 1; i < minMaxArray.size(); ++i)
		//{
		//	score += std::abs(minMaxArray[i - 1] - minMaxArray[i]);
		//}
		//score += std::abs(minMaxArray.back() - minMaxArray.front());

		// �u�ł�������/�傫���ɒl�̗����v�̕����𔽓]�����l���X�R�A�Ƃ���
		// �m�[�}���C�Y�������ɂł��邾��RMS���傫�����c��I��
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

		//// �u�ɒl�̐�Βl�v�̑��a���X�R�A�Ƃ���
		//ScoreType score = 0;
		//for (int64_t i = 0; i < minMaxArray.size(); ++i)
		//{
		//	score += std::abs(minMaxArray[i]);
		//}

		return score;
	}

	// �J�n�ʑ���``�K��''�ɂȂ�悤�ɏC������
	void _CorrectWaveShapeDelay
	(
		vector<HARM>& harms,
		const vector<vector<float> >& sinTables
	)
	{
		// �^����ꂽ�p�����[�^����T���v���o�b�t�@�𐶐�
		vector<float> samples;
		_CreateSampleBuffer(samples, harms, sinTables);

		// �T���v���o�b�t�@���̑S�Ă̋ɏ��Ƌɑ���
		vector<pair<int64_t, float> > extremas = _EnumerateExtremas(samples);

		// �u��ԗ����̌������אڂ���ɒl�y�A�v�́u�擪���ɒl�v�����߂�
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
			// �擪�Ɩ����̋ɒl�͓��ʈ���
			{
				const float diffAbs = std::abs(extremas.front().second - extremas.back().second);
				if (maxDiff < diffAbs)
				{
					headSampleIndex = extremas.back().first;
				}
			}
		}

		// ���߂��u�擪���ɒl�v���u�T���v���o�b�t�@�擪�v�ɗ���悤�Ƀn�[���j�b�N�f�B���C�𒲐�
		for (auto& harm : harms)
		{
			harm.phase.sample = _NormalizeSampleOffset(harm.phase.sample - headSampleIndex, sinTables[0].size());
		}
	}
};

void nup::EstimateOptimalHarmonicPhase(std::vector<HARM>& harms)
{
	// �K�؂ȃo�b�t�@�T�C�Y���v�Z
	const int64_t sampleBufferSize = CalcOptimalSampleCount(harms);

	// �o�b�t�@�̃o�C�g�T�C�Y�����O�v�Z
	const int64_t sampleBufferSizeInBytes = sizeof(float) * sampleBufferSize;

	// �����g�e�[�u���𐶐�
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

	// �{���f�B���C�z���g�ݗ��Ă�
	{
		// �x�[�X�ƂȂ�T���v���o�b�t�@
		vector<float> baseSamples(sampleBufferSize);

		// ���{���͈ʑ� 0 �ō���
		memset(&baseSamples[0], 0, sampleBufferSizeInBytes);
		_Add(baseSamples.begin(), baseSamples.begin(), sinTables[0].begin(), sampleBufferSize, harms[0].phase.sample, harms[0].amplitude);

		// �ꎞ�v�Z�p�T���v���o�b�t�@
		vector<float> tempSamples(sampleBufferSize);

		// �œK�Ȉʑ������ԂɌ�����
		for (int64_t harmIndex = 1; harmIndex < harms.size(); ++harmIndex)
		{
			// ���ݒ��ڂ���{���� 1 �T���v���Â��炵�Ȃ���œK�ȃf�B���C��T��
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

			// �œK�ʑ��Ńx�[�X�T���v���o�b�t�@�ɔ{��������
			_Add(baseSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, harms[harmIndex].phase.sample, harms[harmIndex].amplitude);
		}
	}

	// �ŋ}�~���@�ňʑ����œK������
	// #XXX ���̃A���S���Y�����K����~����̂��͕s��
	{
		float globalBestScore = std::numeric_limits<float>::lowest();
		for (;;)
		{
			for (int64_t harmIndex = 1; harmIndex < harms.size(); ++harmIndex)
			{
				// �x�[�X�T���v���o�b�t�@�𐶐�
				vector<float> baseSamples(sampleBufferSize);
				_CreateSampleBuffer(baseSamples, harms, sinTables, harmIndex);

				// �ꎞ�v�Z�p�T���v���o�b�t�@�𐶐�
				vector<float> tempSamples(sampleBufferSize);

				// ������Ԃ𐶐�
				int64_t bestIndex = harms[harmIndex].phase.sample;
				_Add(tempSamples.begin(), baseSamples.begin(), sinTables[harmIndex].begin(), sampleBufferSize, bestIndex, harms[harmIndex].amplitude);
				float bestScore = _Evaluate(tempSamples);

				// �E����������
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

				// ������������
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

				// �v�Z���ʂ̃n�[���j�b�N�f�B���C��ۑ�
				harms[harmIndex].phase.sample = _NormalizeSampleOffset(bestIndex, sampleBufferSize);

				// #TODO �U���̔�����������
			}

			// �]���l���v�Z
			const float currentScore = [&]()
			{
				vector<float> samples(sampleBufferSize);
				_CreateSampleBuffer(samples, harms, sinTables);
				return _Evaluate(samples);
			}();

			// �]���l�̍X�V���Ȃ���Β�~
			if (currentScore <= globalBestScore)
			{
				break;
			}

			// �X�R�A�X�V
			globalBestScore = currentScore;
		}
	}

	// #DEBUG ���ʂ̔g�`���_���v���郄�c
	auto DumpWaveform = [&](const char* const fileName) {
		// �t�@�C���I�[�v��
		ofstream ofs(fileName);
		// �����ς݃T���v���o�b�t�@�𐶐�
		vector<float> composedSamples(sampleBufferSize);
		memcpy(&composedSamples[0], &sinTables[0][0], sampleBufferSizeInBytes);
		for (int64_t i = 1; i < static_cast<int64_t>(harms.size()); ++i)
		{
			_Add(composedSamples.begin(), composedSamples.begin(), sinTables[i].begin(), sampleBufferSize, harms[i].phase.sample, harms[i].amplitude);
		}
		// �{���ʃo�b�t�@�𐶐�
		vector<vector<float> > tempSamples(sampleBufferSize);
		for (int64_t i = 0; i < static_cast<int64_t>(harms.size()); ++i)
		{
			tempSamples[i].resize(sampleBufferSize);
			memset(&tempSamples[i][0], 0, sampleBufferSize);
			_Add(tempSamples[i].begin(), tempSamples[i].begin(), sinTables[i].begin(), sampleBufferSize, harms[i].phase.sample, harms[i].amplitude);
		}
		// ���������o�b�t�@���t�@�C���o��
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

	// #DEBUG ���ʂ̔g�`���_���v���郄�c
	DumpWaveform("result_waveform_01.csv");

	// �g�`�̈ʑ����C��
	_CorrectWaveShapeDelay(harms, sinTables);

	// #DEBUG ���ʂ̔g�`���_���v���郄�c
	DumpWaveform("result_waveform_02.csv");

	// �f�B���C����ʑ��ɕϊ�
	for (auto& harm : harms)
	{
		const int64_t samplePeriod = sampleBufferSize / harm.order;
		const int64_t delay = _NormalizeSampleOffset(harm.phase.sample, samplePeriod);
		const float position = (float)delay / (float)samplePeriod;
		harm.phase.radian = PositionToRadian(position);
	}
}
