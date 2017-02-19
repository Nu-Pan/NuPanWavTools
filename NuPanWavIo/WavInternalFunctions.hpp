
#pragma once

namespace nup
{
	// 前方定義
	class WavInternal;
	struct WAV_FORMAT_CHUNK;

	/** 指定シグネチャを持つチャンクを探索
	@retval	false	見つかった
	@retval	true	見つからなかった
	*/
	int FindChunk(const WavInternal& wavInternal, uint32_t signature, int startIndex = 0);

	/** 指定チャンクのシグネチャを得る
	*/
	uint32_t GetChunkSignature(const WavInternal& wavInternal, int index);

	/** 指定チャンクの全体サイズを得る
	*/
	size_t GetChunkWholeSizeInBytes(const WavInternal& wavInternal, int index);

	/** フォーマットチャンクから内部表現クラスを生成
	*/
	std::shared_ptr<WavInternal> CreateWavInternal(const WAV_FORMAT_CHUNK& formatChunk);

	/** wav ファイルバッファから内部表現クラスを生成
	*/
	std::shared_ptr<WavInternal> CreateWavInternal(const uint8_t* pBuffer, size_t bufferSizeInBytes);

	/** 内部表現クラスからファイルバッファを生成
	*/
	void CreateWavFileBuffer(const WavInternal& wavInternal, std::vector<uint8_t>& dstBuffer);
};
