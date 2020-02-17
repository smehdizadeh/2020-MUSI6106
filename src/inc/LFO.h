#if !defined(__LFO_hdr__)
#define __LFO_hdr__

#define _USE_MATH_DEFINES
#include <cassert>
#include <algorithm>
#include <cmath>
#include <math.h>

#include "RingBuffer.h"

/*! \wavetable LFO - float only, sinusoidal only
	\details:
	\default constructor moved to private to avoid creating LFO object with no freq
*/
class CLFO
{
public:
	CLFO(float fFreqInHz, float fSampRateInHz) : m_fFreq(fFreqInHz), m_fSampRate(fSampRateInHz)
	{
		assert(!(fFreqInHz < 0) || !(fSampRateInHz < 0));
		m_fFreq = fFreqInHz;
		m_fSampRate = fSampRateInHz;

		//special case: MOD FREQ = 0
		if (m_fFreq == 0)
			m_iTableSize = 1;
		else
			m_iTableSize = static_cast<int>(floor(m_fSampRate/m_fFreq));

		m_pfLFOBuff = new CRingBuffer<float>(m_iTableSize);

		calculateWavetable();
	}

	~CLFO()
	{
		m_pfLFOBuff->reset();
		delete m_pfLFOBuff;
	}

	/*! look up wavetable value at index
	\	return value as a const
	*/
	const float getLFO()
	{
		return(m_pfLFOBuff->getPostInc());
	}

private:
	CLFO();

	/*! Populates values of the LFO wavetable buffer */
	void calculateWavetable()
	{
		for (int i = 0; i < m_iTableSize; i++)
		{
			m_pfLFOBuff->putPostInc(static_cast<float>(sin(2 * M_PI * i * m_fFreq / m_fSampRate)));
		}
	}

	/* Variables */
	float m_fFreq;						//!< LFO frequency in Hz
	float m_fSampRate;					//!< Sample rate in Hz
	int m_iTableSize;					//!< LFO wabetable size for one LFO cycle
	CRingBuffer<float>* m_pfLFOBuff;	//!< LFO wavetable buffer
};

#endif // __LFO_hdr__