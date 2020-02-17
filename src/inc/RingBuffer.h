#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>


/*! \brief implement a circular buffer of type T
*/
template <class T>
class CRingBuffer
{
public:
    explicit CRingBuffer(int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples)
    {
        assert(iBufferLengthInSamples > 0);

        // allocate and init
        m_piBuff = new T[m_iBuffLength];
        reset();
    }

    virtual ~CRingBuffer()
    {
        assert(m_piBuff);

        //free memory and reset members to zero
        reset();

        delete[] m_piBuff;
        m_piBuff = 0;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc(T tNewValue)
    {
        put(tNewValue); //write new value
        m_iWriteIdx = (m_iWriteIdx + 1) % m_iBuffLength; //increment write index

        return;
    }

    /*! add new values of type T to write index and increment write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void putPostInc(const T* ptNewBuff, int iLength)
    {
        put(ptNewBuff, iLength); //write new values
        m_iWriteIdx = (m_iWriteIdx + iLength) % m_iBuffLength; //jump write index

        return;
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        assert(m_piBuff);
        m_piBuff[m_iWriteIdx] = tNewValue;

        return;
    }

    /*! add new values of type T to write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void put(const T* ptNewBuff, int iLength)
    {
        assert(m_piBuff);

        int tempWriteIdx = m_iWriteIdx; //temp write index so the real index is not incremented

        //write new values and increment temp write index
        for (int i = 0; i < iLength; i++)
        {
            m_piBuff[tempWriteIdx] = ptNewBuff[i];
            tempWriteIdx = (tempWriteIdx + 1) % m_iBuffLength;
        }

        return;
    }

    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc()
    {
        assert(m_piBuff);

        T val = m_piBuff[m_iReadIdx]; //get value
        m_iReadIdx = (m_iReadIdx + 1) % m_iBuffLength; //increment

        return static_cast<T>(val);
    }

    /*! return the values starting at the current read index and increment the read pointer
    \param ptBuff: pointer to where the values will be written
    \param iLength: number of values
    \return void
    */
    void getPostInc(T* ptBuff, int iLength)
    {
        get(ptBuff, iLength); //read values
        m_iReadIdx = (m_iReadIdx + iLength) % m_iBuffLength; //jump read index

        return;
    }

    /*! return the value at the current read index
    \param fOffset: read at offset from read index
    \return float the value from the read index
    */
    T get(float fOffset = 0.f) const
    {
        assert(m_piBuff);
        assert(!(fOffset < 0));

        if (fOffset == 0)
        {
            return static_cast<T>(m_piBuff[m_iReadIdx]);
        }
        else
        {
            return static_cast<T>((m_piBuff[(m_iReadIdx + 1)%m_iBuffLength] * fOffset) + (m_piBuff[m_iReadIdx] * (1 - fOffset)));
        }
    }

    /*! return the values starting at the current read index
    \param ptBuff to where the values will be written
    \param iLength: number of values
    \return void
    */
    void get(T* ptBuff, int iLength) const
    {
        assert(m_piBuff);

        int tempReadIdx = m_iReadIdx; //temp read index so the real index is not incremented

        //read values and increment temp read index
        for (int i = 0; i < iLength; i++)
        {
            ptBuff[i] = m_piBuff[tempReadIdx];
            tempReadIdx = (tempReadIdx + 1) % m_iBuffLength;
        }

        return;
    }

    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        //set indices to 0
        m_iWriteIdx = 0;
        m_iReadIdx = 0;

        assert(m_piBuff);
        for (int i = 0; i < m_iBuffLength; i++)
        {
            m_piBuff[i] = 0; //set buffer content to zero
        }

        return;
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWriteIdx;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        if (iNewWriteIdx < 0) //negative index
        {
            int i = m_iBuffLength + iNewWriteIdx;
            m_iWriteIdx = i;
        }
        else if (iNewWriteIdx > (m_iBuffLength - 1)) //out of bounds index
        {
            m_iWriteIdx = iNewWriteIdx % m_iBuffLength;
        }
        else
            m_iWriteIdx = iNewWriteIdx;

        return;
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadIdx;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx(int iNewReadIdx)
    {
        if (iNewReadIdx < 0) //negative index
        {
            int i = m_iBuffLength + iNewReadIdx;
            m_iReadIdx = i;
        }
        else if (iNewReadIdx > (m_iBuffLength - 1)) //out of bounds index
        {
            m_iReadIdx = iNewReadIdx % m_iBuffLength;
        }
        else
            m_iReadIdx = iNewReadIdx;

        return;
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        if (m_iWriteIdx < m_iReadIdx) //read is behind zero point, write is over
        {
            int writeAdj = m_iWriteIdx + m_iBuffLength;
            return (writeAdj - m_iReadIdx);
        }
        else if (m_iWriteIdx > m_iReadIdx) //read is at or over the zero point
        {
            return (m_iWriteIdx - m_iReadIdx);
        }
        else //read and write are at the same place
        {
            //check if buffer is empty (zeros)
            for (int i = 0; i < m_iBuffLength; i++)
            {
                if (m_piBuff[i] != 0)
                {
                    return m_iBuffLength; //buffer is full
                }
            }

            return 0; //if all zeros, buffer is empty
        }
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength() const
    {
        return m_iBuffLength;
    }
private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    T* m_piBuff;                    //!< buffer
    int m_iWriteIdx;                //!< write index
    int m_iReadIdx;                 //!< read index
};
#endif // __RingBuffer_hdr__