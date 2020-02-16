#include <cassert>
#include <iostream>

#include "Vibrato.h"

CVibrato::CVibrato()
{/*
	//initialize all member variables
	m_bIsInitialized = false;
	m_fSampleRate = 44100;
	m_iWidth = 0;
	m_fModFreq = 0;
	m_iNumChannels = 0;
	m_iDelayLineLen = 0;

	m_ppfVibBuff = 0;
	m_pCVibLFO = 0;*/
}

CVibrato::~CVibrato()
{/*
	//clear allocated memory
	for (int i = 0; i < m_iNumChannels; i++)
	{
		delete[] m_ppfVibBuff[i];
	}

	delete[] m_ppfVibBuff;
	delete m_pCVibLFO;

	//set default values
	m_bIsInitialized = false;
	m_fSampleRate = 44100;
	m_iWidth = 0;
	m_fModFreq = 0;
	m_iNumChannels = 0;
	m_iDelayLineLen = 0;

	m_ppfVibBuff = 0;
	m_pCVibLFO = 0;*/
}
/*
void CVibrato::reset()
{
	//clear allocated memory
	for (int i = 0; i < m_iNumChannels; i++)
	{
		delete[] m_ppfVibBuff[i];
	}

	delete[] m_ppfVibBuff;
	delete m_pCVibLFO;

	//set default values
	m_bIsInitialized = false;
	m_fSampleRate = 44100;
	m_iWidth = 0;
	m_fModFreq = 0;
	m_iNumChannels = 0;
	m_iDelayLineLen = 0;

	m_ppfVibBuff = 0;
	m_pCVibLFO = 0;

	return;
}

Error_t CVibrato::init(float fWidthInSec, float fModFreqInHz, float fSampleRateInHz, int iNumChannels)
{
	//check user inputs
	if (fWidthInSec < 0 || fModFreqInHz < 0 || fSampleRateInHz < 0 || !(iNumChannels > 0))
		return kFunctionInvalidArgsError;
	
	//initialize member variables
	m_iWidth = static_cast<int>(floor(fWidthInSec * fSampleRateInHz));
	m_iDelayLineLen = (3 * m_iWidth) + 2;
	m_fModFreq = fModFreqInHz;
	m_fSampleRate = fSampleRateInHz;
	m_iNumChannels = iNumChannels;

	//initialize ring buffer (x iNumChannels) and create LFO
	for (int i = 0; i < iNumChannels; i++)
	{
		m_ppfVibBuff[i] = new CRingBuffer<float>(m_iDelayLineLen);
	}

	m_pCVibLFO = new CLFO(m_fModFreq, m_fSampleRate);

	m_bIsInitialized = true;
	return kNoError;
}

Error_t CVibrato::process(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumFrames)
{
	float fTap = 0; //location on delay line
	int iTap = 0; //index on delay line (floor fTap)
	float fFraction = 0; //remainder of fTap - iTap ("offset" input to ringbuffer.get)
	float fMOD = 0; //output of LFO at current index

	for (int c = 0; c < m_iNumChannels; c++)
	{
		for (int i = 0; i < iNumFrames; i++)
		{
			fMOD = m_pCVibLFO->getLFO(i);
			fTap = 1 + m_iWidth + (m_iWidth * fMOD);
			iTap = static_cast<int>(floor(fTap));
			fFraction = fTap - iTap;

			//put input at current index at the start of the ring buffer/delay line
			m_ppfVibBuff[c]->setWriteIdx(0);
			m_ppfVibBuff[c]->put(ppfInputBuffer[c][i]);

			//calculate output value to write
			m_ppfVibBuff[c]->setReadIdx(iTap);
			ppfOutputBuffer[c][i] = m_ppfVibBuff[c]->get(fFraction);
		}
	}

	return kNoError;
}*/