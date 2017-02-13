#pragma once
#include "ofMain.h"

#define STRINGIFY(...) STRINGIFY_AUX(__VA_ARGS__)
#define STRINGIFY_AUX(...) #__VA_ARGS__

class ofxPointilize
{
public:
    enum ScaleMode {
        /// \brief complete image is shown
        FIT,
        /// \brief image fills whole texture, image is cut off
        FILL,
        /// \brief image fills whole texture, image is streched
        STRECHED
    };
    
    enum RenderType {
        QUADRATIC,
        CIRCLES
    };
    
    enum DynamicSizeMode {
        ALL_EQUAL,
        DARK_BIG,
        BRIGHT_BIG,
        CLOSE_BIG,
        FAR_BIG
    };
    
    void setup(int w = 512, int h = 512, bool useDepth = false);
    void update();
    void draw(int x, int y);
    
    void loadTexture(ofTexture texture);
    void loadDepthTexture(ofTexture texture);
    
    ofTexture& getTexture();
    
    ofxPointilize::RenderType getRenderType();
    void setRenderType(RenderType renderType);
    void setRenderType(int renderType);
    
    float getBorderSize();
    void setBorderSize(float size);
    
    ofxPointilize::DynamicSizeMode getDynamicSizeMode();
    void setDynamicSizeMode(DynamicSizeMode mode);
    void setDynamicSizeMode(int mode);
    
    ofxPointilize::ScaleMode getScaleMode();
    void setScaleMode(ScaleMode mode);
    void setScaleMode(int mode);
    
    float getRadius();
    void setRadius(float radius);
    
private:
    string getVertexShader();
    string getFragmentShader();
    
    static ofVec2f resize(ofVec2f src, ofVec2f dst, ScaleMode mode = FILL);
    static ofVec2f centerObjects(ofVec2f src, ofVec2f dst);
    
    ofShader shader;
    
    bool useDepth;
    int width, height;
    ofTexture colorInTexture, depthInTexture;
    
    RenderType renderType;
    float borderSize;
    DynamicSizeMode dynamicSize;
    ScaleMode scaleMode;
    float radius;
    
    bool useFboColor, useFboDepth;
    ofFbo fboColor, fboDepth;
    ofTexture textureColor, textureDepth;
    ofFbo outputFbo;
    
};

