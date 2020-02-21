
#include <iostream>
#include <fstream>
#include <ctime>
#include <Fft.h>

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

    int                     kBlockSize,
                            kHopSize;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0,
                            *pComplexSpectrum = 0,
                            *pMagSpectrum = 0,
                            *pInputBuffer = 0;

    CAudioFileIf            *phAudioFile = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    CFft                    *pCFft;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments

    sInputFilePath = argv[1];
    sOutputFilePath = sInputFilePath + "output.txt";
    kBlockSize = atoi(argv[2]);
    kHopSize = atoi(argv[3]);

    if(kHopSize > kBlockSize)
        return -1;

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file

    stFileSpec.eFormat = CAudioFileIf::FileFormat_t::kFileFormatWav;
    stFileSpec.eBitStreamType = CAudioFileIf::BitStream_t::kFileBitStreamInt16;
    stFileSpec.iNumChannels = 2;
    stFileSpec.fSampleRateInHz = 44100;
    
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::FileIoType_t::kFileRead, &stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open(sOutputFilePath, std::fstream::out);

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
//    int sizeOfBuffer = sizeof(float) * kBlockSize;
//    int sizeOfSpectrum = sizeof(float) * kBlockSize;

    int sizeOfBuffer = kHopSize;
    int sizeOfSpectrum = kBlockSize;

//    ppfAudioData = (float**) malloc(sizeof(float) * 2);
    ppfAudioData = new float*[2];
    *ppfAudioData = new float [kHopSize];
    *(ppfAudioData+1) = new float [kHopSize];

    pMagSpectrum = new float [sizeOfBuffer+1];
    pComplexSpectrum = new float [sizeOfSpectrum];

    pInputBuffer = new float [kBlockSize]();

    //////////////////////////////////////////////////////////////////////////////
    // Create FFT instance and init

    CFft::createInstance(pCFft);
    pCFft->initInstance(kBlockSize);

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    long long fileData = sizeOfBuffer;

    while(fileData!=0) {
        phAudioFile->readData(ppfAudioData, fileData);

        for (int i = 0; i < fileData; i++) {
            pInputBuffer[kHopSize + i] = ppfAudioData[0][i];
        }

        pCFft->doFft(pComplexSpectrum, pInputBuffer);

        pCFft->getMagnitude(pMagSpectrum, pComplexSpectrum);

        for (int i = 0; i < sizeOfBuffer; i++) {
            pInputBuffer[i] = pInputBuffer[kHopSize + i];
        }

        for(int buf = 0; buf < sizeOfBuffer+1; buf++) {
            hOutputFile << pMagSpectrum[buf] << endl;
            }
        }
    
    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    delete *ppfAudioData;
    delete *(ppfAudioData+1);
    delete ppfAudioData;

    delete[] pInputBuffer;
    delete[] pComplexSpectrum;
    delete[] pMagSpectrum;

    pInputBuffer = 0;
    pComplexSpectrum = 0;
    pMagSpectrum = 0;
    
    CAudioFileIf::destroy(phAudioFile);
    CFft::destroyInstance(pCFft);
    hOutputFile.close();

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