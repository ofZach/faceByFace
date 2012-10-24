#include "testApp.h"
#include "ofxBlur.h"
#include "facePolyline.h"

using namespace ofxCv;


#define BOXSIZE  100.0


void testApp::setup() {
	
    
    
    ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
    //cam.setDeviceID(0);
	//cam.initGrabber(640, 480);
	
    face.loadMovie("face.mov");
    face.play();
	tracker.setup();
    
    blurMain.setup(640,480, 64);
    prevFrame.allocate(640,480);
    
    FPR.setup();
   
   
    
}

void testApp::update() {
    
    blurMain.begin();
    face.draw(0,0);
    blurMain.end();
//    
    FPR.update();

}


void testApp::draw() {
	
    
   
    face.update();
    
    
    
	if(face.isFrameNew()) {
        
        tracker.update(toCv(face));

        position = tracker.getPosition();
		scale = tracker.getScale();
		orientation = tracker.getOrientation();
		rotationMatrix = tracker.getRotationMatrix();
	
        

    }
    
    
    //face.draw(0,0);
    if (ofGetMousePressed()) prevFrame.draw(0,0);
    //cout << tracker.getFound() <<endl;
    tracker.draw();
    
    FPR.drawFaceParts();
    FPR.drawFbo();

    ofSetColor(255);
	//face.draw(0, 0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
    
    if(face.isFrameNew()) {
        prevFrame.begin();
        face.draw(0,0);
        prevFrame.end();
    }
	
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
	}

    if (key == ' '){
        FPR.nFrames = 0;
    }
    
    if (key == 'e'){
        FPR.whichRecording++;
        FPR.whichRecording %= 90;
    }
}

