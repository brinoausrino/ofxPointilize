#include "ofxPointilize.h"



void ofxPointilize::setup(int w, int h, bool _useDepth)
{
    width = w;
    height = h;
    outputFbo.allocate(width, height);
    
    useDepth = _useDepth;
    
    //init standard params
    renderType = QUADRATIC;
    borderSize = 0.0;
    dynamicSize = ALL_EQUAL;
    scaleMode = FILL;
    radius = 0.3;
    
    useFboColor = false;
    useFboDepth = false;
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getVertexShader());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, getFragmentShader());
    shader.linkProgram();
}

void ofxPointilize::update()
{
    //resize input textures
    if (useFboColor)
    {
        ofVec2f size_tex = resize(ofVec2f(colorInTexture.getWidth(),colorInTexture.getHeight()), ofVec2f(width,height));
        ofVec2f pos_tex = centerObjects(size_tex, ofVec2f(width,height));
        
        fboColor.begin();
        ofClear(0, 0, 0, 0);
        colorInTexture.draw(pos_tex.x, pos_tex.y, size_tex.x, size_tex.y);
        fboColor.end();
    }
    
    if (useDepth && useFboDepth) {
        ofVec2f size_tex = resize(ofVec2f(depthInTexture.getWidth(), depthInTexture.getHeight()), ofVec2f(width, height));
        ofVec2f pos_tex = centerObjects(size_tex, ofVec2f(width, height));
        
        fboDepth.begin();
        ofClear(0, 0, 0, 0);
        depthInTexture.draw(pos_tex.x, pos_tex.y, size_tex.x, size_tex.y);
        fboDepth.end();
    }
    
    //create pointilized texture
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    shader.begin();
    if (useFboColor)
        shader.setUniformTexture("tex0", fboColor.getTexture(), 1);
    else
        shader.setUniformTexture("tex0", colorInTexture, 1);
    if (useDepth && useFboDepth)
        shader.setUniformTexture("texDepth", fboDepth, 2);
    else if (useDepth)
        shader.setUniformTexture("texDepth", depthInTexture, 2);
    shader.setUniform2f("textureDim", width ,height);
    shader.setUniform1i("type", renderType);
    shader.setUniform1f("radius", radius);
    shader.setUniform1f("border", borderSize);
    shader.setUniform1i("dynamicSize", dynamicSize);
    if (useFboColor)
        fboColor.draw(0, 0);
    else
        colorInTexture.draw(0, 0);
    shader.end();
    outputFbo.end();
}

void ofxPointilize::draw(int x, int y)
{
    outputFbo.draw(x, y);
}

void ofxPointilize::loadTexture(ofTexture texture)
{
    colorInTexture = texture;
    if (texture.getWidth() != width || texture.getHeight() != height)
    {
        fboColor.allocate(width, height);
        useFboColor = true;
    }
}

void ofxPointilize::loadDepthTexture(ofTexture texture)
{
    if (useDepth) {
        depthInTexture = texture;
        if (texture.getWidth() != width || texture.getHeight() != height)
        {
            fboDepth.allocate(width, height);
            useFboDepth = true;
        }
    }
}

ofTexture & ofxPointilize::getTexture()
{
    return outputFbo.getTexture();
}

ofxPointilize::RenderType ofxPointilize::getRenderType()
{
    return renderType;
}

void ofxPointilize::setRenderType(RenderType renderType_)
{
    renderType = renderType_;
}

void ofxPointilize::setRenderType(int renderType_)
{
    renderType = (RenderType)(renderType_%2);
}

float ofxPointilize::getBorderSize()
{
    return borderSize;
}

void ofxPointilize::setBorderSize(float size)
{
    borderSize = size;
}

ofxPointilize::DynamicSizeMode ofxPointilize::getDynamicSizeMode()
{
    return dynamicSize;
}

void ofxPointilize::setDynamicSizeMode(DynamicSizeMode mode)
{
    dynamicSize = mode;
}

void ofxPointilize::setDynamicSizeMode(int mode)
{
    useDepth ? dynamicSize = (DynamicSizeMode)(mode%5) : dynamicSize = (DynamicSizeMode)(mode%3);
}

ofxPointilize::ScaleMode ofxPointilize::getScaleMode()
{
    return scaleMode;
}

void ofxPointilize::setScaleMode(ScaleMode mode)
{
    scaleMode = mode;
}

void ofxPointilize::setScaleMode(int mode)
{
    scaleMode = (ScaleMode)(mode%3);
}

float ofxPointilize::getRadius()
{
    return radius;
}

void ofxPointilize::setRadius(float radius_)
{
    radius = radius_;
}

