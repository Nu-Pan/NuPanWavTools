//! @file LpcmDataFunctions.inl
//! @author nu-pan

#include <vector>

#include "WavFileHeader.hpp"
#include "WavInternal.hpp"
#include "WavInternalFunctions.hpp"
#include "CppUtils.hpp"

namespace nup
{
	namespace detail
	{
		// サポートするサンプルフォーマット
		static const LpcmFormat SUPPORT_SAMPLE_FORMAT = LCPM_FORMAT_F32;
		static const size_t SUPPORT_SAMPLE_SIZE_IN_BITS = LpcmFormatSizeInBits(SUPPORT_SAMPLE_FORMAT);
		static const size_t SUPPORT_SAMPLE_SIZE_IN_BYTES = SUPPORT_SAMPLE_SIZE_IN_BITS / 8;
	}

	/** サンプルフォーマットのビット数を得る
	*/
	inline size_t LpcmFormatSizeInBits(LpcmFormat fmt)
	{
		switch (fmt)
		{
		case LpcmFormat::LCPM_FORMAT_F32:
			return 32;
		default:
			CPPU_THROW("Unknwon lpcm sample format");
			return 0;
		}
	}

	/** 内部表現 wav データからリニア PCM データパックを作成
	*/
	template<typename SAMPLE_TYPE>
	LPCM_DATA<SAMPLE_TYPE> CreateLpcmDataPack(const WavInternal& wavInternal, int startIndex)
	{
		// 各チャンクのポインタを取得
		const WAV_CHUNK_HEADER* pDataChunk = wavInternal[FindChunk(wavInternal, DATA_CHUNK_SIGNATURE)];
		const WAV_FORMAT_CHUNK* pFormatChunk = reinterpret_cast<const WAV_FORMAT_CHUNK*>(wavInternal[FindChunk(wavInternal, FORMAT_CHUNK_SIGNATURE)]);

		// データパックを生成して返却
		LPCM_DATA<SAMPLE_TYPE> pack;

		// #TODO 実装する
		assert(false);

		return pack;
	}

	/** 内部表現 wav データにリニア PCM データパックを書き込む
	*/
	template<typename SAMPLE_TYPE>
	void WriteLpcmDataPack(const WavInternal& wavInternal, const LPCM_DATA<SAMPLE_TYPE>& data)
	{
		// #TODO 実装する
		assert(false);
	}

	/** リニア PCM データから内部表現 wav データを出力
	*/
	template<typename SAMPLE_TYPE>
	std::shared_ptr<WavInternal> CreateWavInternalFromLpcmDataPack(const LPCM_DATA<SAMPLE_TYPE>& data)
	{
		// 返却用内部表現 wav データ
		auto pWavInternal = std::shared_ptr<WavInternal>(new WavInternal());

		// フォーマットチャンクを生成
		{
			WAV_FORMAT_CHUNK chunk;
			chunk.signature = FORMAT_CHUNK_SIGNATURE;
			chunk.chunkSizeInBytes = sizeof(WAV_FORMAT_CHUNK) - sizeof(WAV_FORMAT_CHUNK::signature) - sizeof(WAV_FORMAT_CHUNK::chunkSizeInBytes);
			chunk.formatId = WAVE_FORMAT_IEEE_FLOAT; // #TODO テンプレート引数で合わせる
			chunk.channels = data.channels;
			chunk.sampleRate = data.sampleRate;
			chunk.bytePerSec = data.sampleRate * data.channels * sizeof(SAMPLE_TYPE);
			chunk.blockSizeInBytes = data.channels * sizeof(SAMPLE_TYPE);
			chunk.sampleSizeInBits = sizeof(SAMPLE_TYPE) * 8;
			pWavInternal->AddChunk(&chunk, sizeof(WAV_FORMAT_CHUNK));
		}

		// データチャンクを生成
		{
			std::vector<uint8_t> chunkBuffer;
			chunkBuffer.resize(sizeof(WAV_CHUNK_HEADER) + data.dataSizeInBytes);
			WAV_CHUNK_HEADER* pChunk = reinterpret_cast<WAV_CHUNK_HEADER*>(&chunkBuffer[0]);
			pChunk->signature = DATA_CHUNK_SIGNATURE;
			pChunk->dataSizeInBytes = data.dataSizeInBytes;
			memcpy(pChunk+1, data.pData, data.dataSizeInBytes);
			pWavInternal->AddChunk(pChunk, chunkBuffer.size());
		}

		return pWavInternal;
	}
};
