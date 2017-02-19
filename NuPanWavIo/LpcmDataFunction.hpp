//! @file LpcmDataFunctions.hpp
//! @author nu-pan

#pragma once

#include "LpcmData.hpp"
#include <memory>

namespace nup
{
	// �O����`
	class WavInternal;

	/** �T���v���t�H�[�}�b�g�̃r�b�g���𓾂�
	*/
	inline size_t LpcmFormatSizeInBits(LpcmFormat fmt);

	/** �����\�� wav �f�[�^���烊�j�A PCM �f�[�^���쐬
	*/
	template<typename SAMPLE_TYPE>
	LPCM_DATA<SAMPLE_TYPE> CreateLpcmDataPack(const WavInternal& wavInternal, int startIndex = 0);

	/** �����\�� wav �f�[�^�Ƀ��j�A PCM �f�[�^����������
	*/
	template<typename SAMPLE_TYPE>
	void WriteLpcmDataPack(const WavInternal& wavInternal, const LPCM_DATA<SAMPLE_TYPE>& data);

	/** ���j�A PCM �f�[�^��������\�� wav �f�[�^���o��
	*/
	template<typename SAMPLE_TYPE>
	std::shared_ptr<WavInternal> CreateWavInternalFromLpcmDataPack(const LPCM_DATA<SAMPLE_TYPE>& data);
};

#include "LpcmDataFunctions.inl"