string ofxPointilize::getVertexShader()
{
    string out = string("#version 120\n") +
    STRINGIFY(varying vec2 texCoordVarying;
              
              void main()
              {
                  vec2 texcoord = gl_MultiTexCoord0.xy;
                  
                  // here we move the texture coordinates
                  texCoordVarying = vec2(texcoord.x, texcoord.y);
                  
                  // send the vertices to the fragment shader
                  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;;
              }
              );
    return out;
}

string ofxPointilize::getFragmentShader()
{
    string out = string("#version 120\n") +
    STRINGIFY(uniform int type;
              //0 - pixels
              //1 - circles
              
              //object radius
              uniform float radius;
              
              //dynamic Point size
              uniform int dynamicSize;
              //0 - all Pixels equal size
              //1 - brighter points bigger
              //2 - darker Points bigger
              //3 - closest Points bigger
              //4 - farest points bigger
              
              //border
              uniform float border;
              
              uniform vec2 textureDim;
              uniform vec2 textureOut;
              varying vec2 texCoordVarying;
              
              uniform sampler2DRect tex0;
              uniform sampler2DRect texDepth;
              
              float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp)
              {
                  if (abs(inputMin - inputMax) < 0.000000001) {
                      return outputMin;
                  }
                  else {
                      float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
                      
                      if (clamp) {
                          if (outputMax < outputMin) {
                              if (outVal < outputMax)outVal = outputMax;
                              else if (outVal > outputMin)outVal = outputMin;
                          }
                          else {
                              if (outVal > outputMax)outVal = outputMax;
                              else if (outVal < outputMin)outVal = outputMin;
                          }
                      }
                      return outVal;
                  }
              }
              
              void main()
              {
                  //get corresponding pixel
                  vec2 uv = texCoordVarying;
                  float nElems = map(radius, 0.1, 1.0, 5.0, 100.0, true);
                  float d = min(textureDim.x, textureDim.y) / nElems;
                  //vec2 coord = vec2(d*floor(uv.x / d), d*floor(uv.y / d)) +vec2(d*0.5);
                  vec2 center = textureDim *0.5;
                  vec2 dist = uv - center;
                  dist = vec2(floor(dist.x / d), floor(dist.y / d));
                  vec2 coord = center + d*dist + vec2(d*0.5);
                  
                  vec3 color = texture2DRect(tex0, coord).rgb;
                  float alpha = 1.0;
                  
                  float radius = d*0.5;
                  radius *= map(border, 0.0, 1.0, 1.0, 0.2, true);
                  
                  float luminance = 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
                  vec3 depthVal = texture2DRect(texDepth, coord).rgb;
                  float depth = (depthVal.r + depthVal.g + depthVal.b) / 3.0;
                  //depth = map(depth,nearClip,farClip,0.0,1.0,true);
                  
                  if (dynamicSize == 1) {
                      radius *= (1 - luminance);
                  }
                  else if (dynamicSize == 2) {
                      radius *= luminance;
                  }
                  else if (dynamicSize == 3) {
                      radius *= (1 - depth);
                  }
                  else if (dynamicSize == 4) {
                      radius *= depth;
                  }
                  
                  //quadratic
                  if (type == 0) {
                      if (uv.x < coord.x - radius || uv.x > coord.x + radius ||
                          uv.y < coord.y - radius || uv.y > coord.y + radius) {
                          color = vec3(0.0, 0.0, 0.0);
                          alpha = 0.0;
                      }
                  }
                  //circle
                  else if (type == 1) {
                      if (length(uv - coord) > radius) {
                          color = vec3(0.0, 0.0, 0.0);
                          alpha = 0.0;
                      }
                  }
                  
                  gl_FragColor = vec4(color, alpha);
              }
              );
    return out;
}

ofVec2f ofxPointilize::resize(ofVec2f src, ofVec2f dst, ScaleMode mode)
{
    if (mode == STRECHED)
    {
        return dst;
    }
    else
    {
        float ratioDst = dst.x / dst.y;
        float ratioSrc = src.x / src.y;
        
        float w = dst.x;
        float h = w / ratioSrc;
        
        if (mode == FIT)
        {
            if (ratioDst > ratioSrc)
            {
                h = dst.y;
                w = dst.y * ratioSrc;
            }
            else
            {
                w = dst.x;
                h = dst.x / ratioSrc;
            }
            return ofVec2f(w, h);
        }
        else if (mode == FILL)
        {
            
            if (h < dst.y)
            {
                h = dst.y;
                w = h*ratioSrc;
            }
            return ofVec2f(w, h);
        }
    }
}

ofVec2f ofxPointilize::centerObjects(ofVec2f src, ofVec2f dst)
{
    return (dst - src) / 2;
}
