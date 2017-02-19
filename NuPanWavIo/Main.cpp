
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
///** エントリ関数
//*/
//int main(int argc, char* argv[])
//{
//	try
//	{
//		// コマンドライン引数処理
//		if (argc < 3)
//		{
//			CPPU_THROW("Too few command line arguments.");
//		}
//		const char* const pSrcFilePath = argv[1];
//		const char* const pDstFilePath = argv[2];
//
//		// ファイルをバッファにロード
//		vector<uint8_t> loadBuffer;
//		ReadFileToBuffer(pSrcFilePath, loadBuffer);
//
//		// 内部表現を生成
//		shared_ptr<WavInternal> pWavInternal = CreateWavInternal(&loadBuffer[0], loadBuffer.size());
//
//		// #TODO 内部表現からデータパックを生成
//
//		// #TODO 位相補正処理呼び出し
//		//- ヘッダ
//
//		// #TODO 編集済みデータパックを内部表現に書き戻す
//
//		// データパックから WAV ファイルバッファに戻す
//		vector<uint8_t> saveBuffer;
//		CreateWavFileBuffer(*pWavInternal, saveBuffer);
//
//		// wav ファイルバッファをファイルに書き出す
//		WriteFileFromBuffer(pDstFilePath, &saveBuffer[0], saveBuffer.size());
//	}
//	catch (std::exception& e)
//	{
//		cerr << "! std::runtime_error has thrown." << endl;
//		cerr << e.what();
//		cerr << endl;
//		
//		// #TODO usage 出す
//	}
//	catch (...)
//	{
//		cerr << "! unknown exception has thrown." << endl;
//
//		// #TODO usage 出す
//	}
//
//	return 0;
//}
