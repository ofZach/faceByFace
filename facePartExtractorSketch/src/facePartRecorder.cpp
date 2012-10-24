#include "facePartRecorder.h"
#include "facePolyline.h"
#include "testApp.h"

#define BOXSIZE  100.0


void facePartRecorder::setup() {
	
    
    ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
    //cam.setDeviceID(0);
	//cam.initGrabber(640, 480);

    for (int i =0; i < 6; i++){
        offscreen[i].allocate(BOXSIZE,BOXSIZE);
        offscreen[i].begin();
        ofClear(0,0,0,0);
        offscreen[i].end();
    }
        
   
    alphaThrough.load("shaders/alphaThrough");
    shader.load("shaders/alpha");
    
    //blurMain.setup(640,480, 64);
    //prevFrame.allocate(640,480);
   
    eyeImg.loadImage("masks/eye.png");
    mouthImg.loadImage("masks/mouth.png");
    noseImg.loadImage("masks/nose.png");
    eyeImg.resize(BOXSIZE, BOXSIZE);
    mouthImg.resize(BOXSIZE, BOXSIZE);
    noseImg.resize(BOXSIZE, BOXSIZE);
    
    
    GLint texSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
    //cout << "maximum texture size " << texSize << endl;
    
    
    
    recording.allocate(8000, BOXSIZE * 6); // let's optimize!
    
    for (int i = 0; i < 90; i++){
        recordings[i].allocate(8000, BOXSIZE * 6);
        recordings[i].begin();
        ofClear(1,0,0,0);
        recordings[i].end();
    }
    
    recording.begin();
    ofClear(1,0,0,0);
    recording.end();
    
    //cout << recording.getWidth() << endl;
    
    nFrames = 0;
    bRecording = true;
    
    
    whichRecording = 0;
    
    //bool bRecording;
    //ofFbo recording;
    
}

void facePartRecorder::update() {
    
    //reload shader
    //if (ofGetFrameNum() % 30 == 0) shader.load("shaders/alpha");
    
    
    if (ofGetFrameNum() % 30 == 0) alphaThrough.load("shaders/alphaThrough");
    
    
    // blur the face image. 
    
        
    if (bRecording && ofGetFrameNum() % 2 == 0){
        
        ofDisableAlphaBlending();
        
        recordings[whichRecording].begin();
        
        
        glEnable(GL_SCISSOR_TEST);
        glScissor(nFrames * BOXSIZE,0, BOXSIZE,recording.getHeight());
        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
        
        ofFill();
        
        alphaThrough.begin();
        for (int i =0; i < 6; i++){
            
            glColor4f(1.0, 1.0, 1.0, 0.0);
            ofRect(nFrames * BOXSIZE,i*BOXSIZE, BOXSIZE, BOXSIZE);
            glColor4f(1.0, 1.0, 1.0, 1.0);
            offscreen[i].draw(nFrames * BOXSIZE,i*BOXSIZE);
        }
        alphaThrough.end();
        recordings[whichRecording].end();
        nFrames++;
    }
        
    
}

void facePartRecorder::drawPart(facePolyline & eye, int who){
    
    float scale = eye.getBoundingBox().width / BOXSIZE;
    
    if (who == 3){
        scale = eye.getBoundingBox().height / BOXSIZE;
    }
    ofPushMatrix();
    ofTranslate(eye.midPt.x, eye.midPt.y);
    ofScale(scale , scale );
    ofRotateZ(eye.theta * RAD_TO_DEG);
    ofEnableAlphaBlending();
    
   
    recordings[whichRecording].getTextureReference().bind();
    
    glBegin(GL_QUADS);
    glTexCoord2d( (ofGetFrameNum()/2 % 50) * BOXSIZE, who * BOXSIZE);
    glVertex2d(-BOXSIZE/2,-BOXSIZE/2);
    glTexCoord2d( (ofGetFrameNum()/2 % 50 + 1) * BOXSIZE, who * BOXSIZE);
    glVertex2d(BOXSIZE/2,-BOXSIZE/2);
    glTexCoord2d( (ofGetFrameNum()/2 % 50 + 1) * BOXSIZE, (who+1) * BOXSIZE);
    glVertex2d(BOXSIZE/2,BOXSIZE/2);
    glTexCoord2d( (ofGetFrameNum()/2 % 50) * BOXSIZE, (who+1) * BOXSIZE);
    glVertex2d(-BOXSIZE/2,BOXSIZE/2);
    glEnd();
    
    //offscreen[3].draw(-BOXSIZE/2,-BOXSIZE/2);
    
    ofPopMatrix();
    
    recordings[whichRecording].getTextureReference().unbind();
    
}


