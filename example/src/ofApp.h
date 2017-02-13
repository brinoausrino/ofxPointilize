#pragma once

#include "ofMain.h"
#include "ofxPointilize.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
public:
    
    void setup();
    void update();
    void draw();
    
    void onRenderTypeChanged(int& t);
    void onDynamicSizeChanged(int& t);
    void onScaleModeChanged(int& t);
    void onBorderSizeChanged(float& t);
    void onRadiusChanged(float& t);
    
    ofxPointilize pointilize;
    
    ofxPanel panel;
    ofParameter<int> renderType;
    ofParameter<int> dynamicSize;
    ofParameter<float> borderSize;
    ofParameter<float> radius;
    
    ofVideoGrabber vidGrabber;
};

