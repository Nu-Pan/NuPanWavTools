
#pragma once

#include "Harmonics.hpp"

namespace nup
{
	/** 倍音成分の最適な位相を求める
	*/
	void EstimateOptimalHarmonicPhase(std::vector<HARM>& harms);
};
