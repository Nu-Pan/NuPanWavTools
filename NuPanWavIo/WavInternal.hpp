
#pragma once

namespace nup
{
	// �O����`
	struct WAV_CHUNK_HEADER;

	/** wav �����\���N���X
	*/
	class WavInternal
	{
	public:
		/** �R���X�g���N�^
		*/
		WavInternal();

		/** �f�X�g���N�^
		*/
		~WavInternal();

		/** �i�[����Ă���`�����N���𓾂�
		*/
		int GetSize() const
		{
			return static_cast<int>(m_chunks.size());
		}

		/** �w��C���f�b�N�X�̃`�����N�̐擪�A�h���X�𓾂�
		*/
		inline const WAV_CHUNK_HEADER* operator[](int index) const
		{
			return reinterpret_cast<const WAV_CHUNK_HEADER*>(m_chunks[index].get());
		}

		/** �w��C���f�b�N�X�̃`�����N�̐擪�A�h���X�𓾂�
		*/
		inline WAV_CHUNK_HEADER* operator[](int index)
		{
			return reinterpret_cast<WAV_CHUNK_HEADER*>(m_chunks[index].get());
		}

		/** �`�����N��ǉ�����
		�`�����N�o�b�t�@�̓N���X�����ɃR�s�[�����������
		*/
		void AddChunk(const void* pChunk, size_t chunkSizeInBytes);

		/** �w��C���f�b�N�X�̃`�����N���폜
		*/
		void RemoveChunk(int index);

	private:
		/** �R�s�[�֎~
		*/
		WavInternal(const WavInternal&) = delete;

		/** �R�s�[�֎~
		*/
		WavInternal& operator=(const WavInternal&) = delete;

		std::vector<std::shared_ptr<uint8_t> > m_chunks;	//!< �`�����N�o�b�t�@
	};
}