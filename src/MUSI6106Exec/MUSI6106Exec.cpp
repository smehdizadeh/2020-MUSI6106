
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"

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

    CAudioFileIf*phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    //sInputFilePath = "C:/\Users/\sophi/\Documents/\GATECH/\MUSI6106/\sweep.wav";
    //sOutputFilePath = "C:/\Users/\sophi/\Documents/\GATECH/\MUSI6106/\sweep.txt";

    sInputFilePath = argv[1];  //input file path should be second argument after .exe
    sOutputFilePath = argv[2]; //output file path should be second argument after .exe

    //cout << "Input file is " << sInputFilePath << endl;
    //cout << "Output file is " << sOutputFilePath << endl;
 

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    phAudioFile->create(phAudioFile); //allocate memory and create instance
    //define file specs
    phAudioFile->getFileSpec(stFileSpec);
    /*stFileSpec.eFormat = CAudioFileIf::FileFormat_t::kFileFormatWav; //WAVE FILE
    stFileSpec.eBitStreamType = CAudioFileIf::BitStream_t::kFileBitStreamUnknown; //do we know this?
    stFileSpec.iNumChannels = 2; //2 CHANNELS
    stFileSpec.fSampleRateInHz = 44100.0; //44.1 KHZ SAMPLE RATE*/

    phAudioFile->openFile(sInputFilePath, CAudioFileIf::FileIoType_t::kFileRead, &stFileSpec); //open input file for reading

    // CHECK
    /*
    if (!phAudioFile->isInitialized()) {
        cout << "input file not initialized" << endl; //verify instance is initialized
    }
    if (!phAudioFile->isOpen()) {
        cout << "input file not open" << endl; //verify file is open
    }
    else {
        cout << "input file initialized and open" << endl;
    }
    cout << endl;
    */
    
     
    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath, std::fstream::out); //open output file for writing

    // CHECK
    /*
    if (!hOutputFile.is_open()) {
        cout << "output file not open" << endl;
    }
    else {
        cout << "output file open" << endl;
    }
    cout << endl;
    */

 
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float* [2]; //ppfAudioData points to two arrays (audio channel 1, audio channel 2)
    long long iLengthInFrames; //initialize file length in frames
    phAudioFile->getLength(iLengthInFrames); //get total file length in frames

    ppfAudioData[0] = new float[kBlockSize]; //array for audio channel 1, length iNumFrames (1024)
    ppfAudioData[1] = new float[kBlockSize]; //array for audio channel 2, length iNumFrames (1024)
 

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    long long iFrame; //current frame position
    phAudioFile->getPosition(iFrame); //get current frame (should start at zero)

    long long iNumFrames = kBlockSize; //number of frames to read at a time
    hOutputFile.precision(15); //keep precision of audio data in output file

    //start reading and writing blocks of audio data
    //while current frame position is less than total frames in file
    while (iFrame < iLengthInFrames) {
        //read in 1024 frames from audio file channels 1 and 2
        //store in ppfAudioData[0] and ppfAudioData[1] respectively
        phAudioFile->readData(ppfAudioData, iNumFrames);
        
        //go through both ppfAudioData arrays and write to output file in two colomns
        for (int i = 0; i < iNumFrames; i++) {
            hOutputFile << ppfAudioData[0][i] << " " << ppfAudioData[1][i] << endl;
        }
        
        phAudioFile->getPosition(iFrame); //check current frame position (should increment by 1024)
    }


    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    hOutputFile.close(); //close the output file
    phAudioFile->closeFile(); //close the input file
    delete[] ppfAudioData; //delete audio data array
    phAudioFile->destroy(phAudioFile); //destroy instance and free memory

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2020 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

