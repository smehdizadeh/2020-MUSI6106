#include <cassert>
#include <iostream>

#include "Util.h"
#include "Vector.h"

#include "CombFilter.h"

using namespace std;

void shiftDelayLine(float* delayLine, long length, float newVal);
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

CCombFilter::CCombFilter() : CCombFilterIf(),
m_piDelayLine(0)
{
    reset();
}

CCombFilter::~CCombFilter()
{
    reset();
}

Error_t CCombFilter::freeMemory()
{
    CCombFilterIf::freeMemory();
    if (!m_piDelayLine)
        return kNoError;

    int iNumChannels = getNumChannels();
    for (int i = 0; i < iNumChannels; i++)
        delete[] m_piDelayLine[i];

    delete[] m_piDelayLine;
    m_piDelayLine = 0;

    return kNoError;
}

Error_t CCombFilter::allocMemory()
{
    freeMemory();

    int iNumChannels = getNumChannels();
    m_piDelayLine = new float* [iNumChannels];
    for(int i = 0; i < iNumChannels; i++)
        m_piDelayLine[i] = new float[static_cast<unsigned int>(m_kiDelayLineLength)];

    //initialize the delay line
    for (int c = 0; c < iNumChannels; c++)
    {
        for (int i = 0; i < m_kiDelayLineLength; i++)
        {
            m_piDelayLine[c][i] = 0;
        }
    }

    if (!m_piDelayLine)
        return kMemError;
    else
        return kNoError;
}

Error_t CCombFilter::processFIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames)
{
    int iNumChannels = getNumChannels();
    float g = getParam(CCombFilterIf::FilterParam_t::kParamGain);

    // sanity check
    assert(ppfInputBuffer || ppfInputBuffer[0]);

    //if delay is zero
    if (m_kiDelayLineLength == 0)
    {
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumberOfFrames; i++)
            {
                ppfOutputBuffer[iCh][i] = ppfInputBuffer[iCh][i]; //copy input to output
            }
        }
    }
    else //for non zero delay
    {
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumberOfFrames; i++)
            {
                ppfOutputBuffer[iCh][i] = ppfInputBuffer[iCh][i] + (g * m_piDelayLine[iCh][m_kiDelayLineLength - 1]);
                shiftDelayLine(m_piDelayLine[iCh], m_kiDelayLineLength, ppfInputBuffer[iCh][i]);
            }
        }
    }

    return kNoError;
}

Error_t CCombFilter::processIIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames)
{
    int iNumChannels = getNumChannels();
    float g = getParam(CCombFilterIf::FilterParam_t::kParamGain);

    // sanity check
    assert(ppfInputBuffer || ppfInputBuffer[0]);

    //if delay is zero
    if (m_kiDelayLineLength == 0)
    {
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumberOfFrames; i++)
            {
                ppfOutputBuffer[iCh][i] = ppfInputBuffer[iCh][i]; //copy input to output
            }
        }
    }
    else //for non zero delay
    {
        for (int iCh = 0; iCh < iNumChannels; iCh++)
        {
            for (int i = 0; i < iNumberOfFrames; i++)
            {
                ppfOutputBuffer[iCh][i] = ppfInputBuffer[iCh][i] + (g * m_piDelayLine[iCh][m_kiDelayLineLength - 1]);
                shiftDelayLine(m_piDelayLine[iCh], m_kiDelayLineLength, ppfOutputBuffer[iCh][i]);
            }
        }
    }

    return kNoError;
}

void shiftDelayLine(float* delayLine, long length, float newVal)
{
    float temp1 = delayLine[0];
    float temp2 = 0;

    //shift delay line, popping off last value
    for (int i = 1; i < length; i++)
    {
        temp2 = delayLine[i];
        delayLine[i] = temp1;
        temp1 = temp2;
    }

    //replace first element
    delayLine[0] = newVal;

    return;
}