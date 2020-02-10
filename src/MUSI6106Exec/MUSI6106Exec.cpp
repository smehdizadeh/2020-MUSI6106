
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"
#include "RingBuffer.h"

using std::cout;
using std::endl;

// local function declarations
int     readWriteFile(std::string sInputFilePath, std::string sOutputFilePath);
void    showClInfo ();
void    test1();
void    test2();
void    test3();
void    test5();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!" << endl;
        cout << "Running tests..." << endl;
        test1();
        test2();
        test3();
        test5();
        return 0;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + "output.wav";
    }
    

    return readWriteFile(sInputFilePath, sOutputFilePath);

}

int    readWriteFile(std::string sInputFilePath, std::string sOutputFilePath)
{
    // declare variables
    CAudioFileIf* phInputAudioFile = 0;          //!< audio file objects
    CAudioFileIf* phOutputAudioFile = 0;

    CAudioFileIf::FileSpec_t    stFileSpec;                     //!< file specs

    float** ppfInputAudioData = 0;        //!< audio data
    float** ppfOutputAudioData = 0;

    static const int            kBlockSize = 1024;

    CRingBuffer<float>* pCRingBuffer = new CRingBuffer<float>(kBlockSize);

    clock_t                     time = 0;

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    phInputAudioFile->create(phInputAudioFile); //allocate memory and create instance
    phInputAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead); //open input file for reading
    if (!phInputAudioFile->isOpen()) { //check
        cout << "Input file open error!";
        return -1;
    }
    phInputAudioFile->getFileSpec(stFileSpec); //get file specs

    //////////////////////////////////////////////////////////////////////////////
    // open the output wave file
    phOutputAudioFile->create(phOutputAudioFile); //allocate memory and create instance
    phOutputAudioFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec); //open output file for writing
    if (!phOutputAudioFile->isOpen()) { //check
        cout << "Output file open error!";
        return -1;
    }

    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    // for audio data
    ppfInputAudioData = new float* [stFileSpec.iNumChannels]; //points to an array for each channel of audio
    ppfOutputAudioData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfInputAudioData[i] = new float[kBlockSize]; //allocate array of kBlockSize
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    time = clock();

    //////////////////////////////////////////////////////////////////////////////
    // set ringbuffer delay to one block
    pCRingBuffer->setWriteIdx(kBlockSize - 1);
    cout << "Output delayed by " << pCRingBuffer->getNumValuesInBuffer() << " samples" << endl;

    //////////////////////////////////////////////////////////////////////////////
    // get audio data, process it, and write it to the output file
    while (!phInputAudioFile->isEof()) //until end of input file is reached
    {
        long long iNumFrames = kBlockSize; //number of frames to read at a time
        //read in 1024 frames from audio file channels 1 and 2
        //store in ppfInputAudioData[0] and ppfInputAudioData[1] respectively
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        for (int c = 0; c < stFileSpec.iNumChannels; c++)
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                pCRingBuffer->putPostInc(ppfInputAudioData[c][i]); //write current value to buffer
                ppfOutputAudioData[c][i] = pCRingBuffer->getPostInc(); //read from buffer
            }
        }

        //write processed audio data to the output file
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;


    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    phInputAudioFile->closeFile(); //close files
    phOutputAudioFile->closeFile();

    CAudioFileIf::destroy(phInputAudioFile); //delete instances
    CAudioFileIf::destroy(phOutputAudioFile);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfInputAudioData[i]; //free memory associated with that channel
        delete[] ppfOutputAudioData[i];
    }

    delete[] ppfInputAudioData; //delete pointer arrays
    delete[] ppfOutputAudioData;

    ppfInputAudioData = 0; //reset to null pointer
    ppfOutputAudioData = 0;

    return 0;
}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

//=============================================================================

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

//=====================================================================================

