//
//  ADRess.h
//  StereoSourceSeparation
//
//  Created by Xinyuan Lai on 4/8/14.
//
//

#ifndef __StereoSourceSeparation__ADRess__
#define __StereoSourceSeparation__ADRess__

#include <iostream>
#include "kiss_fftr.h"
#include <cmath>
#include <complex>

using std::complex;

class ADRess
{
public:
    ADRess (int blockSize, int beta);
    ~ADRess ();
    
    enum Status_t
    {
        kBypass,
        kSolo,
        kMute
    };
    
    void setStatus(Status_t newStatus);
    void setDirection(int newDirection);
    void setWidth(int newWidth);
    
    const Status_t getStatus();
    const int getDirection();
    const int getWidth();
    
    void process (float* leftData, float* rightData);
    
private:
    const int BLOCK_SIZE;
    const int BETA;
    Status_t currStatus_;
    int d_;
    int H_;
    float* windowBuffer_;
    int LR_;   // 0 for left, 1 for right, 2 for centre
    
    kiss_fftr_cfg fwd_;
    kiss_fftr_cfg inv_;
    
    complex<float>* leftSpectrum_;
    complex<float>* rightSpectrum_;
    
    float* leftMag_;
    float* rightMag_;
    float* leftPhase_;
    float* rightPhase_;
    
    int*  minIndicesL_;
    float* minValuesL_;
    float* maxValuesL_;
    
    int*  minIndicesR_;
    float* minValuesR_;
    float* maxValuesR_;
    
    float** azimuthL_;
    float** azimuthR_;
    
    float* resynMagL_;
    float* resynMagR_;
    
    void getMinimum(int nthBin, float* nthBinAzm, float* minValues, int* minIndices);
    void getMaximum(int nthBin, float* nthBinAzm, float* maxValues);
    float sumUpPeaks(float* nthBinAzm);
};

#endif /* defined(__StereoSourceSeparation__ADRess__) */
