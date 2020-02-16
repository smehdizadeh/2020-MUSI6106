#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Synthesis.h"

#include "AudioFileIf.h"
#include "RingBuffer.h"
#include "Vibrato.h"
#include "LFO.h"

extern std::string cTestDataDir;

//===============================================================================================================
// Ring Buffer tests

SUITE(RingBuff)
{
	struct RingBuffData
	{
		RingBuffData()
		{
			//constructor
			m_piCRingBuffer = new CRingBuffer<int>(m_iBufferLengthInSamples); //for basic testing
			m_pfCRingBuffer = new CRingBuffer<float>(m_iBlockSize); //for file testing

			//initialize default parameters
			m_iReadIdx = 0;
			m_iWriteIdx = 0;

			m_ppfAudioData = 0;

			//allocate memory
			m_ppfAudioData = new float* [m_iNumChannels];
			for (int i = 0; i < m_iNumChannels; i++)
			{
				m_ppfAudioData[i] = new float[m_iBlockSize];
			}

		}

		~RingBuffData()
		{
			//destructor
			m_piCRingBuffer->reset();
			m_pfCRingBuffer->reset();
			delete m_piCRingBuffer;
			delete m_pfCRingBuffer;

			assert(m_ppfAudioData != 0);
			for (int i = 0; i < m_iNumChannels; i++)
				delete[] m_ppfAudioData[i];
			delete[] m_ppfAudioData;
		}

		CRingBuffer<int>* m_piCRingBuffer; //for basic testing
		CRingBuffer<float>* m_pfCRingBuffer; //for file testing

		float** m_ppfAudioData;
		static const int testSeq1[5];
        static const int testSeq2[8];

		static const int m_iBufferLengthInSamples; //for basic testing
		static const int m_iBlockSize; //for file testing
		static const int m_iNumChannels;

		int m_iWriteIdx;
		int m_iReadIdx;
	};

	const int RingBuffData::m_iBufferLengthInSamples = 5;
	const int RingBuffData::m_iBlockSize = 1024;

	const int RingBuffData::testSeq1[5] = { 6, 8, 2, 13, 24 };
    const int RingBuffData::testSeq2[8] = { 3, 31, 27, 5, 0, 88, 9, 6 };

	const int RingBuffData::m_iNumChannels = 1;

	//test get/set read/write idx functions
	TEST_FIXTURE(RingBuffData, GetSetIdx)
	{
        m_piCRingBuffer->reset();

		m_iReadIdx = m_piCRingBuffer->getReadIdx();
		m_iWriteIdx = m_piCRingBuffer->getWriteIdx();

		//check buffer length
		CHECK_EQUAL(m_piCRingBuffer->getLength(), m_iBufferLengthInSamples);

		//check zero initialization
		CHECK(m_iReadIdx == 0);
		CHECK(m_iReadIdx == m_iWriteIdx);

		for (int i = 0; i < (2 * m_iBufferLengthInSamples); i++)
		{
			m_piCRingBuffer->setReadIdx(i);
			m_piCRingBuffer->setWriteIdx(i);

			m_iReadIdx = m_piCRingBuffer->getReadIdx();
			m_iWriteIdx = m_piCRingBuffer->getWriteIdx();

			//check setting and overflow handling
			CHECK(m_iReadIdx == (i % m_iBufferLengthInSamples));
			CHECK(m_iReadIdx == m_iWriteIdx);
		}
	}

	//test put and get with single values and arrays
	TEST_FIXTURE(RingBuffData, GetPut)
	{
        
        int seq2OvrWrt[5] = { 88, 9, 6, 5, 0 };
        int resultSeq[5];

        m_piCRingBuffer->reset();

        //simple write and read one by one
        for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
        {
            m_piCRingBuffer->put(testSeq1[i]);
            m_piCRingBuffer->setWriteIdx(i + 1);
        }

        for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
        {
            resultSeq[i] = m_piCRingBuffer->get();
            m_piCRingBuffer->setReadIdx(i + 1);
        }

        CHECK_ARRAY_EQUAL(testSeq1, resultSeq, m_iBufferLengthInSamples);

        m_piCRingBuffer->reset();

        //write and read one by one with overflow
        for (int i = 0; i < 8; i++)
        {
            m_piCRingBuffer->put(testSeq2[i]);
            m_piCRingBuffer->setWriteIdx(i + 1);
        }

        for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
        {
            resultSeq[i] = m_piCRingBuffer->get();
            m_piCRingBuffer->setReadIdx(i + 1);
        }

		CHECK_ARRAY_EQUAL(seq2OvrWrt, resultSeq, m_iBufferLengthInSamples);

		m_piCRingBuffer->reset();

        //simple write and read array
        m_piCRingBuffer->put(testSeq1, m_iBufferLengthInSamples);
		m_piCRingBuffer->get(resultSeq, m_iBufferLengthInSamples);

		m_iReadIdx = m_piCRingBuffer->getReadIdx();
		m_iWriteIdx = m_piCRingBuffer->getWriteIdx();

		CHECK_ARRAY_EQUAL(testSeq1, resultSeq, m_iBufferLengthInSamples);
		CHECK(m_iReadIdx == 0);
		CHECK(m_iWriteIdx == 0);

		m_piCRingBuffer->reset();

        //write and read array with overflow
        m_piCRingBuffer->put(testSeq2, 8);
        m_piCRingBuffer->get(resultSeq, m_iBufferLengthInSamples);
        
		CHECK_ARRAY_EQUAL(seq2OvrWrt, resultSeq, m_iBufferLengthInSamples);
	}

	//test putPostInc and getPostInc with single values and arrays
	TEST_FIXTURE(RingBuffData, GetPutPost)
	{
		int seq2OvrWrt[5] = { 88, 9, 6, 5, 0 };
		int resultSeq[5];

		m_piCRingBuffer->reset();

		//simple write and read one by one
		for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
		{
			m_piCRingBuffer->putPostInc(testSeq1[i]);
		}

		for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
		{
			resultSeq[i] = m_piCRingBuffer->getPostInc();
		}

		CHECK_ARRAY_EQUAL(testSeq1, resultSeq, m_iBufferLengthInSamples);

		m_piCRingBuffer->reset();

		//write and read one by one with overflow
		for (int i = 0; i < 8; i++)
		{
			m_piCRingBuffer->putPostInc(testSeq2[i]);
		}

		for (int i = 0; i < m_piCRingBuffer->getLength(); i++)
		{
			resultSeq[i] = m_piCRingBuffer->getPostInc();
		}

		CHECK_ARRAY_EQUAL(seq2OvrWrt, resultSeq, m_iBufferLengthInSamples);

		m_piCRingBuffer->reset();

		//simple write and read array
		m_piCRingBuffer->putPostInc(testSeq1, m_iBufferLengthInSamples);
		m_piCRingBuffer->getPostInc(resultSeq, m_iBufferLengthInSamples);

		CHECK_ARRAY_EQUAL(testSeq1, resultSeq, m_iBufferLengthInSamples);

		m_piCRingBuffer->reset();

		//write and read array with overflow
		m_piCRingBuffer->putPostInc(testSeq2, 8);
		m_piCRingBuffer->getPostInc(resultSeq, m_iBufferLengthInSamples);

		CHECK_ARRAY_EQUAL(seq2OvrWrt, resultSeq, m_iBufferLengthInSamples);
	}

	//test getNumValuesInBuffer, full vs empty
	TEST_FIXTURE(RingBuffData, FullEmpty)
	{
        m_piCRingBuffer->reset();

		int numVals = 0;
		numVals = m_piCRingBuffer->getNumValuesInBuffer();

		//check initial size is zero
		CHECK_EQUAL(numVals, 0);

		for (int i = 0; i < m_iBufferLengthInSamples; i++)
		{
			m_piCRingBuffer->putPostInc(testSeq1[i]);
		}

		numVals = m_piCRingBuffer->getNumValuesInBuffer();
		//check buffer is now full
		CHECK_EQUAL(numVals, m_iBufferLengthInSamples);
	}

	//test using ring buffer to delay an audio signal
	TEST_FIXTURE(RingBuffData, BuffAudio)
	{
		float** m_ppfDelayedOutput = 0;
		m_ppfDelayedOutput = new float* [m_iNumChannels];

		m_pfCRingBuffer->reset();
		m_pfCRingBuffer->setWriteIdx(m_iBlockSize / 2); //set delay to half block size
		
		for (int i = 0; i < m_iNumChannels; i++)
		{
			m_ppfDelayedOutput[i] = new float[m_iBlockSize];
			CSynthesis::generateNoise(m_ppfAudioData[i], m_iBlockSize);

			m_pfCRingBuffer->put(m_ppfAudioData[i], m_iBlockSize);
			m_pfCRingBuffer->get(m_ppfDelayedOutput[i], m_iBlockSize);

			//check that original = delayed version at delay index
			CHECK_EQUAL(m_ppfAudioData[i][2], m_ppfDelayedOutput[i][2 + (m_iBlockSize / 2)]);
		}
		
		for (int i = 0; i < m_iNumChannels; i++)
		{
			delete[] m_ppfDelayedOutput[i];
		}

		delete[] m_ppfDelayedOutput;
		
	}

	//test get with offset (linear interpolation)
	TEST_FIXTURE(RingBuffData, GetOffset)
	{
		m_pfCRingBuffer->reset();
		float tmpBuf[3] = { 1, 8, 2 };

		m_pfCRingBuffer->putPostInc(tmpBuf, 3);
		m_pfCRingBuffer->setReadIdx(1);

		//testSeq1[1] = 8, testSeq1[2] = 2 -> testSeq1[1.5] = 5
		CHECK_EQUAL(5, m_pfCRingBuffer->get(0.5));
		CHECK_EQUAL(3.5, m_pfCRingBuffer->get(0.75));
		CHECK_EQUAL(6.5, m_pfCRingBuffer->get(0.25));
	}
}

