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
    
    void setDirection(int newDirection);
    void setWidth(int newWidth);
    
    void process (float* leftData, float* rightData);
    
    enum Status
    {
        kDisabled,
        kSolo,
        kMute
    };
    
private:
    const int BLOCK_SIZE;
    const int BETA;
    Status status_;
    int d_;
    int H_;
    float* windowBuffer_;
    int LR_;   // 0 for left, 1 for right
    
    kiss_fftr_cfg fwd_;
    kiss_fftr_cfg inv_;
    
    complex<float>* leftSpectrum_;
    complex<float>* rightSpectrum_;
    
    float* leftMag_;
    float* rightMag_;
    float* leftPhase_;
    float* rightPhase_;
};

#endif /* defined(__StereoSourceSeparation__ADRess__) */