void facePartRecorder::drawSomePart(facePolyline & eye, ofImage & mask, int who){

    ofPoint shapeMidPt = eye.midPt - ofPoint(eye.getBoundingBox().x,eye.getBoundingBox().y);
    float scale = BOXSIZE / eye.getBoundingBox().width;
    if (who == 3) scale = BOXSIZE / eye.getBoundingBox().height;
    shapeMidPt *= scale;
    
    shader.begin();  
    shader.setUniformTexture("tex0", mask.getTextureReference(), 0);  
    
    shader.setUniformTexture("tex1", ((testApp*)ofGetAppPtr())->prevFrame.getTextureReference(), 1); 
    shader.setUniformTexture("tex2", ((testApp*)ofGetAppPtr())->prevFrame.getTextureReference(), 2); 
    
    shader.setUniform1f("midx", eye.midPt.x);
    shader.setUniform1f("midy", eye.midPt.y);
    shader.setUniform1f("scaleAmount", 1/scale);
    shader.setUniform1f("sinMe",  sin(-eye.theta));
    shader.setUniform1f("cosMe",  cos(-eye.theta));
    
    shader.setUniform1f("shapeMidptX",  BOXSIZE/2);
    shader.setUniform1f("shapeMidptY",  BOXSIZE/2);
    
    shader.setUniform1f("sinMe",  sin(eye.theta));
    shader.setUniform1f("cosMe",  cos(eye.theta));
    
    ofDisableAlphaBlending();
    //face.getTextureReference().bind();                // TODO!
    
    offscreen[who].begin();
    ofClear(0,0,0,0);
    mask.draw(0,0);
    offscreen[who].end();

    //face.getTextureReference().unbind();               // TODO!

    shader.end();

}


