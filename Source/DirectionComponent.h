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

class DirectionComponent :  public Component,
                            public MouseListener
{
public:
    DirectionComponent(int width, int height);
    ~DirectionComponent();
    
    void paint (Graphics& g);
    void resized();
    void mouseDown (const MouseEvent& event);
    void setValue(float mVal);
    
private:
    int width_;
    int height_;
    float dirAngle;
    float widAngle;
    float radius;
    
    Path internalPath;
    Path arrowPath;
    Line<float> arrowLine;
    
};


#endif /* defined(__StereoSourceSeparation__DirectionComponent__) */
