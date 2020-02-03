
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"
#include "RingBuffer.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();
    void test1();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!" << endl;
        cout << "Running tests..." << endl;
        test1();
        return 0;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioData[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

/*! RingBuffer test #1
\ Verify initialization
\ Basic functions - constructor, getLength, getNumValuesInBuffer(after init)
\ get and set indeces, get() - make sure buffer init to 0, reset
*/
void    test1()
{
    //create new ring buffer, size 10, int
    CRingBuffer<int> *pCRingBuffer = new CRingBuffer<int>(10);

    if (pCRingBuffer->getLength() != 10) //sanity check, getLength()
    {
        cout << "test 1 - getLength FAILED" << endl;
        return;
    }

    //should read as empty
    if (pCRingBuffer->getNumValuesInBuffer() != 0) //sanity check, getNumValuesInBuffer()
    {
        cout << "test 1 - getNumValuesInBuffer FAILED" << endl;
        return;
    }

    //try resetting
    pCRingBuffer->reset();

    //length should be the same
    if (pCRingBuffer->getLength() != 10) //sanity check, getLength()
    {
        cout << "test 1 - getLength FAILED" << endl;
        return;
    }

    //should still read as empty
    if (pCRingBuffer->getNumValuesInBuffer() != 0) //sanity check, getNumValuesInBuffer()
    {
        cout << "test 1 - getNumValuesInBuffer FAILED" << endl;
        return;
    }

    //read and write indeces should be zero
    if (pCRingBuffer->getReadIdx() != 0 || pCRingBuffer->getWriteIdx() != 0)
    {
        cout << "test 1 - getReadIdx / getWriteIdx FAILED" << endl;
        return;
    }

    //try setting indeces
    pCRingBuffer->setReadIdx(2);
    pCRingBuffer->setWriteIdx(7);
    if (pCRingBuffer->getReadIdx() != 2 || pCRingBuffer->getWriteIdx() != 7)
    {
        cout << "test 1 - setReadIdx / setWriteIdx FAILED" << endl;
        return;
    }

    //should read 5 values in buffer
    if (pCRingBuffer->getNumValuesInBuffer() != 5) //sanity check, getNumValuesInBuffer()
    {
        cout << "test 1 - getNumValuesInBuffer after idx set FAILED" << endl;
        return;
    }

    //reset
    pCRingBuffer->reset();

    //read and write indeces should be zero
    if (pCRingBuffer->getReadIdx() != 0 || pCRingBuffer->getWriteIdx() != 0)
    {
        cout << "test 1 - getReadIdx / getWriteIdx FAILED" << endl;
        return;
    }

    //buffer should be initialized with zeros
    for (int i = 0; i < 10; i++)
    {
        if (pCRingBuffer->get(0) != 0)
        {
            cout << "test 1 - RingBuffer init FAILED" << endl;
            return;
        }
        pCRingBuffer->setReadIdx(i + 1); //increment read index

        //test i=9 case (overflow)
        if (i == 9)
        {
            if (pCRingBuffer->getReadIdx() != 0) //index should overflow to 0
            {
                cout << "test 1 - set index overflow FAILED" << endl;
                return;
            }
        }
    }

    cout << "test 1 - PASSED" << endl;

    //clean up
    pCRingBuffer->reset();
    //pCRingBuffer->~CRingBuffer();
    delete pCRingBuffer;

    return;
}