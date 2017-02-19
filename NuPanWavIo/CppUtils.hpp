//! @file CppUtils.hpp
//! @author nu-pan

#pragma once

namespace nup
{
	/** �w�肳�ꂽ���b�Z�[�W�� std::runtime_error() �𓊂���
	*/
#define CPPU_THROW(msg) nup::CppuThrow(msg, __FILE__, __LINE__)

	/** �G���[������𐶐����� std::runtime_error() �œ�����
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

	/** �G���[������𐶐����� std::runtime_error() �œ�����
	*/
	inline void CppuThrow(const std::ostringstream& oss, const char* const file, int line)
	{
		CppuThrow(oss.str().c_str(), file, line);
	}

	/** �w��T�C�Y�̃o�b�t�@���A���P�[�g���Ďw��^�� shared_ptr �Ƃ��ĕԋp
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

	/** �A���C�������g�𑵂���
	*/	
	template<typename T>
	T AdjustAlignment(T value, T alignment)
	{
		T temp = value + alignment - 1;
		return temp - (temp % alignment);
	}

	/** �t�@�C�����o�b�t�@�Ƀo�C�i���f�[�^�Ƃ��ă��[�h
	*/
	void ReadFileToBuffer(const char* pFilePath, std::vector<uint8_t>& dstBuffer);

	/**�@�o�b�t�@���t�@�C���Ƀo�C�i���f�[�^�Ƃ��ď�������
	*/
	void WriteFileFromBuffer(const char* pFilePath, const uint8_t* pSrcBuffer, size_t srcBufferSizeInBytes);
}

