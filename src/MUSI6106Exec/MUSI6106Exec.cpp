
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"

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

    float                   **ppfAudioInputData = 0;
    float                   **ppfAudioOutputData = 0;


    CAudioFileIf            *phAudioInputFile = 0;
    CAudioFileIf            *phAudioOutputFile = 0;

    CAudioFileIf::FileSpec_t stFileSpec;

    CVibrato                *phVibrato = 0;
    float                   fModWidthInSec = 0;
    float                   fModFreqInHz = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 4)
    {
        cout << "Missing argument!" << endl;
        cout << "Please enter <input file path> <vibrato mod width in sec> <vibrato mod freq in Hz>" << endl;
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = sInputFilePath + "VibProc.wav";

        fModWidthInSec = std::stof(argv[2]);
        fModFreqInHz = std::stof(argv[3]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioInputFile);
    phAudioInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioInputFile->isOpen())
    {
        cout << "Input file open error!";
        return -1;
    }
    phAudioInputFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output wav file
    CAudioFileIf::create(phAudioOutputFile);
    phAudioOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phAudioOutputFile->isOpen())
    {
        cout << "Output file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // create and init vibrato
    phVibrato = new CVibrato();
    Error_t initErr = phVibrato->init(fModWidthInSec, fModFreqInHz, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    if (initErr != kNoError)
    {
        cout << "Vibrato effect initialization error -- invalid parameters";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioInputData = new float*[stFileSpec.iNumChannels];
    ppfAudioOutputData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioInputData[i] = new float[kBlockSize];
        ppfAudioOutputData[i] = new float[kBlockSize];
    }

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    int i = 0;
    while (!phAudioInputFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioInputFile->readData(ppfAudioInputData, iNumFrames);

        cout << "\r" << "reading and writing";
        i = i + iNumFrames;
        phVibrato->process(ppfAudioInputData, ppfAudioOutputData, iNumFrames);

        phAudioOutputFile->writeData(ppfAudioOutputData, iNumFrames);
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    phAudioInputFile->closeFile();
    phAudioOutputFile->closeFile();
    phVibrato->reset();

    CAudioFileIf::destroy(phAudioInputFile);
    CAudioFileIf::destroy(phAudioOutputFile);
    delete phVibrato;

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete[] ppfAudioInputData[i];
        delete[] ppfAudioOutputData[i];
    }
    delete[] ppfAudioInputData;
    delete[] ppfAudioOutputData;
    ppfAudioInputData = 0;
    ppfAudioOutputData = 0;

    return 0;
}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

