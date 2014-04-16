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
#include "kiss_fft.h"

class ADRess
{
public:
    ADRess (int blockSize, int beta);
    ~ADRess ();
    void setParameter (int index, int newValue);
    void processBlock (float* leftChannelData, float* rightChannelData);
    
    enum Parameters
    {
        d = 0,
        H,
        LR
    };
    
    enum kLeftRightChannel_t
    {
        kLeftChannel = 0,
        kRightChannel
    };
    
private:
    int blockSize_;
    int beta_;
    int d_;
    int H_;
    float* windowBuffer_;
    int LR_;
};

#endif /* defined(__StereoSourceSeparation__ADRess__) */
