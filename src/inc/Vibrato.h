/*
====================================================================================
Vibrato implementation - design choices:

To make the class user friendly, creating (calling the constructor) and initializing 
an instance of the Vibrato class are two separate processes. This way if a user wants
to create one Vibrato object per audio file, they can keep the objects in the signal
chain as they re-initialize the parameters if needed.

The Vibrato class will also automatically handle multi-channel files by creating
one ring buffer per channel, so the user doesn't have to worry about creating
multiple instances of Vibrato.

*/
#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include <cassert>
#include "ErrorDef.h"
#include "LFO.h"
#include "RingBuffer.h"


/*! \Class for vibrato effect implementation
*/
class CVibrato
{
public:
	CVibrato();
	virtual ~CVibrato();

	/*! set all members to default
	\	destroy LFO object and ringbuffers
	\	return Error_t
	*/
	void reset();

	/*! initialize vibrato instance
	\	param fWidthInSec vibrato modulation width in seconds
	\	param fModFreqInHz vibrato modulation freq in Hz
	\	param fSampleRateInHz audio sample rate in Hz
	\	param iNumChannels number of audio channels
	\	param iBlockSize desired processing block size in samples
	\	return Error_t
	*/
	Error_t init(float fWidthInSec, float fModFreqInHz, float fSampleRateInHz, int iNumChannels);

	/*! process audio and apply vibrato effect
	\	param ppfInputBuffer iNumChannels of incoming audio data (dry)
	\	param ppfOutputBuffer iNumChannels of output audio data (wet)
	\	param iNumFrames number of samples to be processed
	*/
	Error_t process(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumFrames);

private:

	bool m_bIsInitialized;			//!< internal bool to check if init has been called

	float m_fSampleRate;			//!< audio sample rate in Hz
	int m_iWidth;					//!< modulation width in samples
	float m_fModFreq;				//!< modulation frequency in Hz
	int m_iNumChannels;				//!< number of audio channels
	int m_iDelayLineLen;			//!< length of ringbuffer (3*WIDTH + 2)

	CRingBuffer<float>** m_ppfVibBuff;	//!< pointer to Ring Buffers for vibrato fractional delay
	CLFO* m_pCVibLFO;					//!< LFO for vibrato modulation
};

#endif // __Vibrato_hdr__