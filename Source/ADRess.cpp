//
//  ADRess.cpp
//  StereoSourceSeparation
//
//  Created by Xinyuan Lai on 4/8/14.
//
//

#include "ADRess.h"

ADRess::ADRess(int blockSize, int beta)
{
    blockSize_ = blockSize;
    beta_ = beta;
    
    windowBuffer_ = new float[blockSize_];
    for (int i = 0; i < blockSize_; i++) {
        windowBuffer_[i] = 0.5*(1.0 - cos(2.0*M_PI*(float)i/(blockSize_-1)) );
    }
    
}

ADRess::~ADRess()
{
    delete [] windowBuffer_;
}

void ADRess::setParameter(int index, int newValue)
{
    
}