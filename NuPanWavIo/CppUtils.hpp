//! @file CppUtils.hpp
//! @author nu-pan

#pragma once

namespace nup
{
	/** 指定されたメッセージで std::runtime_error() を投げる
	*/
#define CPPU_THROW(msg) nup::CppuThrow(msg, __FILE__, __LINE__)

	/** エラー文字列を生成して std::runtime_error() で投げる
	*/
	inline void CppuThrow(const char* const pMsg, const char* const file, int line)
	{
		using namespace std;

		std::ostringstream oss;
		oss << "Error description : " << endl;
		oss << pMsg << endl;
		oss << "File : " << endl;
		oss << file << endl;
		oss << "Line : " << endl;
		oss << line << endl;
		throw std::runtime_error(oss.str());
	}

	/** エラー文字列を生成して std::runtime_error() で投げる
	*/
	inline void CppuThrow(const std::ostringstream& oss, const char* const file, int line)
	{
		CppuThrow(oss.str().c_str(), file, line);
	}

	/** 指定サイズのバッファをアロケートして指定型の shared_ptr として返却
	*/
	template<typename T>
	std::shared_ptr<T> MakeSharedAsArray(size_t size)
	{
		auto dataPackDeleter = [](T* p) {
			if (p) {
				delete[] reinterpret_cast<uint8_t*>(p);
			}
		};
		return shared_ptr<T>(new uint8_t[size], dataPackDeleter);
	}

	/** アラインメントを揃える
	*/	
	template<typename T>
	T AdjustAlignment(T value, T alignment)
	{
		T temp = value + alignment - 1;
		return temp - (temp % alignment);
	}

	/** ファイルをバッファにバイナリデータとしてロード
	*/
	void ReadFileToBuffer(const char* pFilePath, std::vector<uint8_t>& dstBuffer);

	/**　バッファをファイルにバイナリデータとして書き込み
	*/
	void WriteFileFromBuffer(const char* pFilePath, const uint8_t* pSrcBuffer, size_t srcBufferSizeInBytes);
}

