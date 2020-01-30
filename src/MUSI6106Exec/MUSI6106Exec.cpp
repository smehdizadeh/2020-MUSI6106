
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
int     filtering(std::string sInputFilePath, std::string sOutputFilePath, std::string sFilterType, std::string sMaxDelayLengthInS, std::string sDelayTime, std::string sFilterGain, int numBlocks);
void    showClInfo();
void    test1();
void    test2();
void    test3(std::string sFilterType);
void    test4(std::string sFilterType);
void    test5(std::string sFilterType);


/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    // declare variables
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath,
                            sFilterType,                    //!< filter params (FIR/IIR) (dekay time)
                            sMaxDelayLengthInS,
                            sDelayTime,
                            sFilterGain;
 

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    // expected: CombFilter.exe <input file path> <FIR/IIR> <delay in sec> <filter gain>
    if (argc == 1) //if called with no command line arguments
    {
        //call test cases
        cout << "Running test cases..." << endl;

        test1();
        cout << endl;
        test2();
        cout << endl;
        test3("FIR");
        test3("IIR");
        cout << endl;
        test4("FIR");
        test4("IIR");
        cout << endl;
        test5("FIR");
        test5("IIR");
        
        return 0;
    }
    else if ((argc < 6) && (argc != 1)) //if called with too few command line arguments
    {
        cout << "Missing argument!" << endl;
        cout << "Please enter <input file path> <FIR/IIR> <max delay in sec> <delay time in sec> <filter gain (-1:1)>" << endl;
        return -1;
    }
    else
    {
        sInputFilePath = argv[1]; //input wav file
        sOutputFilePath = sInputFilePath + "Proc.wav"; //output wav file

        sFilterType = argv[2]; //filter type
        sMaxDelayLengthInS = argv[3]; //max delay time
        sDelayTime = argv[4]; //delay time
        sFilterGain = argv[5]; //filter gain
    }

    return filtering(sInputFilePath, sOutputFilePath, sFilterType, sMaxDelayLengthInS, sDelayTime, sFilterGain, 1024);
}

