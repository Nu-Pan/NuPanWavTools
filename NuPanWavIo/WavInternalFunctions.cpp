
#include "stdafx.hpp"

#include "WavInternalFunctions.hpp"

#include "WavInternal.hpp"
#include "WavFileHeader.hpp"
#include "CppUtils.hpp"

using namespace std;
using namespace nup;
using namespace nup;

namespace
{
	// アラインメントサイズ
	static const size_t ALIGNMENT_SIZE_IN_BYTES = 4;

	/** バッファサイズが足りているかチェック
	*/
	template<typename PtrTypeA, typename PtrTypeB>
	bool _IsShortBufferSize(PtrTypeA pBegin, PtrTypeB pEnd, size_t expectSizeInBytes)
	{
		const size_t actualSizeInBytes = static_cast<size_t>(reinterpret_cast<uintptr_t>(pEnd) - reinterpret_cast<uintptr_t>(pBegin));
		return actualSizeInBytes < expectSizeInBytes;
	}

	/** アドレスにオフセットを加算（バイト単位）
	*/
	template<typename AddrType>
	AddrType _Add(AddrType addr, size_t offset)
	{
		return reinterpret_cast<AddrType>(reinterpret_cast<uintptr_t>(addr) + offset);
	}
}

/** 指定シグネチャを持つチャンクを探索
*/
int nup::FindChunk(const WavInternal& wavInternal, uint32_t signature, int startIndex)
{
	for (size_t i = startIndex; i < wavInternal.GetSize(); ++i)
	{
		if (wavInternal[i]->signature == signature)
		{
			return i;
		}
	}

	return -1;
}

/** 指定チャンクのシグネチャを得る
*/
uint32_t nup::GetChunkSignature(const WavInternal& wavInternal, int index)
{
	return wavInternal[index]->signature;
}

/** 指定チャンクの全体サイズを得る
*/
size_t nup::GetChunkWholeSizeInBytes(const WavInternal& wavInternal, int index)
{
	return wavInternal[index]->dataSizeInBytes + sizeof(WAV_CHUNK_HEADER);
}

/** 内部表現クラスを生成
*/
std::shared_ptr<WavInternal> CreateWavInternal(const WAV_FORMAT_CHUNK& formatChunk)
{
	// #TODO 実装する
	assert(false);
	return shared_ptr<WavInternal>(new WavInternal());
}

/** wav ファイルバッファから内部表現クラスを生成
*/
std::shared_ptr<WavInternal> nup::CreateWavInternal(const uint8_t* pBuffer, size_t bufferSizeInBytes)
{
	// 引数チェック
	if (pBuffer == nullptr)
	{
		CPPU_THROW("Invalid arguments(null pointer).");
	}

	// 返却用内部表現クラスを用意
	std::shared_ptr<WavInternal> pWavInternal(new WavInternal);

	// バッファ末尾アドレスを事前に計算
	const uint8_t* const  pBufferEnd = pBuffer + bufferSizeInBytes;

	// RIFF ヘッダをマップ
	const WAV_RIFF_HEADER* pRiffHeader = reinterpret_cast<const WAV_RIFF_HEADER*>(pBuffer);

	// RIFF ヘッダチェック
	{
		// サイズチェック
		if (_IsShortBufferSize(pRiffHeader, pBufferEnd, sizeof(WAV_RIFF_HEADER)))
		{
			CPPU_THROW("Too short buffer(in parsing RIFF header).");
		}

		// RIFF シグネチャチェック
		if (pRiffHeader->signature != RIFF_HEADER_SIGNATURE)
		{
			CPPU_THROW("Invalid RIFF signature.");
		}

		// フォーマットチェック
		if (pRiffHeader->fileType != RIFF_HEADER_FORMAT)
		{
			CPPU_THROW("Invalid RIFF format.");
		}

		// ファイルサイズチェック
		const size_t parsedFileSize = pRiffHeader->fileSizeInBytes + sizeof(pRiffHeader->signature) + sizeof(pRiffHeader->fileSizeInBytes);
		if (bufferSizeInBytes != parsedFileSize)
		{
			CPPU_THROW("Invalid buffer size(not match with file size recorded in RIFF header).");
		}
	}

	// チャンクを内部表現クラスに詰めていく
	{
		const uint8_t* pRead = reinterpret_cast<const uint8_t*>(pRiffHeader + 1);
		while (pBufferEnd != pRead)
		{
			//バッファ末尾ちょうどなら終了
			if (pBufferEnd <= pRead)
			{
				break;
			}

			// 少なくともシグネチャとチャンクサイズ分のサイズが残っているか
			const size_t leamingSize = static_cast<size_t>(pBufferEnd - pRead);
			if (leamingSize < sizeof(WAV_CHUNK_HEADER))
			{
				CPPU_THROW("Too short buffer(in loading chunks).");
			}

			// ヘッダだけマップ
			const WAV_CHUNK_HEADER* pChunk = reinterpret_cast<const WAV_CHUNK_HEADER*>(pRead);
			const size_t chunkWholeSizeInBytes = pChunk->dataSizeInBytes + sizeof(WAV_CHUNK_HEADER);

			// チャンク全体分のサイズが残っているか
			if (leamingSize < chunkWholeSizeInBytes)
			{
				CPPU_THROW("Too short buffer(in loading chunks).");
			}

			// 内部表現クラスにチャンクを詰める
			// @note: 構造体に手を出すのは面倒なので const 外してもドンマイってことで(渡した先では読み込みしかしないはずだし)
			pWavInternal->AddChunk(pChunk, chunkWholeSizeInBytes);

			// ポインタを進める
			// @note 実際のオフセットはアラインメントの影響を受ける
			pRead += AdjustAlignment(chunkWholeSizeInBytes, ALIGNMENT_SIZE_IN_BYTES);
		}
	}

	// 内部表現クラスを返却
	return pWavInternal;
}

