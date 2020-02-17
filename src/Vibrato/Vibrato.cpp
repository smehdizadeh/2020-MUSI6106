#include <cassert>
#include <iostream>

#include "Vibrato.h"

CVibrato::CVibrato()
{
	//initialize all member variables
	m_bIsInitialized = false;
	m_fSampleRate = 44100;
	m_iWidth = 0;
	m_fModFreq = 0;
	m_iNumChannels = 0;
	m_iDelayLineLen = 0;

	m_ppfVibBuff = 0;
	m_pCVibLFO = 0;

	m_fTap = 0;
	m_iTap = 0;
	m_fFraction = 0;
	m_fMod = 0;
	m_iModStep = 0;
}

CVibrato::~CVibrato()
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

	m_fTap = 0;
	m_iTap = 0;
	m_fFraction = 0;
	m_fMod = 0;
	m_iModStep = 0;
}

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

	m_fTap = 0;
	m_iTap = 0;
	m_fFraction = 0;
	m_fMod = 0;
	m_iModStep = 0;

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
	m_ppfVibBuff = new CRingBuffer<float>*[m_iNumChannels];
	
	for (int i = 0; i < iNumChannels; i++)
	{
		m_ppfVibBuff[i] = new CRingBuffer<float>(m_iDelayLineLen);

		m_ppfVibBuff[i]->setWriteIdx(0);
		m_ppfVibBuff[i]->setReadIdx(-(m_iWidth + 1)); //base delay
	}
	
	m_pCVibLFO = new CLFO(m_fModFreq, m_fSampleRate);

	m_bIsInitialized = true;
	return kNoError;
}

Error_t CVibrato::process(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumFrames)
{
	//analyze the same index of each channel together for consistency
	for (int i = 0; i < iNumFrames; i++)
	{
		for (int c = 0; c < m_iNumChannels; c++)
		{
			m_fMod = m_pCVibLFO->getLFO();
			m_fTap = 1 + m_iWidth + (m_iWidth * m_fMod);
			m_iTap = static_cast<int>(floor(m_fTap));
			m_fFraction = m_fTap - m_iTap;

			m_ppfVibBuff[c]->putPostInc(ppfInputBuffer[c][i]);

			//calculate output value to write
			m_iModStep++;
			m_ppfVibBuff[c]->setReadIdx(-m_iTap + m_iModStep); //increment along with write idx to maintain base delay
			ppfOutputBuffer[c][i] = m_ppfVibBuff[c]->get(m_fFraction);
		}
	}
	/*
	float fTap = 0; //location on delay line
	int iTap = 0; //index on delay line (floor fTap)
	float fFraction = 0; //remainder of fTap - iTap ("offset" input to ringbuffer.get)
	float fMOD = 0; //output of LFO at current index

	for (int c = 0; c < m_iNumChannels; c++)
	{
		m_ppfVibBuff[c]->setWriteIdx(0);
		m_ppfVibBuff[c]->setReadIdx(-(m_iWidth+1)); //base delay

		for (int i = 0; i < iNumFrames; i++)
		{
			fMOD = m_pCVibLFO->getLFO(i);
			fTap = 1 + m_iWidth + (m_iWidth * fMOD);
			iTap = static_cast<int>(floor(fTap));
			fFraction = fTap - iTap;

			m_ppfVibBuff[c]->putPostInc(ppfInputBuffer[c][i]);

			//calculate output value to write
			m_ppfVibBuff[c]->setReadIdx(-iTap+i+1); //increment along with write idx to maintain base delay
			ppfOutputBuffer[c][i] = m_ppfVibBuff[c]->get(fFraction);
		}
	}
	*/

	return kNoError;
}