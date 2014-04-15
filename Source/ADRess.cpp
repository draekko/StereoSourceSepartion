//
//  ADRess.cpp
//  StereoSourceSeparation
//
//  Created by Xinyuan Lai on 4/8/14.
//
//

#include "ADRess.h"

ADRess::ADRess(int blockSize, int beta):BLOCK_SIZE(blockSize),BETA(beta)
{
    currStatus_ = kSolo;
    d_ = 50;
    H_ = 0;
    LR_ = 0;
    
    // Hann window
    windowBuffer_ = new float[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++) {
        windowBuffer_[i] = 0.5*(1.0 - cos(2.0*M_PI*(float)i/(BLOCK_SIZE-1)) );
    }
    
    // initialise FFt
    fwd_= kiss_fftr_alloc(BLOCK_SIZE,0,NULL,NULL);
    inv_= kiss_fftr_alloc(BLOCK_SIZE,1,NULL,NULL);
    
    leftSpectrum_ = new complex<float>[BLOCK_SIZE];
    rightSpectrum_ = new complex<float>[BLOCK_SIZE];
    
    leftMag_ = new float[BLOCK_SIZE/2+1];
    rightMag_ = new float[BLOCK_SIZE/2+1];
    leftPhase_ = new float[BLOCK_SIZE/2+1];
    rightPhase_ = new float[BLOCK_SIZE/2+1];
}

ADRess::~ADRess()
{
    delete [] windowBuffer_;
    delete [] leftSpectrum_;
    delete [] rightSpectrum_;
    delete [] leftMag_;
    delete [] rightMag_;
    delete [] leftPhase_;
    delete [] rightPhase_;
}

void ADRess::setDirection(int newDirection)
{
    if (newDirection <= BETA/2) {
        d_  = newDirection;
        LR_ = 0;
    }
    else {
        d_  = BETA - newDirection;
        LR_ = 1;
    }

}

void ADRess::setWidth(int newWidth)
{
    H_ = newWidth;
}


void ADRess::process(float *leftData, float *rightData)
{
    // add window
    for (int i = 0; i<BLOCK_SIZE; i++) {
        leftData[i]  *= windowBuffer_[i];
        rightData[i] *= windowBuffer_[i];
    }
    
    if (currStatus_ != kBypass) {
        
        // do fft
        kiss_fftr(fwd_, (kiss_fft_scalar*)leftData,  (kiss_fft_cpx*)leftSpectrum_);
        kiss_fftr(fwd_, (kiss_fft_scalar*)rightData, (kiss_fft_cpx*)rightSpectrum_);
        
        // convert complex to magnitude-phase representation
        for (int i = 0; i<BLOCK_SIZE/2+1; i++) {
            leftMag_[i] = std::abs(leftSpectrum_[i]);
            rightMag_[i] = std::abs(rightSpectrum_[i]);
            leftPhase_[i] = std::arg(leftSpectrum_[i]);
            rightPhase_[i] = std::arg(rightSpectrum_[i]);
        }
        
        // convert complex to real-imaginary representation
        for (int i = 0; i<BLOCK_SIZE/2+1; i++) {
            leftSpectrum_[i] = std::polar(leftMag_[i], leftPhase_[i]);
            rightSpectrum_[i] = std::polar(rightMag_[i], rightPhase_[i]);
        }
        
        // do inverse fft
        kiss_fftri(inv_, (kiss_fft_cpx*)leftSpectrum_, (kiss_fft_scalar*)leftData);
        kiss_fftri(inv_, (kiss_fft_cpx*)rightSpectrum_, (kiss_fft_scalar*)rightData);
        
        // scale down ifft results
        for (int i = 0; i<BLOCK_SIZE; i++) {
            leftData[i] /= BLOCK_SIZE;
            rightData[i] /= BLOCK_SIZE;
        }
        
    }
}