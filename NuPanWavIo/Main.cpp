
#include "stdafx.hpp"
//
//#include "WavInternal.hpp"
//#include "WavInternalFunctions.hpp"
//#include "CppUtils.hpp"
//
//using namespace std;
//using namespace nup;
//using namespace nup;
//
//namespace
//{
//};
//
///** �G���g���֐�
//*/
//int main(int argc, char* argv[])
//{
//	try
//	{
//		// �R�}���h���C����������
//		if (argc < 3)
//		{
//			CPPU_THROW("Too few command line arguments.");
//		}
//		const char* const pSrcFilePath = argv[1];
//		const char* const pDstFilePath = argv[2];
//
//		// �t�@�C�����o�b�t�@�Ƀ��[�h
//		vector<uint8_t> loadBuffer;
//		ReadFileToBuffer(pSrcFilePath, loadBuffer);
//
//		// �����\���𐶐�
//		shared_ptr<WavInternal> pWavInternal = CreateWavInternal(&loadBuffer[0], loadBuffer.size());
//
//		// #TODO �����\������f�[�^�p�b�N�𐶐�
//
//		// #TODO �ʑ��␳�����Ăяo��
//		//- �w�b�_
//
//		// #TODO �ҏW�ς݃f�[�^�p�b�N������\���ɏ����߂�
//
//		// �f�[�^�p�b�N���� WAV �t�@�C���o�b�t�@�ɖ߂�
//		vector<uint8_t> saveBuffer;
//		CreateWavFileBuffer(*pWavInternal, saveBuffer);
//
//		// wav �t�@�C���o�b�t�@���t�@�C���ɏ����o��
//		WriteFileFromBuffer(pDstFilePath, &saveBuffer[0], saveBuffer.size());
//	}
//	catch (std::exception& e)
//	{
//		cerr << "! std::runtime_error has thrown." << endl;
//		cerr << e.what();
//		cerr << endl;
//		
//		// #TODO usage �o��
//	}
//	catch (...)
//	{
//		cerr << "! unknown exception has thrown." << endl;
//
//		// #TODO usage �o��
//	}
//
//	return 0;
//}
