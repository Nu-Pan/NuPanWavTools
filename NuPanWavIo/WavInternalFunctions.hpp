
#pragma once

namespace nup
{
	// �O����`
	class WavInternal;
	struct WAV_FORMAT_CHUNK;

	/** �w��V�O�l�`�������`�����N��T��
	@retval	false	��������
	@retval	true	������Ȃ�����
	*/
	int FindChunk(const WavInternal& wavInternal, uint32_t signature, int startIndex = 0);

	/** �w��`�����N�̃V�O�l�`���𓾂�
	*/
	uint32_t GetChunkSignature(const WavInternal& wavInternal, int index);

	/** �w��`�����N�̑S�̃T�C�Y�𓾂�
	*/
	size_t GetChunkWholeSizeInBytes(const WavInternal& wavInternal, int index);

	/** �t�H�[�}�b�g�`�����N��������\���N���X�𐶐�
	*/
	std::shared_ptr<WavInternal> CreateWavInternal(const WAV_FORMAT_CHUNK& formatChunk);

	/** wav �t�@�C���o�b�t�@��������\���N���X�𐶐�
	*/
	std::shared_ptr<WavInternal> CreateWavInternal(const uint8_t* pBuffer, size_t bufferSizeInBytes);

	/** �����\���N���X����t�@�C���o�b�t�@�𐶐�
	*/
	void CreateWavFileBuffer(const WavInternal& wavInternal, std::vector<uint8_t>& dstBuffer);
};
