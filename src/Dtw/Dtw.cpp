#include "Vector.h"
#include "Util.h"
#include <algorithm>
#include <iostream>

#include "Dtw.h"

CDtw::CDtw(void)
{

}

CDtw::~CDtw(void)
{

}

Error_t CDtw::init(int iNumRows, int iNumCols)
{
    //assert(iNumCols > 0);
    //assert(iNumRows > 0);

    if (iNumCols < 1 || iNumRows < 1)
        return  kFunctionInvalidArgsError;

    m_iNumRows = iNumRows;
    m_iNumCols = iNumCols;

    // Allocate memory for cumulative cost matrix and min cost path matrix.
    m_ppfCumulativeCostMatrix = new float* [m_iNumRows]();
    m_ppkMinCostPathMatrix = new Directions_t * [m_iNumRows]();
    for (int i = 0; i < m_iNumRows; i++) {
        m_ppfCumulativeCostMatrix[i] = new float[m_iNumCols]();
        m_ppkMinCostPathMatrix[i] = new Directions_t[m_iNumCols]();
    }

    // Allocate memory for path buffer
    m_ppiMinCostPathBuffer = new int* [2];
    m_ppiMinCostPathStart = new int* [2];
    for (int i = 0; i < 2; i++)
        m_ppiMinCostPathBuffer[i] = new int[m_iNumRows + m_iNumCols];

    // Set path start pointer to end of path buffer
    m_iMinCostPathLength = -1;
    m_fMinPathCost = -1;

    return kNoError;
}

Error_t CDtw::reset()
{
    assert(m_ppfCumulativeCostMatrix);
    assert(m_ppiMinCostPathBuffer);

    if (!m_ppfCumulativeCostMatrix)
        return kNotInitializedError;

    m_fMinPathCost = -1;
    m_iMinCostPathLength = -1;
    m_iNumRows = -1;
    m_iNumCols = -1;


    // Reset path buffer adn path start pointer
    for (int i = 0; i < 2; i++)
        delete[] m_ppiMinCostPathBuffer[i];
    delete[] m_ppiMinCostPathBuffer;
    delete[] m_ppiMinCostPathStart;
    m_ppiMinCostPathBuffer = nullptr;
    m_ppiMinCostPathStart = nullptr;

    // Reset cumulative cost matrix and path matrix
    for (int i = 0; i < m_iNumRows; i++) {
        delete[] m_ppkMinCostPathMatrix[i];
        delete[] m_ppfCumulativeCostMatrix[i];
    }
    delete[] m_ppkMinCostPathMatrix;
    delete[] m_ppfCumulativeCostMatrix;
    m_ppkMinCostPathMatrix = nullptr;
    m_ppfCumulativeCostMatrix = nullptr;

    return kNoError;
}

Error_t CDtw::process(float** ppfDistanceMatrix)
{
    assert(ppfDistanceMatrix);
    if (!ppfDistanceMatrix)
        return kFunctionInvalidArgsError;
    if (!**ppfDistanceMatrix)
        return kFunctionInvalidArgsError;
    if (!*ppfDistanceMatrix)
        return kNotInitializedError;

    // Calculate cumulative cost matrix and path matrix
    m_ppfCumulativeCostMatrix[0][0] = ppfDistanceMatrix[0][0];
    m_ppkMinCostPathMatrix[0][0] = kDiag;
    for (int i = 1; i < m_iNumRows; i++) {
        m_ppfCumulativeCostMatrix[i][0] = m_ppfCumulativeCostMatrix[i - 1][0] + ppfDistanceMatrix[i][0];
        m_ppkMinCostPathMatrix[i][0] = kVert;
    }
    for (int j = 1; j < m_iNumCols; j++) {
        m_ppfCumulativeCostMatrix[0][j] = m_ppfCumulativeCostMatrix[0][j - 1] + ppfDistanceMatrix[0][j];
        m_ppkMinCostPathMatrix[0][j] = kHoriz;
    }

    for (int i = 1; i < m_iNumRows; i++) {
        for (int j = 1; j < m_iNumCols; j++) {
            int min;
            Directions_t dir;

            if (m_ppfCumulativeCostMatrix[i - 1][j] < m_ppfCumulativeCostMatrix[i][j - 1]) {
                min = m_ppfCumulativeCostMatrix[i - 1][j];
                dir = kVert;
            }
            else {
                min = m_ppfCumulativeCostMatrix[i][j - 1];
                dir = kHoriz;
            }

            if (m_ppfCumulativeCostMatrix[i - 1][j - 1] < min) {
                m_ppfCumulativeCostMatrix[i][j] = m_ppfCumulativeCostMatrix[i - 1][j - 1] + ppfDistanceMatrix[i][j];
                m_ppkMinCostPathMatrix[i][j] = kDiag;
            }
            else {
                m_ppfCumulativeCostMatrix[i][j] = min + ppfDistanceMatrix[i][j];
                m_ppkMinCostPathMatrix[i][j] = dir;
            }
        }
    }

    // Find min cost path from path matrix
    int i = m_iNumRows - 1;
    int j = m_iNumCols - 1;
    m_fMinPathCost = m_ppfCumulativeCostMatrix[i][j];

    int k = m_iNumRows + m_iNumCols;

    do {
        --k;
        m_ppiMinCostPathBuffer[0][k] = i;
        m_ppiMinCostPathBuffer[1][k] = j;

        //std::cout << m_ppiMinCostPathBuffer[0][k] << " " << m_ppiMinCostPathBuffer[1][k] << " " << k << std::endl;

        if (m_ppkMinCostPathMatrix[i][j] == kHoriz)
            --j;
        else if (m_ppkMinCostPathMatrix[i][j] == kVert)
            --i;
        else if (m_ppkMinCostPathMatrix[i][j] == kDiag) {
            --i;
            --j;
        }
    } while ((i >= 0) || (j >= 0));

    m_iMinCostPathLength = m_iNumRows + m_iNumCols - k;
    m_ppiMinCostPathStart[0] = m_ppiMinCostPathBuffer[0] + k;
    m_ppiMinCostPathStart[1] = m_ppiMinCostPathBuffer[1] + k;

    return kNoError;
}

int CDtw::getPathLength()
{
    return m_iMinCostPathLength;
}

float CDtw::getPathCost() const
{
    return m_fMinPathCost;
}

Error_t CDtw::getPath(int** ppiPathResult) const
{
    if (!m_ppiMinCostPathStart)
        return kNotInitializedError;

    for (int k = 0; k < m_iMinCostPathLength; k++)
    {
        ppiPathResult[0][k] = m_ppiMinCostPathStart[0][k];
        ppiPathResult[1][k] = m_ppiMinCostPathStart[1][k];
    }
    /*ppiPathResult = m_ppiMinCostPathStart;
    std::cout << "***************" << std::endl;
    std::cout << ppiPathResult[0][0] << std::endl;
    */
    return kNoError;
}