
#pragma once

namespace nup
{
	/** リニア PCM サンプルフォーマット
	とりあえず使うつもりのある float 32 だけサポート
	*/
	enum LpcmFormat
	{
		LCPM_FORMAT_F32,
	};

	/** リニア PCM データ
	*/
	template<typename SAMPLE_TYPE>
	struct LPCM_DATA
	{
		LpcmFormat sampleFormat;	//!< サンプルフォーマット
		uint32_t channels;			//!< チャンネル数
		uint32_t sampleRate;		//!< サンプルレート
		size_t dataSizeInBytes;		//!< サンプル列バッファのサイズ(バイト単位)
		SAMPLE_TYPE* pData;				//!< サンプル列へのポインタ
	};
};
