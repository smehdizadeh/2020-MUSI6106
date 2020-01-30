
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilterIf.h"
#include "CombFilter.h"

static const char*  kCMyProjectBuildDate             = __DATE__;
long int       CCombFilterIf::m_kiDelayLineLength; //delay line length

CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),
    //m_pCCombFilter(0),
    m_fSampleRate(0)
{
    // this never hurts
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}


const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create( CCombFilterIf*& pCCombFilter)
{
    pCCombFilter = new CCombFilter();

    if (!pCCombFilter)
        return kMemError;

    return kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = 0;

    return kNoError;
}

Error_t CCombFilterIf::init( CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels )
{
    // check parameters
    if (fMaxDelayLengthInS < 0 || fSampleRateInHz < 0 || iNumChannels < 0)
        return kFunctionInvalidArgsError;

    reset();

    m_eFilterType = eFilterType;
    m_fSampleRate = fSampleRateInHz;
    m_fFilterGain = 1;
    m_iNumChannels = iNumChannels;
    m_kiDelayLineLength = fMaxDelayLengthInS * fSampleRateInHz;
    m_iDelaySamp = m_kiDelayLineLength;

    //setParam(FilterParam_t::kParamDelay, fMaxDelayLengthInS);

    m_bIsInitialized = true;

    //allocate memory for delay line
    return allocMemory();
}

Error_t CCombFilterIf::reset ()
{
    m_bIsInitialized = false;

    //reset internal variables
    m_fSampleRate = 0;
    m_fFilterGain = 0;
    m_iNumChannels = 0;
    m_iDelaySamp = 0;
    m_kiDelayLineLength = 0;
    setParam(FilterParam_t::kParamDelay, 0);
    setParam(FilterParam_t::kParamGain, 0);


    return freeMemory(); //free delay line memory
}

Error_t CCombFilterIf::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    // check parameters
    if (!ppfInputBuffer || !ppfOutputBuffer || !ppfInputBuffer[0] || !ppfOutputBuffer[0] || iNumberOfFrames < 0)
        return kFunctionInvalidArgsError;

    // check file properties
    if (!m_bIsInitialized)
        return kNotInitializedError;

    //check filter type
    if (m_eFilterType == CCombFilterIf::CombFilterType_t::kCombFIR) //FIR filter implementation
        return processFIRIntern(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames, m_iDelaySamp);
    else if (m_eFilterType == CCombFilterIf::CombFilterType_t::kCombIIR) //IIR filter implementation
        return processIIRIntern(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames, m_iDelaySamp);
    else
        return kFunctionInvalidArgsError; //if m_eFilterType is not FIR or IIR as specified above, return invalid
}

Error_t CCombFilterIf::setParam( FilterParam_t eParam, float fParamValue )
{
    // check init status
    if (!m_bIsInitialized)
        return kNotInitializedError;

    //find parameter
    if (eParam == FilterParam_t::kParamDelay)
    {
        if (fParamValue < 0)
            return kFunctionInvalidArgsError;
        else m_iDelaySamp = fParamValue * m_fSampleRate;
    }

    if (eParam == FilterParam_t::kParamGain)
    {
        if (fParamValue < -1 || fParamValue > 1)
            return kFunctionInvalidArgsError;
        else m_fFilterGain = fParamValue;
    }

    return kNoError;
}

float CCombFilterIf::getParam( FilterParam_t eParam ) const
{
    // check init status
    if (!m_bIsInitialized)
        return kNotInitializedError;

    //find parameter
    if (eParam == FilterParam_t::kParamDelay) //delay parameter
        return (m_iDelaySamp / m_fSampleRate);

    else if (eParam == FilterParam_t::kParamGain) //gain parameter
        return m_fFilterGain;

    else
        return kFunctionInvalidArgsError; //if eParam is not gain or delay as specified above, return invalid
}

Error_t CCombFilterIf::freeMemory()
{
    return kNoError;
}

Error_t CCombFilterIf::allocMemory()
{
    freeMemory();

    return kNoError;
}

int CCombFilterIf::getNumChannels() const
{
    return m_iNumChannels;
}