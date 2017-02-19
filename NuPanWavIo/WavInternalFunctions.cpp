
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
	// �A���C�������g�T�C�Y
	static const size_t ALIGNMENT_SIZE_IN_BYTES = 4;

	/** �o�b�t�@�T�C�Y������Ă��邩�`�F�b�N
	*/
	template<typename PtrTypeA, typename PtrTypeB>
	bool _IsShortBufferSize(PtrTypeA pBegin, PtrTypeB pEnd, size_t expectSizeInBytes)
	{
		const size_t actualSizeInBytes = static_cast<size_t>(reinterpret_cast<uintptr_t>(pEnd) - reinterpret_cast<uintptr_t>(pBegin));
		return actualSizeInBytes < expectSizeInBytes;
	}

	/** �A�h���X�ɃI�t�Z�b�g�����Z�i�o�C�g�P�ʁj
	*/
	template<typename AddrType>
	AddrType _Add(AddrType addr, size_t offset)
	{
		return reinterpret_cast<AddrType>(reinterpret_cast<uintptr_t>(addr) + offset);
	}
}

/** �w��V�O�l�`�������`�����N��T��
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

/** �w��`�����N�̃V�O�l�`���𓾂�
*/
uint32_t nup::GetChunkSignature(const WavInternal& wavInternal, int index)
{
	return wavInternal[index]->signature;
}

/** �w��`�����N�̑S�̃T�C�Y�𓾂�
*/
size_t nup::GetChunkWholeSizeInBytes(const WavInternal& wavInternal, int index)
{
	return wavInternal[index]->dataSizeInBytes + sizeof(WAV_CHUNK_HEADER);
}

/** �����\���N���X�𐶐�
*/
std::shared_ptr<WavInternal> CreateWavInternal(const WAV_FORMAT_CHUNK& formatChunk)
{
	// #TODO ��������
	assert(false);
	return shared_ptr<WavInternal>(new WavInternal());
}

/** wav �t�@�C���o�b�t�@��������\���N���X�𐶐�
*/
std::shared_ptr<WavInternal> nup::CreateWavInternal(const uint8_t* pBuffer, size_t bufferSizeInBytes)
{
	// �����`�F�b�N
	if (pBuffer == nullptr)
	{
		CPPU_THROW("Invalid arguments(null pointer).");
	}

	// �ԋp�p�����\���N���X��p��
	std::shared_ptr<WavInternal> pWavInternal(new WavInternal);

	// �o�b�t�@�����A�h���X�����O�Ɍv�Z
	const uint8_t* const  pBufferEnd = pBuffer + bufferSizeInBytes;

	// RIFF �w�b�_���}�b�v
	const WAV_RIFF_HEADER* pRiffHeader = reinterpret_cast<const WAV_RIFF_HEADER*>(pBuffer);

	// RIFF �w�b�_�`�F�b�N
	{
		// �T�C�Y�`�F�b�N
		if (_IsShortBufferSize(pRiffHeader, pBufferEnd, sizeof(WAV_RIFF_HEADER)))
		{
			CPPU_THROW("Too short buffer(in parsing RIFF header).");
		}

		// RIFF �V�O�l�`���`�F�b�N
		if (pRiffHeader->signature != RIFF_HEADER_SIGNATURE)
		{
			CPPU_THROW("Invalid RIFF signature.");
		}

		// �t�H�[�}�b�g�`�F�b�N
		if (pRiffHeader->fileType != RIFF_HEADER_FORMAT)
		{
			CPPU_THROW("Invalid RIFF format.");
		}

		// �t�@�C���T�C�Y�`�F�b�N
		const size_t parsedFileSize = pRiffHeader->fileSizeInBytes + sizeof(pRiffHeader->signature) + sizeof(pRiffHeader->fileSizeInBytes);
		if (bufferSizeInBytes != parsedFileSize)
		{
			CPPU_THROW("Invalid buffer size(not match with file size recorded in RIFF header).");
		}
	}

	// �`�����N������\���N���X�ɋl�߂Ă���
	{
		const uint8_t* pRead = reinterpret_cast<const uint8_t*>(pRiffHeader + 1);
		while (pBufferEnd != pRead)
		{
			//�o�b�t�@�������傤�ǂȂ�I��
			if (pBufferEnd <= pRead)
			{
				break;
			}

			// ���Ȃ��Ƃ��V�O�l�`���ƃ`�����N�T�C�Y���̃T�C�Y���c���Ă��邩
			const size_t leamingSize = static_cast<size_t>(pBufferEnd - pRead);
			if (leamingSize < sizeof(WAV_CHUNK_HEADER))
			{
				CPPU_THROW("Too short buffer(in loading chunks).");
			}

			// �w�b�_�����}�b�v
			const WAV_CHUNK_HEADER* pChunk = reinterpret_cast<const WAV_CHUNK_HEADER*>(pRead);
			const size_t chunkWholeSizeInBytes = pChunk->dataSizeInBytes + sizeof(WAV_CHUNK_HEADER);

			// �`�����N�S�̕��̃T�C�Y���c���Ă��邩
			if (leamingSize < chunkWholeSizeInBytes)
			{
				CPPU_THROW("Too short buffer(in loading chunks).");
			}

			// �����\���N���X�Ƀ`�����N���l�߂�
			// @note: �\���̂Ɏ���o���͖̂ʓ|�Ȃ̂� const �O���Ă��h���}�C���Ă��Ƃ�(�n������ł͓ǂݍ��݂������Ȃ��͂�����)
			pWavInternal->AddChunk(pChunk, chunkWholeSizeInBytes);

			// �|�C���^��i�߂�
			// @note ���ۂ̃I�t�Z�b�g�̓A���C�������g�̉e�����󂯂�
			pRead += AdjustAlignment(chunkWholeSizeInBytes, ALIGNMENT_SIZE_IN_BYTES);
		}
	}

	// �����\���N���X��ԋp
	return pWavInternal;
}

