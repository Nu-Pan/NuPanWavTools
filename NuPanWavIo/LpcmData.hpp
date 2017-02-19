
#pragma once

namespace nup
{
	/** ���j�A PCM �T���v���t�H�[�}�b�g
	�Ƃ肠�����g������̂��� float 32 �����T�|�[�g
	*/
	enum LpcmFormat
	{
		LCPM_FORMAT_F32,
	};

	/** ���j�A PCM �f�[�^
	*/
	template<typename SAMPLE_TYPE>
	struct LPCM_DATA
	{
		LpcmFormat sampleFormat;	//!< �T���v���t�H�[�}�b�g
		uint32_t channels;			//!< �`�����l����
		uint32_t sampleRate;		//!< �T���v�����[�g
		size_t dataSizeInBytes;		//!< �T���v����o�b�t�@�̃T�C�Y(�o�C�g�P��)
		SAMPLE_TYPE* pData;				//!< �T���v����ւ̃|�C���^
	};
};
