
#include "stdafx.hpp"

#include "WavInternal.hpp"

#include "WavFileHeader.hpp"
#include "CppUtils.hpp"

using namespace std;
using namespace nup;
using namespace nup;

/** �R���X�g���N�^
*/
WavInternal::WavInternal()
	:m_chunks()
{
}

/** �f�X�g���N�^
*/
WavInternal::~WavInternal()
{
}

/** �`�����N��ǉ�����
�`�����N�o�b�t�@�̓N���X�����ɃR�s�[�����������
*/
void WavInternal::AddChunk(const void* pChunk, size_t chunkSizeInBytes)
{
	// �����`�F�b�N
	if (pChunk == nullptr)
	{
		CPPU_THROW("Invalid arguments(null pointer).");
	}
	if (chunkSizeInBytes < sizeof(WAV_CHUNK_HEADER))
	{
		CPPU_THROW("Invalid arguments(Too short buffer).");
	}

	// �`�����N�o�b�t�@���N���[�����Ēǉ�
	auto newBuffer = MakeSharedAsArray<uint8_t>(chunkSizeInBytes);
	memcpy(newBuffer.get(), pChunk, chunkSizeInBytes);
	m_chunks.push_back(newBuffer);
}

/** �w��C���f�b�N�X�̃`�����N���폜
*/
void WavInternal::RemoveChunk(int index)
{
	// �����`�F�b�N
	if (static_cast<int>(m_chunks.size()) <= index)
	{
		CPPU_THROW("Invalid index of chunk.");
	}

	// �`�����N�o�b�t�@���폜
	m_chunks.erase(m_chunks.begin() + index);
}