int filtering(std::string sInputFilePath, std::string sOutputFilePath, std::string sFilterType, std::string sMaxDelayLengthInS, std::string sDelayTime, std::string sFilterGain, int numBlocks)
{
    // declare variables
    CAudioFileIf                *phInputAudioFile = 0;          //!< audio file objects
    CAudioFileIf                *phOutputAudioFile = 0;
    CAudioFileIf::FileSpec_t    stFileSpec;                     //!< file specs

    CCombFilterIf               *combFilter = 0;                //!< comb filter
    CCombFilterIf::CombFilterType_t stFilterType;               //!< filter specs
    float                       fDelayTime = 0;
    float                       fMaxDelayLengthInS = 0;
    float                       fFilterGain = 0;

    float                       **ppfInputAudioData = 0;        //!< audio data
    float                       **ppfOutputAudioData = 0;

    static const int            kBlockSize = numBlocks;

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
    // create the comb filter
    combFilter->create(combFilter); //allocate memory and create instance

    if (sFilterType == "FIR") //determine desired filter type
    {
        stFilterType = CCombFilterIf::CombFilterType_t::kCombFIR;
    }
    else if (sFilterType == "IIR")
    {
        stFilterType = CCombFilterIf::CombFilterType_t::kCombIIR;
    }
    else
    {
        cout << "Invalid filter type! Enter FIR or IIR " << endl;
        return -1;
    }

    fDelayTime = std::stof(sDelayTime); //convert entered params to float
    fMaxDelayLengthInS = std::stof(sMaxDelayLengthInS);
    fFilterGain = std::stof(sFilterGain);

    if (fMaxDelayLengthInS < fDelayTime) //check for potential delay buffer issues
    {
        cout << "Init error: <max delay in sec> must be longer than <delay time in sec>" << endl;
        return -1;
    }

    if ((stFilterType == CCombFilterIf::CombFilterType_t::kCombIIR) && (fFilterGain == 1) && (fDelayTime == 0)) //check for potential divide by zero error in IIR
    {
        cout << "Invalid params for IIR filter" << endl;
        cout << "Delay cannot be 0 while gain is 1" << endl;
        return -1;
    }

    //initialize filter
    combFilter->init(stFilterType, fMaxDelayLengthInS, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    combFilter->setParam(CCombFilterIf::FilterParam_t::kParamDelay, fDelayTime);
    combFilter->setParam(CCombFilterIf::FilterParam_t::kParamGain, fFilterGain);

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
    // get audio data, process it, and write it to the output file
    while (!phInputAudioFile->isEof()) //until end of input file is reached
    {
        long long iNumFrames = kBlockSize; //number of frames to read at a time
        //read in 1024 frames from audio file channels 1 and 2
        //store in ppfInputAudioData[0] and ppfInputAudioData[1] respectively
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        //apply the comb filter and write data to ppfOutputAudioData[0] and ppfOutputAudioData[1] respectively
        combFilter->process(ppfInputAudioData, ppfOutputAudioData, (int)iNumFrames);

        //write processed audio data to the output file
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;


    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    phInputAudioFile->closeFile(); //close files
    phOutputAudioFile->closeFile();
    combFilter->reset(); //reset

    CAudioFileIf::destroy(phInputAudioFile); //delete instances
    CAudioFileIf::destroy(phOutputAudioFile);
    CCombFilterIf::destroy(combFilter);

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
    cout << endl;

    return;
}

//////////////////////////////////////////////////////////////////////////////
/*! test #1
\ FIR: output is zero if input freq matches feedforward
\ Delay = 0, Gain = -1
*/
void    test1()
{
    cout << "Running test 1..." << endl;

    CAudioFileIf* phOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfOutput = 0;

    std::string sInputFilePath = "./samp2_sawtooth.wav";
    std::string sOutputFilePath = "./samp2_sawtoothProc1.wav";

    //create a processed output file with FIR comb and 1024 block size
    filtering(sInputFilePath, sOutputFilePath, "FIR", "0.1", "0", "-1", 1024);

    //open and check output
    phOutput->create(phOutput);
    phOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phOutput->isOpen()) //check
    {
        cout << "Test 1 - Input file open error!";
        return;
    }
    phOutput->getFileSpec(stFileSpec); //get file specs

    ppfOutput = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfOutput[i] = new float[1024]; //allocate array of 1024 (arbitrary)
    }

    while (!phOutput->isEof()) //until end of input file is reached
    {
        long long iNumFrames = 1024; //number of frames to read at a time
        //read in 1024 frames
        //store in ppfOutput
        phOutput->readData(ppfOutput, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if (ppfOutput[c][i] != 0) //check if output is zero
                {
                    cout << "Test 1 - FAILED" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 1 - PASSED" << endl;

    //clean up
    phOutput->closeFile();
    CAudioFileIf::destroy(phOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfOutput[i]; //free memory associated with that channel
    }

    delete[] ppfOutput; //delete pointer arrays

    ppfOutput = 0; //reset to NULL 

    return;

}

//////////////////////////////////////////////////////////////////////////////
/*! test #2
\ IIR: input magnitude is scaled by a factor of 1/(1-g) if input freq matches feedback
*/
void    test2()
{
    cout << "Running test 2..." << endl;

    CAudioFileIf* phInput = 0;
    CAudioFileIf* phOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfInput = 0;
    float** ppfOutput = 0;

    std::string sInputFilePath = "./samp2_sawtooth.wav";
    std::string sOutputFilePath = "./samp2_sawtoothProc2.wav";

    //create a processed output file with IIR comb and 1024 block size
    filtering(sInputFilePath, sOutputFilePath, "IIR", "0.1", "0", "-1", 1024);

    //open and both files
    phInput->create(phInput);
    phOutput->create(phOutput);

    phInput->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    phOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    if (!phInput->isOpen() || !phOutput->isOpen()) //check
    {
        cout << "Test 2 - File open error!";
        return;
    }
    phInput->getFileSpec(stFileSpec); //get file specs

    //allocate memory
    ppfInput = new float* [stFileSpec.iNumChannels];
    ppfOutput = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfInput[i] = new float[1024]; //allocate array of 1024 (arbitrary)
        ppfOutput[i] = new float[1024]; 
    }

    //check that output is just the input scaled by a factor of 1/(1-g) -> 1/2
    while (!phInput->isEof() || !phOutput->isEof()) //until end of file is reached
    {
        long long iNumFrames = 1024; //number of frames to read at a time
        //read in 1024 frames
        //store in ppfInput/Output
        phInput->readData(ppfInput, iNumFrames);
        phOutput->readData(ppfOutput, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if ((ppfOutput[c][i] - (0.5 * ppfInput[c][i])) > 0.001 || (ppfOutput[c][i] - (0.5 * ppfInput[c][i])) < -0.001) //check if output is scaled properly - account for error b/c write function is only 16 bit
                {
                    cout << "Test 2 - FAILED" << endl;
                    cout << "Output " << ppfOutput[c][i] << " Input " << ppfInput[c][i] << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 2 - PASSED" << endl;

    //clean up
    phInput->closeFile();
    phOutput->closeFile();
    CAudioFileIf::destroy(phInput);
    CAudioFileIf::destroy(phOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfInput[i]; //free memory associated with that channel
        delete[] ppfOutput[i]; 
    }

    delete[] ppfInput; //delete pointer arrays
    delete[] ppfOutput; 

    ppfInput = 0; //reset to NULL 
    ppfOutput = 0; 

    return;
}

//////////////////////////////////////////////////////////////////////////////
/*! test #3
\ FIR/IIR: correct output result for VARYING input block size
*/
void     test3(std::string sFilterType)
{
    cout << "Running test 3..." << endl;
    CAudioFileIf* phProc1 = 0;
    CAudioFileIf* phProc2 = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfProc1 = 0;
    float** ppfProc2 = 0;

    std::string sInputFilePath = "./samp2_sawtooth.wav";
    std::string sOutputFilePath1 = "./samp2_sawtoothProc3_1.wav";
    std::string sOutputFilePath2 = "./samp2_sawtoothProc3_2.wav";
    
    //create a processed output file with sFilterType comb and 1024 block size
    filtering(sInputFilePath, sOutputFilePath1, sFilterType, "0.02", "0.01", "0.5", 1024);

    //process the same file using 512 block size and write to different output file
    filtering(sInputFilePath, sOutputFilePath2, sFilterType, "0.02", "0.01", "0.5", 512);

    //read and compare both files
    phProc1->create(phProc1);
    phProc2->create(phProc2);

    phProc1->openFile(sOutputFilePath1, CAudioFileIf::kFileRead); //open input file for reading
    if (!phProc1->isOpen()) { //check
        cout << "Test 3 - Input file open error!";
        return;
    }

    phProc2->openFile(sOutputFilePath2, CAudioFileIf::kFileRead); //open input file for reading
    if (!phProc2->isOpen()) { //check
        cout << "Test 3 - Input file open error!";
        return;
    }

    phProc1->getFileSpec(stFileSpec); //get file specs

    ppfProc1 = new float* [stFileSpec.iNumChannels];
    ppfProc2 = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfProc1[i] = new float[1024]; //allocate array of 1024 (arbitrary)
        ppfProc2[i] = new float[1024];
    }

    while ((!phProc1->isEof()) || (!phProc2->isEof())) //until end of input file is reached
    {
        long long iNumFrames = 1024; //number of frames to read at a time
        //read in 1024 frames
        //store in ppfProc1/2
        phProc1->readData(ppfProc1, iNumFrames);
        phProc2->readData(ppfProc2, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if (ppfProc1[c][i] != ppfProc2[c][i])
                {
                    cout << "Test 3 - FAILED for " << sFilterType << " filter" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 3 - PASSED for " << sFilterType << " filter" << endl;

    //clean up
    phProc1->closeFile();
    phProc2->closeFile();

    CAudioFileIf::destroy(phProc1);
    CAudioFileIf::destroy(phProc2);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfProc1[i]; //free memory associated with that channel
        delete[] ppfProc2[i];
    }

    delete[] ppfProc1; //delete pointer arrays
    delete[] ppfProc2;

    ppfProc1 = 0; //reset to null pointer
    ppfProc2 = 0;

    return;
}

//////////////////////////////////////////////////////////////////////////////
/*! test #4
\ FIR/IIR: correct processing for zero input signal
\ expected: zero input -> zero output
*/
void     test4(std::string sFilterType)
{
    cout << "Running test 4..." << endl;
    CAudioFileIf* phOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfOutput = 0;

    std::string sInputFilePath = "./samp3_silence.wav";
    std::string sOutputFilePath = "./samp3_silenceProc4.wav";

    //create a processed output file with sFilterType comb and 1024 block size
    filtering(sInputFilePath, sOutputFilePath, sFilterType, "0.02", "0.01", "0.5", 1024);

    //open and check output
    phOutput->create(phOutput);

    phOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead); //open input file for reading
    if (!phOutput->isOpen()) { //check
        cout << "Test 4 - Input file open error!";
        return;
    }

    phOutput->getFileSpec(stFileSpec); //get file specs

    ppfOutput = new float* [stFileSpec.iNumChannels];

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfOutput[i] = new float[1024]; //allocate array of 1024 (arbitrary)
    }

    while (!phOutput->isEof()) //until end of input file is reached
    {
        long long iNumFrames = 1024; //number of frames to read at a time
        //read in 1024 frames
        //store in ppfOutput
        phOutput->readData(ppfOutput, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if ((ppfOutput[c][i] - 0) > 0.001) //account for artifacts in the "silence" file
                {
                    cout << "Test 4 - FAILED for " << sFilterType << " filter" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 4 - PASSED for " << sFilterType << " filter" << endl;

    //clean up
    phOutput->closeFile();

    CAudioFileIf::destroy(phOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfOutput[i]; //free memory associated with that channel
    }

    delete[] ppfOutput; //delete pointer arrays

    ppfOutput = 0; //reset to null pointer

    return;
}

//////////////////////////////////////////////////////////////////////////////
/*! test #5
\ FIR/IIR: correct processing for zero delay time, zero gain
\ expected: input = output
*/

void test5(std::string sFilterType)
{
    cout << "Running test 5..." << endl;
    CAudioFileIf* phInput = 0;
    CAudioFileIf* phOutput = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    float** ppfInput = 0;
    float** ppfOutput = 0;

    std::string sInputFilePath = "./samp2_sawtooth.wav";
    std::string sOutputFilePath = "./samp2_sawtoothProc5.wav";

    //create a processed output file with sFilterType comb and 1024 block size
    filtering(sInputFilePath, sOutputFilePath, sFilterType, "0.01", "0", "0", 1024);

    //open and compare input/output
    phInput->create(phInput);
    phOutput->create(phOutput);

    phInput->openFile(sInputFilePath, CAudioFileIf::kFileRead); //open input file for reading
    if (!phInput->isOpen()) { //check
        cout << "Test 5 - Input file open error!";
        return;
    }

    phOutput->openFile(sOutputFilePath, CAudioFileIf::kFileRead); //open input file for reading
    if (!phOutput->isOpen()) { //check
        cout << "Test 5 - Input file open error!";
        return;
    }

    phInput->getFileSpec(stFileSpec); //get file specs

    ppfInput = new float* [stFileSpec.iNumChannels];
    ppfOutput = new float* [stFileSpec.iNumChannels];

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each channel of audio in the file
    {
        ppfInput[i] = new float[1024]; //allocate array of 1024 (arbitrary)
        ppfOutput[i] = new float[1024]; //allocate array of 1024 (arbitrary)
    }

    while ((!phInput->isEof()) || (!phOutput->isEof())) //until end of input file is reached
    {
        long long iNumFrames = 1024; //number of frames to read at a time
        //read in 1024 frames
        //store in ppf
        phInput->readData(ppfInput, iNumFrames);
        phOutput->readData(ppfOutput, iNumFrames);

        for (int c = 0; c < stFileSpec.iNumChannels; c++) //compare the two arrays
        {
            for (int i = 0; i < iNumFrames; i++)
            {
                if (ppfInput[c][i] != ppfOutput[c][i]) //account for artifacts in the "silence" file
                {
                    cout << "Test 5 - FAILED for " << sFilterType << " filter" << endl;
                    return;
                }
            }
        }
    }
    cout << "Test 5 - PASSED for " << sFilterType << " filter" << endl;

    //clean up
    phInput->closeFile();
    phOutput->closeFile();

    CAudioFileIf::destroy(phInput);
    CAudioFileIf::destroy(phOutput);

    for (int i = 0; i < stFileSpec.iNumChannels; i++) //for each audio channel array
    {
        delete[] ppfInput[i]; //free memory associated with that channel
        delete[] ppfOutput[i];
    }

    delete[] ppfInput; //delete pointer arrays
    delete[] ppfOutput;

    ppfInput = 0; //reset to null pointer
    ppfOutput = 0;

    return;
}