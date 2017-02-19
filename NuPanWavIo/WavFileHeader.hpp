//! @file WavFileHeader.hpp
//! @author nu-pan

#pragma once

/*
参考にしたページ : http://www.web-sky.org/program/other/wave.php
*/

namespace nup
{
	// 4文字のシグネチャを uin32_t として定義するマクロ
#define DEFINE_WAV_SIGNATURE(name, str) \
	static const char (name ## _STR)[] = (str); \
	static const uint32_t (name) = *reinterpret_cast<const uint32_t*>(name ## _STR)

	// ヘッダ/チャンクのシグネチャ
	DEFINE_WAV_SIGNATURE(RIFF_HEADER_SIGNATURE, "RIFF");
	DEFINE_WAV_SIGNATURE(RIFF_HEADER_FORMAT, "WAVE");
	DEFINE_WAV_SIGNATURE(FORMAT_CHUNK_SIGNATURE, "fmt ");
	DEFINE_WAV_SIGNATURE(DATA_CHUNK_SIGNATURE, "data");

	/** RIFF ヘッダー
	*/
	struct WAV_RIFF_HEADER
	{
		uint32_t signature;			//!< 必ず "RIFF"
		uint32_t  fileSizeInBytes;	//!< riff と size を除くファイルサイズ 
		uint32_t fileType;			//!< 音声データなら "WAVE"
	};

	/** フォーマットチャンク
	*/
	struct WAV_FORMAT_CHUNK 
	{
		uint32_t signature;				//!< 必ず "fmt "
		uint32_t chunkSizeInBytes;		//!< signature と chunkSizeInBytes を除くフォーマットチャンクのサイズ
		uint16_t formatId;				//!< 波形データのフォーマット(とりあえず 0x0001 以外考えない)
		uint16_t channels;				//!< チャンネル数
		uint32_t sampleRate;			//!< サンプリングレート(Hz)
		uint32_t bytePerSec;			//!< ビットレート * 8
		uint16_t blockSizeInBytes;		//!< データブロックのサイズ(サンプルサイズ*チャンネル数)
		uint16_t sampleSizeInBits;		//!< サンプル１個のサイズ

		// 対応予定も無いので消しておく
//		uint16_t extendedSizeInBytes;	//!< extended のサイズ(存在しないかもしれない)
//		uint8_t extended[];				//!< 拡張データ領域(存在しないかもしれない)
	};

	/** チャンクの頭に必ず付くヘッダ情報
	*/
	struct WAV_CHUNK_HEADER 
	{
		uint32_t signature;			//!< シグネチャ
		uint32_t dataSizeInBytes;	//!< 後続するデータのサイズ（バイト単位）
	};

	/** Wav データフォーマット ID の列挙値
	*/
	enum WavFormatId
	{
		WAVE_FORMAT_UNKNOWN		= 0x0000,	//!< Microsoft Corporation
		WAVE_FORMAT_PCM			= 0x0001,	//!< Microsoft PCM format
		WAVE_FORMAT_IEEE_FLOAT	= 0x0003,	//!< Micrososft 32 bit float format
		WAVE_FORMAT_EXTENSIBLE	= 0xFFFE,	//!< 上限値
	};

};
