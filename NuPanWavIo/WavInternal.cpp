
#include "stdafx.hpp"

#include "WavInternal.hpp"

#include "WavFileHeader.hpp"
#include "CppUtils.hpp"

using namespace std;
using namespace nup;
using namespace nup;

/** コンストラクタ
*/
WavInternal::WavInternal()
	:m_chunks()
{
}

/** デストラクタ
*/
WavInternal::~WavInternal()
{
}

/** チャンクを追加する
チャンクバッファはクラス内部にコピーが生成される
*/
void WavInternal::AddChunk(const void* pChunk, size_t chunkSizeInBytes)
{
	// 引数チェック
	if (pChunk == nullptr)
	{
		CPPU_THROW("Invalid arguments(null pointer).");
	}
	if (chunkSizeInBytes < sizeof(WAV_CHUNK_HEADER))
	{
		CPPU_THROW("Invalid arguments(Too short buffer).");
	}

	// チャンクバッファをクローンして追加
	auto newBuffer = MakeSharedAsArray<uint8_t>(chunkSizeInBytes);
	memcpy(newBuffer.get(), pChunk, chunkSizeInBytes);
	m_chunks.push_back(newBuffer);
}

/** 指定インデックスのチャンクを削除
*/
void WavInternal::RemoveChunk(int index)
{
	// 引数チェック
	if (static_cast<int>(m_chunks.size()) <= index)
	{
		CPPU_THROW("Invalid index of chunk.");
	}

	// チャンクバッファを削除
	m_chunks.erase(m_chunks.begin() + index);
}
