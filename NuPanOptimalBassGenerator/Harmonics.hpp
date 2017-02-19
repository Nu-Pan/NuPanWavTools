
#pragma once


namespace nup
{
	/** 音名列挙定数
	*/
	enum NoteName
	{
		NOTE_A = 0,
		NOTE_Bb = 1,
		NOTE_B = 2,
		NOTE_C = 3,
		NOTE_Db = 4,
		NOTE_D = 5,
		NOTE_E = 6,
		NOTE_Eb = 7,
		NOTE_F = 8,
		NOTE_Gb = 9,
		NOTE_G = 10,
		NOTE_Ab = 11,
	};

	/** 倍音構造体
	*/
	struct HARM
	{
		//! 倍音の遅れ
		union
		{
			int32_t sample;	//!< サンプル単位
			float radian;	//!< ラジアン単位
		} phase;
		float amplitude;	//!< 倍音の振幅 #TODO デシベル単位にする
		int32_t order;		//!< 基音からの周波数倍率
	};
};