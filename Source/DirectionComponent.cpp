//
//  DirectionComponent.cpp
//  StereoSourceSeparation
//
//  Created by annie zhang on 4/14/14.
//
//

#include "DirectionComponent.h"

DirectionComponent::DirectionComponent(int width, int height)
{
    dirAngle = M_PI/2;
    widAngle = M_PI/8;
    width_ = width;
    height_ = height;
    radius = width_/2-5;
    arrowLine = Line<float>(width_/2, height_, width_/2+radius*cos(dirAngle), height_-radius*sin(dirAngle));
    
}

DirectionComponent::~DirectionComponent()
{
}

//==============================================================================
void DirectionComponent::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    g.setColour (Colours::grey);
    g.fillPath (internalPath);
    g.strokePath (internalPath, PathStrokeType (1.200f));
    g.setColour(Colours::darkgrey);
    g.fillPath (arrowPath);
    g.strokePath (arrowPath, PathStrokeType (1.200f));
    g.setColour (Colours::wheat);
    g.drawEllipse(5, 5, width_-10, 2*height_-10, 5);
}

void DirectionComponent::resized()
{
    internalPath.clear();
    internalPath.startNewSubPath (width_/2, height_);
    internalPath.lineTo (width_/2+radius*cos(dirAngle-widAngle), height_-radius*sin(dirAngle-widAngle));
    internalPath.quadraticTo(width_/2+radius*cos(dirAngle), -20, width_/2+radius*cos(dirAngle+widAngle), height_-radius*sin(dirAngle+widAngle));
    internalPath.lineTo (width_/2, height_);
    internalPath.closeSubPath();
    arrowPath.clear();
    arrowPath.startNewSubPath (width_/2, height_);
    arrowPath.addArrow(arrowLine, 5, 10, 10);
    arrowPath.closeSubPath();
}

void DirectionComponent::mouseDown(const juce::MouseEvent &e)
{
    dirAngle = atanf(e.getPosition().y*1.0/e.getPosition().x);
    this->resized();
}

void DirectionComponent::setValue(float mVal)
{
    if (mVal >= 0 && mVal <= M_PI)
        dirAngle = mVal;
}