#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxBlur.h"
using namespace ofxCv;
using namespace cv;

#include "ofxFaceTrackerThreaded.h"
#include "facePartRecorder.h"



class facePolyline;

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
        
    
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ofVec2f position;
	float scale;
	ofVec3f orientation;
	ofMatrix4x4 rotationMatrix;
	
	Mat translation, rotation;
	ofMatrix4x4 pose;
	
    ofVideoPlayer face;
    ofxFaceTracker copy;
    ofEasyCam easyCam;
    
    ofxBlur blurMain;
    ofFbo prevFrame;
    
    facePartRecorder  FPR;
    
    
    
};