//===========================================================================================================
// Vibrato Tests

SUITE(Vibrato)
{
	struct VibratoData
	{
		VibratoData()
		{
			//constructor
			m_pCVib = new CVibrato();

			//initialize default parameters
			m_iWidth = 0;
			m_fModFreq = 0;

			m_ppfAudioData = 0;
			m_ppfOutput = 0;
			m_fSampRate = 44100;

			//allocate memory
			m_ppfAudioData = new float* [m_iNumChannels];
			m_ppfOutput = new float* [m_iNumChannels];

			for (int i = 0; i < m_iNumChannels; i++)
			{
				m_ppfAudioData[i] = new float[m_iBlockSize];
				m_ppfOutput[i] = new float[m_iBlockSize];

				CSynthesis::generateSine(m_ppfAudioData[i], 440.F, 44100.F, m_iBlockSize);
			}
		}

		~VibratoData()
		{
			//destructor
			//m_pCVib->reset();
			delete m_pCVib;

			//free memory
			assert(m_ppfAudioData != 0);
			for (int i = 0; i < m_iNumChannels; i++)
			{
				delete[] m_ppfAudioData[i];
				delete[] m_ppfOutput[i];
			}
			delete[] m_ppfAudioData;
			delete[] m_ppfOutput;
		}

		CVibrato* m_pCVib;
		int m_iWidth;
		float m_fModFreq;
		float m_fSampRate;

		float** m_ppfAudioData;
		float** m_ppfOutput;

		static const int m_iBlockSize;
		static const int m_iNumChannels;
	};

	const int VibratoData::m_iBlockSize = 1000;
	const int VibratoData::m_iNumChannels = 1;

	//when modulation freq = 0, output = delayed input
	TEST_FIXTURE(VibratoData, ZeroModFreq)
	{
		/*
		RESET VIBRATO
		*/

		m_fModFreq = 0;
		m_iWidth = 70;

		/*
		CALL VIBRATO FUNCTIONS
		*/

		for (int c = 0; c < m_iNumChannels; c++)
		{
			for(int i = 0; i < m_iBlockSize; i++)
				CHECK_CLOSE(m_ppfAudioData[c][i], m_ppfOutput[c][i+m_iWidth], 1e-3);
		}
	}

	//if DC input, param have no effect on output
	TEST_FIXTURE(VibratoData, DCInput)
	{
		/*
		RESET VIBRATO
		*/

		m_iWidth = 40;
		m_fModFreq = 11;

		int DCInput[m_iBlockSize];
		for (int i = 0; i < m_iBlockSize; i++)
			DCInput[i] = 7;

		/*
		CALL VIBRATO FUNCTIONS
		*/

		CHECK_ARRAY_CLOSE(DCInput, m_ppfOutput[0], m_iBlockSize, 1e-3);
	}

	//test with varying block sizes
	TEST_FIXTURE(VibratoData, VaryBlockSize)
	{
		/*
		RESET VIBRATO
		*/

		m_iWidth = 40;
		m_fModFreq = 10;
		int tmpBlk = 560;
		float tmpBuff[m_iNumChannels][m_iBlockSize];

		/*
		CALL VIBRATO FUNCTIONS
		FIRST WITH STANDARD BLOCK SIZE
		*/

		/*
		RESET VIBRATO
		*/

		m_iWidth = 40;
		m_fModFreq = 10;

		/*
		CALL VIBRATO FUNCTIONS
		WITH SECOND BLOCK SIZE
		*/

		for (int i = 0; i < m_iNumChannels; i++)
			CHECK_ARRAY_CLOSE(m_ppfOutput[i], tmpBuff[i], m_iBlockSize, 1e-3);
	}

	//zero input -> zero output
	TEST_FIXTURE(VibratoData, ZeroInput)
	{
		/*
		RESET VIBRATO
		*/

		m_iWidth = 80;
		m_fModFreq = 20.5;

		int zeroInput[m_iBlockSize];
		for (int i = 0; i < m_iBlockSize; i++)
			zeroInput[i] = 0;

		/*
		CALL VIBRATO FUNCTIONS
		*/

		CHECK_ARRAY_CLOSE(zeroInput, m_ppfOutput[0], m_iBlockSize, 1e-3);
	}

	//when mod width = 0, output = input
	TEST_FIXTURE(VibratoData, ZeroModWidth)
	{
		/*
		RESET VIBRATO
		*/

		m_iWidth = 0;
		m_fModFreq = 5;

		/*
		CALL VIBRATO FUNCTIONS
		*/

		for(int i = 0; i < m_iNumChannels; i++)
			CHECK_ARRAY_CLOSE(m_ppfAudioData[i], m_ppfOutput[i], m_iBlockSize, 1e-3);
	}
}

