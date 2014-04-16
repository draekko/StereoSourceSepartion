//
//  DirectionComponent.cpp
//  StereoSourceSeparation
//
//  Created by annie zhang on 4/14/14.
//
//

#include "DirectionComponent.h"

DirectionComponent::DirectionComponent()
{
    dirAngle = M_PI/2;
}

DirectionComponent::~DirectionComponent()
{
}

//==============================================================================
void DirectionComponent::paint (Graphics& g)
{
    radius = (int)getWidth()*2/5;
    g.fillAll(Colours::darkgrey);
    g.setColour (Colours::wheat);
    g.drawEllipse((int)getWidth()/2-radius, (int)getHeight()-radius, 2*radius, 2*radius, 5);
    g.drawLine((int)getWidth()/2, getHeight(), (int)getWidth()/2+radius*cos(dirAngle), getHeight()-radius*sin(dirAngle), 3);
}


void DirectionComponent::setValue(float mVal)
{
    if (mVal >= 0 && mVal <= M_PI)
        dirAngle = mVal;
}