/** 内部表現クラスからファイルバッファを生成
*/
void nup::CreateWavFileBuffer(const WavInternal& wavInternal, std::vector<uint8_t>& fileBuffer)
{
	// 最低限必要なチャンクが存在するかチェック
	{
		if (FindChunk(wavInternal, FORMAT_CHUNK_SIGNATURE)<0)
		{
			CPPU_THROW("wav format chunk was not found.");
		}
		if (FindChunk(wavInternal, DATA_CHUNK_SIGNATURE)<0)
		{
			CPPU_THROW("wav format chunk was not found.");
		}
	}

	// 先にトータルサイズを計算
	const size_t fileBufferSizeInBytes = [&]()
	{
		size_t tempSize = sizeof(WAV_RIFF_HEADER);
		for (int i = 0; i < wavInternal.GetSize() - 1; ++i)
		{
			// @note 実際のオフセットはアラインメントの影響を受ける
			tempSize += AdjustAlignment(GetChunkWholeSizeInBytes(wavInternal, i), ALIGNMENT_SIZE_IN_BYTES);
		}
		// @note 最終チャンクだけは影響を受けない
		tempSize += GetChunkWholeSizeInBytes(wavInternal, wavInternal.GetSize() - 1);
		return tempSize;
	}();

	// バッファを確保
	fileBuffer.clear();
	fileBuffer.resize(fileBufferSizeInBytes);
	uint8_t* const pFileBufferBegin = &fileBuffer[0];
	uint8_t* const pFileBufferEnd = pFileBufferBegin + fileBuffer.size();

	// RIFF ヘッダ作成
	WAV_RIFF_HEADER* pRiffHeader = reinterpret_cast<WAV_RIFF_HEADER*>(&fileBuffer[0]);
	pRiffHeader->signature = RIFF_HEADER_SIGNATURE;
	pRiffHeader->fileSizeInBytes = fileBufferSizeInBytes - sizeof(WAV_RIFF_HEADER::signature) - sizeof(WAV_RIFF_HEADER::fileSizeInBytes);
	pRiffHeader->fileType = RIFF_HEADER_FORMAT;

	// チャンクバッファを順次コピー
	{
		uint8_t* pWriteHead = reinterpret_cast<uint8_t*>(pRiffHeader + 1);
		for (int i = 0; i < wavInternal.GetSize(); ++i)
		{
			// ファイルバッファ・サイズが足りてるかチェック
			const size_t chunkWholeSizeInBytes = GetChunkWholeSizeInBytes(wavInternal, i);
			if (pFileBufferEnd < pWriteHead + chunkWholeSizeInBytes)
			{
				CPPU_THROW("Too short file buffer to write chunk buffer.");
			}

			{
				const void* pTempCHunk = wavInternal[i];
				char tempSignature[] = "xxxx";
				*reinterpret_cast<uint32_t*>(tempSignature) = *reinterpret_cast<const uint32_t*>(pTempCHunk);
			}
			// チャンクバッファのコピーを実行
			memcpy(pWriteHead, wavInternal[i], chunkWholeSizeInBytes);

			// ポインタをすすめる
			// @note 実際のオフセットはアラインメントの影響を受ける
			pWriteHead += AdjustAlignment(chunkWholeSizeInBytes, ALIGNMENT_SIZE_IN_BYTES);
		}
	}
}
