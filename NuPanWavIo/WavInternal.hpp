
#pragma once

namespace nup
{
	// 前方定義
	struct WAV_CHUNK_HEADER;

	/** wav 内部表現クラス
	*/
	class WavInternal
	{
	public:
		/** コンストラクタ
		*/
		WavInternal();

		/** デストラクタ
		*/
		~WavInternal();

		/** 格納されているチャンク数を得る
		*/
		int GetSize() const
		{
			return static_cast<int>(m_chunks.size());
		}

		/** 指定インデックスのチャンクの先頭アドレスを得る
		*/
		inline const WAV_CHUNK_HEADER* operator[](int index) const
		{
			return reinterpret_cast<const WAV_CHUNK_HEADER*>(m_chunks[index].get());
		}

		/** 指定インデックスのチャンクの先頭アドレスを得る
		*/
		inline WAV_CHUNK_HEADER* operator[](int index)
		{
			return reinterpret_cast<WAV_CHUNK_HEADER*>(m_chunks[index].get());
		}

		/** チャンクを追加する
		チャンクバッファはクラス内部にコピーが生成される
		*/
		void AddChunk(const void* pChunk, size_t chunkSizeInBytes);

		/** 指定インデックスのチャンクを削除
		*/
		void RemoveChunk(int index);

	private:
		/** コピー禁止
		*/
		WavInternal(const WavInternal&) = delete;

		/** コピー禁止
		*/
		WavInternal& operator=(const WavInternal&) = delete;

		std::vector<std::shared_ptr<uint8_t> > m_chunks;	//!< チャンクバッファ
	};
}