/*! RingBuffer test #2
\ Test put and get with single values and arrays
*/
void    test2()
{
    //create new ring buffer, size 5, int
    CRingBuffer<int>* pCRingBuffer = new CRingBuffer<int>(5);

    int test2[5] = { 6, 8, 2, 13, 24 };
    int test2long[8] = { 3, 31, 27, 5, 0, 88, 9, 6 };
    int testresult[5] = { 88, 9, 6, 5, 0 };
    int testempty[8];
    //simple write and read one by one
    for (int i = 0; i < pCRingBuffer->getLength(); i++)
    {
        pCRingBuffer->put(test2[i]);
        pCRingBuffer->setWriteIdx(i + 1);
    }

    if (pCRingBuffer->getWriteIdx() != 0) //write index should be back to zero
    {
        cout << "test 2 - write index error" << endl;
        return;
    }

    for (int i = 0; i < pCRingBuffer->getLength(); i++)
    {
        if (pCRingBuffer->get() != test2[i]) //read back values to check
        {
            cout << "test 2 - read/write single FAILED" << endl;
            return;
        }
        pCRingBuffer->setReadIdx(i + 1);
    }

    if (pCRingBuffer->getReadIdx() != 0) //read index should be back to zero
    {
        cout << "test 2 - read index error" << endl;
        return;
    }

    //write and read one by one with overflow
    for (int i = 0; i < 8; i++)
    {
        pCRingBuffer->put(test2long[i]);
        pCRingBuffer->setWriteIdx(i + 1);
    }

    if (pCRingBuffer->getWriteIdx() != 3) //write index should be at 3
    {
        cout << "test 2 - write index overflow error" << endl;
        return;
    }

    if (pCRingBuffer->getNumValuesInBuffer() != 3) //buffer size should register as 3
    {
        cout << "test 2 - buffer size error" << endl;
        return;
    }

    for (int i = 0; i < pCRingBuffer->getLength(); i++)
    {
        if (pCRingBuffer->get() != testresult[i])
        {
            cout << "test 2 - read/write overflow single FAILED" << endl;
            return;
        }
        pCRingBuffer->setReadIdx(i + 1);
    }

    //simple write and read array
    pCRingBuffer->reset();

    pCRingBuffer->put(test2, 5);
    for (int i = 0; i < pCRingBuffer->getLength(); i++)
    {
        if (pCRingBuffer->get() != test2[i]) //test by reading one by one
        {
            cout << "test 2 - read/write array FAILED" << endl;
            return;
        }
        pCRingBuffer->setReadIdx(i + 1);
    }

    pCRingBuffer->get(testempty, 5); //get entire buffer
    for (int i = 0; i < 5; i++)
    {
        if (test2[i] != testempty[i]) //test by reading array
        {
            cout << "test 2 - read/write array FAILED" << endl;
            return;
        }
    }

    if (pCRingBuffer->getReadIdx() != 0 || pCRingBuffer->getWriteIdx() != 0)
    {
        cout << "test 2 - read/write index error" << endl;
        return;
    }

    //write and read array with overflow
    pCRingBuffer->put(test2long, 8);
    pCRingBuffer->get(testempty, pCRingBuffer->getLength());
    for (int i = 0; i < pCRingBuffer->getLength(); i++)
    {
        if (testempty[i] != testresult[i])
        {
            cout << "test 2 - read/write array overflow FAILED" << endl;
            return;
        }
        
    }


    cout << "test 2 - PASSED" << endl;

    //clean up
    pCRingBuffer->reset();
    //pCRingBuffer->~CRingBuffer();
    delete pCRingBuffer;

    return;
}

//=========================================================================================
/*! RingBuffer test #3
\ Test putpostinc and getpostinc with single values and arrays
*/
void test3()
{
    //create new ring buffer, size 5, int
    CRingBuffer<int>* pCRingBuffer = new CRingBuffer<int>(5);
    int test3[3] = { 0, 1, 2 };
    int testempty[5];
    int testresult[5] = { 2,1,2,0,1 };
    int testinvert[5] = { 2,0,1,2,1 };

    //simple read write one by one
    for (int i = 0; i < 3; i++)
    {
        pCRingBuffer->putPostInc(test3[i]);
        if (pCRingBuffer->getPostInc() != test3[i])
        {
            cout << "test 3 - read/write auto inc FAILED" << endl;
        }
    }

    if (pCRingBuffer->getWriteIdx() != 3 || pCRingBuffer->getReadIdx() != 3) //read/write idx should now be 3
    {
        cout << "test 3 - idx auto inc FAILED" << endl;
        return;
    }

    //read write array with overflow
    pCRingBuffer->putPostInc(test3, 3); //writes array again starting at idx 3
    if (pCRingBuffer->getNumValuesInBuffer() != 3) //buffer size should now be 3
    {
        cout << "test 3 - buffer size error" << endl;
        return;
    }
    
    for (int i = 0; i < 3; i++) //get next three values one by one
    {
        if (pCRingBuffer->getPostInc() != test3[i])
        {
            cout << "test 3 - write array auto inc FAILED" << endl;
            return;
        }
    }

    pCRingBuffer->setReadIdx(0); //reset read idx
    pCRingBuffer->getPostInc(testempty, 5); //read entire buffer
    for (int i = 0; i < 5; i++)
    {
        if (testempty[i] != testresult[i])
        {
            //cout << "rb " << testempty[i] << " c " << testresult[i] << endl;
            cout << "test 3 - read/write array auto inc FAILED" << endl;
            return;
        }
    }

    pCRingBuffer->setReadIdx(2);
    pCRingBuffer->getPostInc(testempty, 5); //get entire buffer from read offset
    for (int i = 0; i < 5; i++)
    {
        if (testempty[i] != testinvert[i])
        {
            //cout << "rb " << testempty[i] << " c " << testresult[i] << endl;
            cout << "test 3 - read/write array auto inc offset FAILED" << endl;
            return;
        }
    }

    cout << "test 3 - PASSED" << endl;

    // clean up
        pCRingBuffer->reset();
    //pCRingBuffer->~CRingBuffer();
    delete pCRingBuffer;

    return;
}

//=========================================================================================
/*! RingBuffer test #4
\ Test get with offset
*/

//=========================================================================================
/*! RingBuffer test #5
\ Test with sample input (sine ramp)
\ write input to output, delayed by 1023 samples thru ring buffer
*/
void    test5()
{
    std::string sInputFilePath = "C:/Users/sophi/Documents/GATECH/MUSI6106/samp4_ramp.wav";
    std::string sOutputFilePath = "C:/Users/sophi/Documents/GATECH/MUSI6106/samp4_rampDelay.wav";

    if (readWriteFile(sInputFilePath, sOutputFilePath) != 0)
    {
        cout << "test 5 - error in read write with delay" << endl;
        return;
    }

    cout << "test 5 - complete, check output file" << endl;

    return;
}