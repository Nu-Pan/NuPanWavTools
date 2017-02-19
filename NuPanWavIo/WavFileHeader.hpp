//! @file WavFileHeader.hpp
//! @author nu-pan

#pragma once

/*
�Q�l�ɂ����y�[�W : http://www.web-sky.org/program/other/wave.php
*/

namespace nup
{
	// 4�����̃V�O�l�`���� uin32_t �Ƃ��Ē�`����}�N��
#define DEFINE_WAV_SIGNATURE(name, str) \
	static const char (name ## _STR)[] = (str); \
	static const uint32_t (name) = *reinterpret_cast<const uint32_t*>(name ## _STR)

	// �w�b�_/�`�����N�̃V�O�l�`��
	DEFINE_WAV_SIGNATURE(RIFF_HEADER_SIGNATURE, "RIFF");
	DEFINE_WAV_SIGNATURE(RIFF_HEADER_FORMAT, "WAVE");
	DEFINE_WAV_SIGNATURE(FORMAT_CHUNK_SIGNATURE, "fmt ");
	DEFINE_WAV_SIGNATURE(DATA_CHUNK_SIGNATURE, "data");

	/** RIFF �w�b�_�[
	*/
	struct WAV_RIFF_HEADER
	{
		uint32_t signature;			//!< �K�� "RIFF"
		uint32_t  fileSizeInBytes;	//!< riff �� size �������t�@�C���T�C�Y 
		uint32_t fileType;			//!< �����f�[�^�Ȃ� "WAVE"
	};

	/** �t�H�[�}�b�g�`�����N
	*/
	struct WAV_FORMAT_CHUNK 
	{
		uint32_t signature;				//!< �K�� "fmt "
		uint32_t chunkSizeInBytes;		//!< signature �� chunkSizeInBytes �������t�H�[�}�b�g�`�����N�̃T�C�Y
		uint16_t formatId;				//!< �g�`�f�[�^�̃t�H�[�}�b�g(�Ƃ肠���� 0x0001 �ȊO�l���Ȃ�)
		uint16_t channels;				//!< �`�����l����
		uint32_t sampleRate;			//!< �T���v�����O���[�g(Hz)
		uint32_t bytePerSec;			//!< �r�b�g���[�g * 8
		uint16_t blockSizeInBytes;		//!< �f�[�^�u���b�N�̃T�C�Y(�T���v���T�C�Y*�`�����l����)
		uint16_t sampleSizeInBits;		//!< �T���v���P�̃T�C�Y

		// �Ή��\��������̂ŏ����Ă���
//		uint16_t extendedSizeInBytes;	//!< extended �̃T�C�Y(���݂��Ȃ���������Ȃ�)
//		uint8_t extended[];				//!< �g���f�[�^�̈�(���݂��Ȃ���������Ȃ�)
	};

	/** �`�����N�̓��ɕK���t���w�b�_���
	*/
	struct WAV_CHUNK_HEADER 
	{
		uint32_t signature;			//!< �V�O�l�`��
		uint32_t dataSizeInBytes;	//!< �㑱����f�[�^�̃T�C�Y�i�o�C�g�P�ʁj
	};

	/** Wav �f�[�^�t�H�[�}�b�g ID �̗񋓒l
	*/
	enum WavFormatId
	{
		WAVE_FORMAT_UNKNOWN		= 0x0000,	//!< Microsoft Corporation
		WAVE_FORMAT_PCM			= 0x0001,	//!< Microsoft PCM format
		WAVE_FORMAT_IEEE_FLOAT	= 0x0003,	//!< Micrososft 32 bit float format
		WAVE_FORMAT_EXTENSIBLE	= 0xFFFE,	//!< ����l
	};

};
