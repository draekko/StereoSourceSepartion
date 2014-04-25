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
    LR_ = 2;
    
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
    
    resynMagL_ = new float[BLOCK_SIZE/2+1];
    for (int i = 0; i<=BLOCK_SIZE/2; i++)
        resynMagL_[i] = 0;
    
    resynMagR_ = new float[BLOCK_SIZE/2+1];
    for (int i = 0; i<=BLOCK_SIZE/2; i++)
        resynMagR_[i] = 0;
    
    minIndicesL_ = new int[BLOCK_SIZE/2+1];
    minValuesL_ = new float[BLOCK_SIZE/2+1];
    maxValuesL_ = new float[BLOCK_SIZE/2+1];
    
    minIndicesR_ = new int[BLOCK_SIZE/2+1];
    minValuesR_ = new float[BLOCK_SIZE/2+1];
    maxValuesR_ = new float[BLOCK_SIZE/2+1];
    
    azimuthL_ = new float*[BLOCK_SIZE/2+1];
    for (int n = 0; n<BLOCK_SIZE/2+1; n++)
        azimuthL_[n] = new float[BETA+1];
    
    azimuthR_ = new float*[BLOCK_SIZE/2+1];
    for (int n = 0; n<BLOCK_SIZE/2+1; n++)
        azimuthR_[n] = new float[BETA+1];
    
}



ADRess::~ADRess()
{
    if (resynMagL_) {
        delete [] resynMagL_;
        resynMagL_ = 0;
    }
    
    if (resynMagR_) {
        delete [] resynMagR_;
        resynMagR_ = 0;
    }
    
    if (windowBuffer_) {
        delete [] windowBuffer_;
        windowBuffer_ = 0;
    }
    
    if (leftSpectrum_) {
        delete [] leftSpectrum_;
        leftSpectrum_ = 0;
    }
    
    if (rightSpectrum_) {
        delete [] rightSpectrum_;
        rightSpectrum_ = 0;
    }
    
    if (leftMag_) {
        delete [] leftMag_;
        leftMag_ = 0;
    }
    
    if (rightMag_) {
        delete [] rightMag_;
        rightMag_ = 0;
    }
    
    if (leftPhase_) {
        delete [] leftPhase_;
        leftPhase_ = 0;
    }
    
    if (rightPhase_) {
        delete [] rightPhase_;
        rightPhase_ = 0;
    }
    
    if (minIndicesL_) {
        delete [] minIndicesL_;
        minIndicesL_ = 0;
    }
    
    if (minValuesL_) {
        delete [] minValuesL_;
        minValuesL_ = 0;
    }
    
    if (maxValuesL_) {
        delete [] maxValuesL_;
        maxValuesL_ = 0;
    }
    
    if (minIndicesR_) {
        delete [] minIndicesR_;
        minIndicesR_ = 0;
    }
    
    if (minValuesR_) {
        delete [] minValuesR_;
        minValuesR_ = 0;
    }
    
    if (maxValuesR_) {
        delete [] maxValuesR_;
        maxValuesR_ = 0;
    }
    
    if (azimuthL_) {
        for (int n = 0; n<BLOCK_SIZE/2+1; n++)
            delete [] azimuthL_[n];
        
        delete [] azimuthL_;
        azimuthL_ = 0;
    }
    
    if (azimuthR_) {
        for (int n = 0; n<BLOCK_SIZE/2+1; n++)
            delete [] azimuthR_[n];
        
        delete [] azimuthR_;
        azimuthR_ = 0;
    }
    
}



void ADRess::setStatus(Status_t newStatus)
{
    currStatus_ = newStatus;
}



