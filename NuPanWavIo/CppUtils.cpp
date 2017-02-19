//! @file CppUtils.cpp
//! @author nu-pan

#include "stdafx.hpp"

#include "CppUtils.hpp"

using namespace std;
using namespace nup;

/** �t�@�C�����o�b�t�@�Ƀo�C�i���f�[�^�Ƃ��ă��[�h
*/
void nup::ReadFileToBuffer(const char* pFilePath, std::vector<uint8_t>& dstBuffer)
{
	// �t�@�C���I�[�v��
	ifstream ifs(pFilePath, ios::in | ios::binary);
	if (!ifs)
	{
		ostringstream oss;
		oss << "Failed to open file '" << pFilePath << "'" << flush;
		CPPU_THROW(oss);
	}

	// �t�@�C���T�C�Y���v�Z
	// @note: �����ɃV�[�N�͖���`���삾�����C������
	ifs.seekg(0, fstream::end);
	const size_t endPos = static_cast<size_t>(ifs.tellg());
	ifs.clear();
	ifs.seekg(0, fstream::beg);
	const size_t begPos = static_cast<size_t>(ifs.tellg());
	const size_t fileSizeInBytes = endPos - begPos;

	// �������ݐ�o�b�t�@���m��
	dstBuffer.clear();
	dstBuffer.resize(fileSizeInBytes);

	// �o�b�t�@�Ƀ��[�h
	ifs.read(reinterpret_cast<char*>(&dstBuffer[0]), dstBuffer.size());
	// #TODO �G���[����

	//�N���[�Y
	ifs.close();
}

/**�@�o�b�t�@���t�@�C���Ƀo�C�i���f�[�^�Ƃ��ď�������
*/
void nup::WriteFileFromBuffer(const char* pFilePath, const uint8_t* pSrcBuffer, size_t srcBufferSizeInBytes)
{
	// �t�@�C���I�[�v��
	ofstream ofs(pFilePath, ios::out | ios::binary);
	if (!ofs)
	{
		ostringstream oss;
		oss << "Failed to open file '" << pFilePath << "'" << flush;
		CPPU_THROW(oss);
	}

	// �t�@�C���ɏ�������
	ofs.write(reinterpret_cast<const char*>(pSrcBuffer), srcBufferSizeInBytes);
	// #TODO �G���[����

	//�N���[�Y
	ofs.close();
}
