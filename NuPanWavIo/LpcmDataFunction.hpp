//! @file LpcmDataFunctions.hpp
//! @author nu-pan

#pragma once

#include "LpcmData.hpp"
#include <memory>

namespace nup
{
	// 前方定義
	class WavInternal;

	/** サンプルフォーマットのビット数を得る
	*/
	inline size_t LpcmFormatSizeInBits(LpcmFormat fmt);

	/** 内部表現 wav データからリニア PCM データを作成
	*/
	template<typename SAMPLE_TYPE>
	LPCM_DATA<SAMPLE_TYPE> CreateLpcmDataPack(const WavInternal& wavInternal, int startIndex = 0);

	/** 内部表現 wav データにリニア PCM データを書き込む
	*/
	template<typename SAMPLE_TYPE>
	void WriteLpcmDataPack(const WavInternal& wavInternal, const LPCM_DATA<SAMPLE_TYPE>& data);

	/** リニア PCM データから内部表現 wav データを出力
	*/
	template<typename SAMPLE_TYPE>
	std::shared_ptr<WavInternal> CreateWavInternalFromLpcmDataPack(const LPCM_DATA<SAMPLE_TYPE>& data);
};

#include "LpcmDataFunctions.inl"