//=================================================================================================
// LFO tests

SUITE(LFO)
{
	struct LFOData
	{
		LFOData()
		{
			//constructor
			testLFO = new CLFO(m_fLFOFreq, m_fSampRate);

			//allocate memory
			m_pfLFOOutput = new float[m_iBuffSize];
			m_pfExpected = new float[m_iBuffSize];
			m_pfLFOOutputLong = new float[2*m_iBuffSize];
			m_pfExpectedLong = new float[2*m_iBuffSize];
		}

		~LFOData()
		{
			//destructor
			delete testLFO;

			//free memory
			delete[] m_pfLFOOutput;
			delete[] m_pfExpected;
			delete[] m_pfLFOOutputLong;
			delete[] m_pfExpectedLong;
		}

		CLFO* testLFO;
		float* m_pfLFOOutput;
		float* m_pfExpected;
		float* m_pfLFOOutputLong;
		float* m_pfExpectedLong;

		static const float m_fLFOFreq ;
		static const float m_fSampRate;
		static const int m_iBuffSize;
	};

	const float LFOData::m_fLFOFreq = 10;
	const float LFOData::m_fSampRate = 44100;
	const int LFOData::m_iBuffSize = static_cast<int>(LFOData::m_fSampRate / LFOData::m_fLFOFreq);

	TEST_FIXTURE(LFOData, GenerateLFO)
	{
		CSynthesis::generateSine(m_pfExpected, m_fLFOFreq, m_fSampRate, m_iBuffSize);

		for (int i = 0; i < m_iBuffSize; i++)
		{
			m_pfLFOOutput[i] = testLFO->getLFO(i);
		}

		CHECK_ARRAY_CLOSE(m_pfExpected, m_pfLFOOutput, m_iBuffSize, 1e-3);
	}

	TEST_FIXTURE(LFOData, GenerateTwoCycles)
	{
		CSynthesis::generateSine(m_pfExpectedLong, m_fLFOFreq, m_fSampRate, 2*m_iBuffSize);

		for (int i = 0; i < 2*m_iBuffSize; i++)
		{
			m_pfLFOOutputLong[i] = testLFO->getLFO(i);
		}

		CHECK_ARRAY_CLOSE(m_pfExpectedLong, m_pfLFOOutputLong, 2*m_iBuffSize, 1e-3);
	}
}

#endif //WITH_TESTS