/** �����\���N���X����t�@�C���o�b�t�@�𐶐�
*/
void nup::CreateWavFileBuffer(const WavInternal& wavInternal, std::vector<uint8_t>& fileBuffer)
{
	// �Œ���K�v�ȃ`�����N�����݂��邩�`�F�b�N
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

	// ��Ƀg�[�^���T�C�Y���v�Z
	const size_t fileBufferSizeInBytes = [&]()
	{
		size_t tempSize = sizeof(WAV_RIFF_HEADER);
		for (int i = 0; i < wavInternal.GetSize() - 1; ++i)
		{
			// @note ���ۂ̃I�t�Z�b�g�̓A���C�������g�̉e�����󂯂�
			tempSize += AdjustAlignment(GetChunkWholeSizeInBytes(wavInternal, i), ALIGNMENT_SIZE_IN_BYTES);
		}
		// @note �ŏI�`�����N�����͉e�����󂯂Ȃ�
		tempSize += GetChunkWholeSizeInBytes(wavInternal, wavInternal.GetSize() - 1);
		return tempSize;
	}();

	// �o�b�t�@���m��
	fileBuffer.clear();
	fileBuffer.resize(fileBufferSizeInBytes);
	uint8_t* const pFileBufferBegin = &fileBuffer[0];
	uint8_t* const pFileBufferEnd = pFileBufferBegin + fileBuffer.size();

	// RIFF �w�b�_�쐬
	WAV_RIFF_HEADER* pRiffHeader = reinterpret_cast<WAV_RIFF_HEADER*>(&fileBuffer[0]);
	pRiffHeader->signature = RIFF_HEADER_SIGNATURE;
	pRiffHeader->fileSizeInBytes = fileBufferSizeInBytes - sizeof(WAV_RIFF_HEADER::signature) - sizeof(WAV_RIFF_HEADER::fileSizeInBytes);
	pRiffHeader->fileType = RIFF_HEADER_FORMAT;

	// �`�����N�o�b�t�@�������R�s�[
	{
		uint8_t* pWriteHead = reinterpret_cast<uint8_t*>(pRiffHeader + 1);
		for (int i = 0; i < wavInternal.GetSize(); ++i)
		{
			// �t�@�C���o�b�t�@�E�T�C�Y������Ă邩�`�F�b�N
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
			// �`�����N�o�b�t�@�̃R�s�[�����s
			memcpy(pWriteHead, wavInternal[i], chunkWholeSizeInBytes);

			// �|�C���^�������߂�
			// @note ���ۂ̃I�t�Z�b�g�̓A���C�������g�̉e�����󂯂�
			pWriteHead += AdjustAlignment(chunkWholeSizeInBytes, ALIGNMENT_SIZE_IN_BYTES);
		}
	}
}