void facePartRecorder::drawFaceParts() {
	
   
    
    if(((testApp*)ofGetAppPtr())->tracker.getFound()) {
		
        // 27 = top of nose
        // 30 = bottom of nose;
        
         vector < ofVec2f > pts = ((testApp*)ofGetAppPtr())->tracker.getImagePoints();
//        //cout << pts.size() << endl;
//        for (int i= 0; i < pts.size(); i++){
//            ofPoint mouse(mouseX, mouseY);
//            if ((mouse - pts[i]).length() < 10){
//                ofDrawBitmapString(ofToString(i), pts[i]);
//            }
//        }
//       // fof (int i = 0; i < tracker.getImagePoint(0)
        
        ofPoint noseBridge = pts[27] - pts[30];
        
        
        
        facePolyline eye;
        *((ofPolyline *)(&eye)) = ((testApp*)ofGetAppPtr())->tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
        eye.growAsEyes();
        //eye.draw();
        eye.calculateStats();
        //eye.drawStats();
        eye.rotatePolyline();
        //eye.draw();
        
               
        drawSomePart(eye, eyeImg, 0);
        
        
        float scale;
        scale = eye.getBoundingBox().width / BOXSIZE;
        ofPushMatrix();
        ofTranslate(eye.midPt.x, eye.midPt.y);
        ofScale(scale, scale);
        ofRotateZ(eye.theta * RAD_TO_DEG);
        ofEnableAlphaBlending();
        //offscreen[0].draw(-BOXSIZE/2,-BOXSIZE/2);
        ofPopMatrix();
        
        
        
        drawPart(eye, 0);
        
        //virtual void  drawBlobIntoMe( ofxCvBlob& blob, int color );
        
      
        
        //drawSomePart(eye, ofPoint(mouseX, mouseY));
        
        //growablePolyline eye;
        *((ofPolyline *)(&eye)) = ((testApp*)ofGetAppPtr())->tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
        eye.growAsEyes();
        //eye.draw();
        
        
        eye.calculateStats();
        //eye.drawStats();
        eye.rotatePolyline();
        //eye.draw();
        
        
        drawSomePart(eye, eyeImg, 1);
        
        drawPart(eye, 1);
        
        
        //scale = eye.getBoundingBox().width / BOXSIZE;
        //scale *= ofMap(mouseX, 0, ofGetWidth(), 0.3, 4.0);
        ofPushMatrix();
        ofTranslate(eye.midPt.x, eye.midPt.y);
        ofScale(scale, scale);
        ofRotateZ(eye.theta * RAD_TO_DEG);
        ofEnableAlphaBlending();
        //offscreen[1].draw(-BOXSIZE/2,-BOXSIZE/2);
        ofPopMatrix();
        
        
        
        
        
        *((ofPolyline *)(&eye)) = ((testApp*)ofGetAppPtr())->tracker.getImageFeature(ofxFaceTracker::OUTER_MOUTH);
        eye.growAsMouth();
        //eye.draw();
        eye.calculateStats();
        eye.drawStats();
        eye.rotatePolyline();
        //eye.draw();
        
        //eye.drawStats();
        drawSomePart(eye, mouthImg, 2);
        
        
        drawPart(eye, 2);
        
       
        
        
        scale = eye.getBoundingBox().width / BOXSIZE;
        ofPushMatrix();
        ofTranslate(eye.midPt.x, eye.midPt.y);
        ofScale(scale, scale);
        ofRotateZ(eye.theta * RAD_TO_DEG);
        ofEnableAlphaBlending();
        //offscreen[2].draw(-BOXSIZE/2,-BOXSIZE/2);
        ofPopMatrix();
        
        
        
        //drawSomePart(eye, ofPoint(mouseX+100, mouseY));
        
        /*
        
        
        *((ofPolyline *)(&eye)) = tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW);
        eye.growAsEyebrows(noseBridge);
        eye.draw();
        eye.calculateStats();
        eye.drawStats();
        eye.rotatePolyline();
        eye.draw();
        
        drawSomePart(eye, 0);
        
        
        //drawSomePart(eye, ofPoint(mouseX, mouseY + 50));

        
        *((ofPolyline *)(&eye)) = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW);
        eye.growAsEyebrows(noseBridge);
        eye.draw();
        eye.calculateStats();
        eye.drawStats();
        eye.rotatePolyline();
        eye.draw();
        
        
        drawSomePart(eye, 0);
        
        */
        
        //drawSomePart(eye, ofPoint(mouseX + 50, mouseY + 50));
        
        eye.growAsNose(pts);
        //eye.draw();
        eye.calculateStats();
        eye.drawStats();
        eye.rotatePolyline();
        //eye.draw();
        
        drawSomePart(eye, noseImg, 3);
        
        scale = eye.getBoundingBox().height / BOXSIZE;
        ofPushMatrix();
        ofTranslate(eye.midPt.x, eye.midPt.y);
        ofScale(scale, scale);
        ofRotateZ(eye.theta * RAD_TO_DEG);
        ofEnableAlphaBlending();
        //offscreen[3].draw(-BOXSIZE/2,-BOXSIZE/2);
        ofPopMatrix();
        
        
        drawPart(eye, 3);
        
        //drawSomePart(eye, ofPoint(mouseX + 100, mouseY + 50));
     
        
        
	}

}



void facePartRecorder::drawFbo(){
    float scalefff = ofGetWidth() / (float)recording.getWidth();
    ofPushMatrix();
    ofTranslate(0, ofGetHeight() - scalefff*recording.getHeight());
    ofScale(scalefff, scalefff);
    recordings[whichRecording].draw(0,0);
    ofPopMatrix();
}