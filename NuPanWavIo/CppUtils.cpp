//! @file CppUtils.cpp
//! @author nu-pan

#include "stdafx.hpp"

#include "CppUtils.hpp"

using namespace std;
using namespace nup;

/** ファイルをバッファにバイナリデータとしてロード
*/
void nup::ReadFileToBuffer(const char* pFilePath, std::vector<uint8_t>& dstBuffer)
{
	// ファイルオープン
	ifstream ifs(pFilePath, ios::in | ios::binary);
	if (!ifs)
	{
		ostringstream oss;
		oss << "Failed to open file '" << pFilePath << "'" << flush;
		CPPU_THROW(oss);
	}

	// ファイルサイズを計算
	// @note: 末尾にシークは未定義動作だった気がする
	ifs.seekg(0, fstream::end);
	const size_t endPos = static_cast<size_t>(ifs.tellg());
	ifs.clear();
	ifs.seekg(0, fstream::beg);
	const size_t begPos = static_cast<size_t>(ifs.tellg());
	const size_t fileSizeInBytes = endPos - begPos;

	// 書き込み先バッファを確保
	dstBuffer.clear();
	dstBuffer.resize(fileSizeInBytes);

	// バッファにロード
	ifs.read(reinterpret_cast<char*>(&dstBuffer[0]), dstBuffer.size());
	// #TODO エラー処理

	//クローズ
	ifs.close();
}

/**　バッファをファイルにバイナリデータとして書き込み
*/
void nup::WriteFileFromBuffer(const char* pFilePath, const uint8_t* pSrcBuffer, size_t srcBufferSizeInBytes)
{
	// ファイルオープン
	ofstream ofs(pFilePath, ios::out | ios::binary);
	if (!ofs)
	{
		ostringstream oss;
		oss << "Failed to open file '" << pFilePath << "'" << flush;
		CPPU_THROW(oss);
	}

	// ファイルに書き込み
	ofs.write(reinterpret_cast<const char*>(pSrcBuffer), srcBufferSizeInBytes);
	// #TODO エラー処理

	//クローズ
	ofs.close();
}