void ADRess::setDirection(int newDirection)
{
    if (newDirection == BETA/2) {
        d_ = newDirection;
        LR_ = 2;
    }
    else if (newDirection < BETA/2) {
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
        if (LR_ == 1) { // when right channel dominates
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuthR_[n][g] = std::abs(leftSpectrum_[n] - rightSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
            
            for (int n = 0; n<BLOCK_SIZE/2+1; n++) {
                getMinimum(n, azimuthR_[n], minValuesR_, minIndicesR_);
                getMaximum(n, azimuthR_[n], maxValuesR_);
                
                for (int g = 0; g<=BETA; g++)
                    azimuthR_[n][g] = 0;
                
                if  (currStatus_ == kSolo)
                    // for better rejection of signal from other channel
                    azimuthR_[n][minIndicesR_[n]] = maxValuesR_[n] - minValuesR_[n];
                else
                    azimuthR_[n][minIndicesR_[n]] = maxValuesR_[n];
                
                resynMagR_[n] = sumUpPeaks(azimuthR_[n]);
            }
            
        } else if (LR_ == 0) {   // when left channel dominates
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuthL_[n][g] = std::abs(rightSpectrum_[n] - leftSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
            
            for (int n = 0; n<BLOCK_SIZE/2+1; n++) {
                getMinimum(n, azimuthL_[n], minValuesL_, minIndicesL_);
                getMaximum(n, azimuthL_[n], maxValuesL_);
                
                for (int g = 0; g<=BETA; g++)
                    azimuthL_[n][g] = 0;
                
                if  (currStatus_ == kSolo)
                    // for better rejection of signal from other channel
                    azimuthL_[n][minIndicesL_[n]] = maxValuesL_[n] - minValuesL_[n];
                else
                    azimuthL_[n][minIndicesL_[n]] = maxValuesL_[n];
                
                resynMagL_[n] = sumUpPeaks(azimuthL_[n]);
            }
            
        } else {
            // azimuthR_ for right channel
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuthR_[n][g] = std::abs(leftSpectrum_[n] - rightSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
            // azimuthL_ for left channel
            for (int n = 0; n<BLOCK_SIZE/2+1; n++)
                for (int g = 0; g<=BETA; g++)
                    azimuthL_[n][g] = std::abs(rightSpectrum_[n] - leftSpectrum_[n]*(float)2.0*(float)g/(float)BETA);
            
            for (int n = 0; n<BLOCK_SIZE/2+1; n++) {
                getMinimum(n, azimuthR_[n], minValuesR_, minIndicesR_);
                getMaximum(n, azimuthR_[n], maxValuesR_);
                
                for (int g = 0; g<=BETA; g++)
                    azimuthR_[n][g] = 0;
                
                if  (currStatus_ == kSolo)
                    // for better rejection of signal from other channel
                    azimuthR_[n][minIndicesR_[n]] = maxValuesR_[n] - minValuesR_[n];
                else
                    azimuthR_[n][minIndicesR_[n]] = maxValuesR_[n];
                
                resynMagR_[n] = sumUpPeaks(azimuthR_[n]);
            }
            
            for (int n = 0; n<BLOCK_SIZE/2+1; n++) {
                getMinimum(n, azimuthL_[n], minValuesL_, minIndicesL_);
                getMaximum(n, azimuthL_[n], maxValuesL_);
                
                for (int g = 0; g<=BETA; g++)
                    azimuthL_[n][g] = 0;
                
                if  (currStatus_ == kSolo)
                    // for better rejection of signal from other channel
                    azimuthL_[n][minIndicesL_[n]] = maxValuesL_[n] - minValuesL_[n];
                else
                    azimuthL_[n][minIndicesL_[n]] = maxValuesL_[n];
                
                resynMagL_[n] = sumUpPeaks(azimuthL_[n]);
            }
            
        }
        
        
        if (LR_ == 1) {
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                rightSpectrum_[i] = std::polar(resynMagR_[i], rightPhase_[i]);
            
            kiss_fftri(inv_, (kiss_fft_cpx*)rightSpectrum_, (kiss_fft_scalar*)rightData);
            memcpy(leftData, rightData, BLOCK_SIZE*sizeof(float));
            
        } else if (LR_ == 0){
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                leftSpectrum_[i] = std::polar(resynMagL_[i], leftPhase_[i]);
            
            kiss_fftri(inv_, (kiss_fft_cpx*)leftSpectrum_, (kiss_fft_scalar*)leftData);
            memcpy(rightData, leftData, BLOCK_SIZE*sizeof(float));
        } else {
            
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                rightSpectrum_[i] = std::polar(resynMagR_[i], rightPhase_[i]);
            kiss_fftri(inv_, (kiss_fft_cpx*)rightSpectrum_, (kiss_fft_scalar*)rightData);
            
            for (int i = 0; i<BLOCK_SIZE/2+1; i++)
                leftSpectrum_[i] = std::polar(resynMagL_[i], leftPhase_[i]);
            
            kiss_fftri(inv_, (kiss_fft_cpx*)leftSpectrum_, (kiss_fft_scalar*)leftData);
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




void ADRess::getMinimum(int nthBin, float* nthBinAzm, float* minValues, int* minIndices)
{
    int minIndex = 0;
    float minValue = nthBinAzm[0];
    
    for (int i = 1; i<=BETA; i++) {
        if (nthBinAzm[i] < minValue) {
            minIndex = i;
            minValue = nthBinAzm[i];
        }
    }
    
    minIndices[nthBin] = minIndex;
    minValues[nthBin] = minValue;
}




void ADRess::getMaximum(int nthBin, float* nthBinAzm, float* maxValues)
{
    float maxValue = nthBinAzm[0];
    
    for (int i = 1; i<=BETA; i++)
        if (nthBinAzm[i]>maxValue)
            maxValue = nthBinAzm[i];
    
    maxValues[nthBin] = maxValue;
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
            
            // add smoothing along azimuth
            for (int i = 1; i<4 && startInd-i>=0; i++)
                sum += nthBinAzm[startInd-i]*(4-i)/4;
            for (int i = 1; i<4 && endInd+i<=BETA;i++)
                sum += nthBinAzm[endInd+i]*(4-i)/4;
            
            break;
            
        case kMute:
            for (int i = 0; i<=BETA; i++)
                if (i<startInd || i>endInd)
                    sum += nthBinAzm[i];
            
            // smoothing along azimuth
//            for (int i = 0; i<4; i++)
//                if (startInd+i<endInd-i)
//                    sum += nthBinAzm[startInd+i]*(4-i)/4 + nthBinAzm[endInd-i]*(4-i)/4;
//            break;
            
        case kBypass:
        default:
            break;
    }
    
    return sum;
}