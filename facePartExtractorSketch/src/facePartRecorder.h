#pragma once

#include "ofMain.h"
#include "ofxBlur.h"


class facePolyline;

class facePartRecorder {
public:
	
    
    void setup();
    void update();
    void drawSomePart(facePolyline & eye, ofImage & mask, int who);
    void drawPart(facePolyline & eye, int who);
    
    void drawFaceParts();
    
    void drawFbo();
    
    
    ofShader shader;
    
    
    
    ofImage eyeImg;
    ofImage mouthImg;
    ofImage noseImg;

    ofFbo offscreen[6];
    
    
    
    
    int nFrames;
    bool bRecording;
    ofFbo recording;
    ofFbo recordings[90];
    
    
    ofShader alphaThrough;
    
    int whichRecording;
    
    
    
};
