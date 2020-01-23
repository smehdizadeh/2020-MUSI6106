#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "CombFilterIf.h"


class CCombFilter : public CCombFilterIf
{
public:
    CCombFilter();
    virtual ~CCombFilter();

private:
    CCombFilter(const CCombFilter& that);
    Error_t freeMemory() override;
    Error_t allocMemory() override;
    Error_t processFIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames) override;
    Error_t processIIRIntern(float** ppfInputBuffer, float** ppfOutputBuffer, int iNumberOfFrames) override;

    float  **m_piDelayLine;           //!< delay line
};

#endif  //__CombFilter_hdr__

