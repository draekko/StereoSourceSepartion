//
//  DirectionComponent.h
//  StereoSourceSeparation
//
//  Created by annie zhang on 4/14/14.
//
//

#ifndef __StereoSourceSeparation__DirectionComponent__
#define __StereoSourceSeparation__DirectionComponent__

#include <iostream>
#include <math.h>
#include "JuceHeader.h"

#define _USE_MATH_DEFINES

class DirectionComponent :  public Component
{
public:
    DirectionComponent();
    ~DirectionComponent();
    
    void paint (Graphics& g);
    void setValue(float mVal);
    
private:
    float dirAngle;
    int radius;
};


#endif /* defined(__StereoSourceSeparation__DirectionComponent__) */
