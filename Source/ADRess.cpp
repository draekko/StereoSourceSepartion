//
//  ADRess.cpp
//  StereoSourceSeparation
//
//  Created by Xinyuan Lai on 4/8/14.
//
//

#include "ADRess.h"

#define SCALE_DOWN_FACTOR 2

ADRess::ADRess(int blockSize, int beta):BLOCK_SIZE(blockSize),BETA(beta)
{
    currStatus_ = kBypass;
    d_ = 50;
    H_ = 25;
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
    
    resynMag_ = new float[BLOCK_SIZE/2+1];
    
    minIndices_ = new int[BLOCK_SIZE/2+1];
    minValues_ = new float[BLOCK_SIZE/2+1];
    maxValues_ = new float[BLOCK_SIZE/2+1];
    
    azimuth_ = new float*[BLOCK_SIZE/2+1];
    for (int n = 0; n<BLOCK_SIZE/2+1; n++)
        azimuth_[n] = new float[BETA+1];
    
}

ADRess::~ADRess()
{
    delete [] resynMag_;
    delete [] windowBuffer_;
    delete [] leftSpectrum_;
    delete [] rightSpectrum_;
    delete [] leftMag_;
    delete [] rightMag_;
    delete [] leftPhase_;
    delete [] rightPhase_;
    delete [] minIndices_;
    delete [] minValues_;
    delete [] maxValues_;
    
    for (int n = 0; n<BLOCK_SIZE/2+1; n++)
        delete [] azimuth_[n];
    delete [] azimuth_;
}

void ADRess::setStatus(Status_t newStatus)
{
    currStatus_ = newStatus;
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

const ADRess::Status_t ADRess::getStatus()
{
    return currStatus_;
}

const int ADRess::getDirection()
{
    if (LR_) {
        return BETA - d_;
    } else {
        return d_;
    }
}

const int ADRess::getWidth()
{
    return H_;
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
        
        
        // generate right or left azimuth
        if (LR_) { // when right channel dominates
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuth_[n][g] = std::abs(leftSpectrum_[n] - rightSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
            
        } else {   // when left channel dominates
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuth_[n][g] = std::abs(rightSpectrum_[n] - leftSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
        }
        
        // find azimuth minima and turn them to peaks
        for (int n = 0; n<BLOCK_SIZE/2+1; n++) {
            getMinimum(n, azimuth_[n]);
            getMaximum(n, azimuth_[n]);
            
            for (int g = 0; g<=BETA; g++)
                azimuth_[n][g] = 0;
            
            azimuth_[n][minIndices_[n]] = maxValues_[n] - minValues_[n];
            
            resynMag_[n] = sumUpPeaks(azimuth_[n]);
        }
        
        
        
        if (LR_) {
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                rightSpectrum_[i] = std::polar(resynMag_[i], rightPhase_[i]);
            
            kiss_fftri(inv_, (kiss_fft_cpx*)rightSpectrum_, (kiss_fft_scalar*)rightData);
            memcpy(leftData, rightData, BLOCK_SIZE*sizeof(float));
            
        } else {
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                leftSpectrum_[i] = std::polar(resynMag_[i], leftPhase_[i]);
            
            kiss_fftri(inv_, (kiss_fft_cpx*)leftSpectrum_, (kiss_fft_scalar*)leftData);
            memcpy(rightData, leftData, BLOCK_SIZE*sizeof(float));
        }
        
        
        // scale down ifft results and windowing
        for (int i = 0; i<BLOCK_SIZE; i++) {
            leftData[i] = leftData[i]*windowBuffer_[i]/BLOCK_SIZE/SCALE_DOWN_FACTOR;
            rightData[i] = rightData[i]*windowBuffer_[i]/BLOCK_SIZE/SCALE_DOWN_FACTOR;
        }
        
    }
    
    // when by-pass, compensate for the gain coming from 1/4 hopsize
    else {
        for (int i = 0; i<BLOCK_SIZE; i++) {
            leftData[i] /= SCALE_DOWN_FACTOR;
            rightData[i] /= SCALE_DOWN_FACTOR;
        }
    }
}

void ADRess::getMinimum(int nthBin, float *nthBinAzm)
{
    int minIndex = 0;
    float minValue = nthBinAzm[0];
    
    for (int i = 1; i<=BETA; i++) {
        if (nthBinAzm[i] < minValue) {
            minIndex = i;
            minValue = nthBinAzm[i];
        }
    }
    
    minIndices_[nthBin] = minIndex;
    minValues_[nthBin] = minValue;
}

void ADRess::getMaximum(int nthBin, float *nthBinAzm)
{
    float maxValue = nthBinAzm[0];
    
    for (int i = 1; i<=BETA; i++)
        if (nthBinAzm[i]>maxValue)
            maxValue = nthBinAzm[i];
    
    maxValues_[nthBin] = maxValue;
}

float ADRess::sumUpPeaks(float *nthBinAzm)
{
    float sum = 0;
    
    int startInd = std::max(0, d_-H_/2);
    int endInd = std::min(BETA, d_+H_/2);
    
    switch (currStatus_) {
        case kSolo:
            for (int i = startInd; i<=endInd; i++)
                sum += nthBinAzm[i];
            break;
            
        case kMute:
            for (int i = 0; i<=BETA; i++)
                if (i<startInd || i>endInd)
                    sum += nthBinAzm[i];
            break;
            
        case kBypass:
        default:
            break;
    }
    
    return sum;
}