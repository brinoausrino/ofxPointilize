#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //init video stream
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(800, 600);
    
    //init ornament
    pointilize.setup(1024,768);
    
    //assign video input to ornament
    pointilize.loadTexture(vidGrabber.getTexture());
    
    //setup gui
    renderType.set("renderType", 0, 0, 1);
    renderType.addListener(this, &ofApp::onRenderTypeChanged);
    dynamicSize.set("dynamic Size", 0, 0, 2);
    dynamicSize.addListener(this, &ofApp::onDynamicSizeChanged);
    borderSize.set("border size", 0, 0, 1.0);
    borderSize.addListener(this, &ofApp::onBorderSizeChanged);
    radius.set("radius", 0, 0, 1.0);
    radius.addListener(this, &ofApp::onRadiusChanged);
    
    panel.setup();
    panel.add(renderType);
    panel.add(dynamicSize);
    panel.add(borderSize);
    panel.add(radius);
}


//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(0);
    
    //update vidgrabber
    vidGrabber.update();
    
    //update filter
    if(vidGrabber.isFrameNew()){ 
        pointilize.update();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //draw filter
    pointilize.draw(0,0);
    
    //draw gui
    panel.draw();
    
}

void ofApp::onRenderTypeChanged(int& t){
    pointilize.setRenderType(t);
}

void ofApp::onDynamicSizeChanged(int& t){
    pointilize.setDynamicSizeMode(t);
}

void ofApp::onScaleModeChanged(int& t){
    pointilize.setScaleMode(t);
}

void ofApp::onBorderSizeChanged(float& t){
    pointilize.setBorderSize(t);
}

void ofApp::onRadiusChanged(float& t){
    pointilize.setRadius(t);
}
