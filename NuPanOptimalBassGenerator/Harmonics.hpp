
#pragma once


namespace nup
{
	/** �����񋓒萔
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

	/** �{���\����
	*/
	struct HARM
	{
		//! �{���̒x��
		union
		{
			int32_t sample;	//!< �T���v���P��
			float radian;	//!< ���W�A���P��
		} phase;
		float amplitude;	//!< �{���̐U�� #TODO �f�V�x���P�ʂɂ���
		int32_t order;		//!< �����̎��g���{��
	};
};