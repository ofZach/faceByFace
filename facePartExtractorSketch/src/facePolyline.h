//
//  facePolyline.h
//  ThreadedExample
//
//  Created by molmol on 10/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ThreadedExample_facePolyline_h
#define ThreadedExample_facePolyline_h


#include "ofMain.h"
#include "ofxCv.h"



using namespace ofxCv;



class facePolyline : public ofPolyline {
    
    
public: 
    
    void growAsEyes(){
        
        ofPoint midPt;
        for (int i = 0; i < size(); i++){
            midPt += (*this)[i];
        }
        midPt /= MAX(size(), 1);
        
        
        float pctUpX = 0;//0.7 * 1.3;
        float pctUpY = 0;//1.4 * 2.3;
        for (int i = 0; i < size(); i++){
            ofPoint diff = (*this)[i] - midPt;
            //if (diff.y < 0) diff.y *= 1.3;
            //else diff.y *= 0.1;
            (*this)[i].x += diff.x * 1.5;
            (*this)[i].y += diff.y * 2.3;
            
        }
        
        *((ofPolyline *)(this)) = getResampledByCount(100);
        *((ofPolyline *)(this))  = getSmoothed(9);
        
    }
    
    void growAsMouth(){
        
        ofPoint midPt;
        for (int i = 0; i < size(); i++){
            midPt += (*this)[i];
        }
        midPt /= MAX(size(), 1);
        
        
        float pctUpX = 0.7 * 0.6;
        float pctUpY = 1.4 * 0.6;
        for (int i = 0; i < size(); i++){
            ofPoint diff = (*this)[i] - midPt;
            (*this)[i].x += diff.x * pctUpX;
            (*this)[i].y += diff.y * pctUpY;
            
        }
        
        *((ofPolyline *)(this)) = getResampledByCount(100);
        *((ofPolyline *)(this))  = getSmoothed(9);
        
    }
    
    
    void growAsEyebrows(ofPoint bottomToTopOfNose){
        
        
        int n = size();
        for (int i = 0; i < n; i++){
            addVertex((*this)[ n - 1 - i]);
        }
        
        
        for (int i = 0; i < size(); i++){
            
            if (i < size()/2){
                (*this)[i] += bottomToTopOfNose * 0.1;
            } else {
                (*this)[i] -= bottomToTopOfNose * 0.1;
            }
        }
        
        setClosed(true);
        
        *((ofPolyline *)(this)) = getResampledByCount(100);
        *((ofPolyline *)(this))  = getSmoothed(9);
        
        //        float pctUpX = 0.7 * 0.6;
        //        float pctUpY = 1.4 * 0.6;
        //        for (int i = 0; i < size(); i++){
        //            ofPoint diff = (*this)[i] - midPt;
        //            (*this)[i].x += diff.x * pctUpX;
        //            (*this)[i].y += diff.y * pctUpY;
        //            
        //        }
        //        
        //        *((ofPolyline *)(this)) = getResampledByCount(100);
        //        *((ofPolyline *)(this))  = getSmoothed(9);
        
    }
    
    
    void growAsNose( vector < ofVec2f > pts){
        
        clear();
        
        
        ofPoint noseBridge = pts[27] - pts[30];
        
        pts[27] += noseBridge * 0.3;
        
        ofPoint midPtA = (pts[39] + pts[27])/2.0;
        ofPoint midPtB = (pts[42] + pts[27])/2.0;
        
        addVertex(midPtB);
        addVertex(midPtA);
        
        ofPoint diff = pts[35] - pts[31];
        
        for (int i =  31; i <= 35; i++){
            
            
            if (i == 31) addVertex(pts[i] - diff * 0.4 + noseBridge * 0.01);
            addVertex(pts[i] - noseBridge * 0.15);
            
            if (i == 35) addVertex(pts[i] + diff * 0.3 + noseBridge * 0.01);
        }
        
        setClosed(true);
        
        *((ofPolyline *)(this)) = getResampledByCount(100);
        *((ofPolyline *)(this))  = getSmoothed(9);
        
        // 39       42 
        
        //case NOSE_BRIDGE: return consecutive(27, 31);
        //case NOSE_BASE: return consecutive(31, 36);
        
    }
    
    
    
    float theta;
    ofPoint midPt;
    
    void calculateStats(){
        
        cv::Mat x(size(), 1, cv::DataType<cv::Point2f>::type);
        for(int j=0;j < size();j++) {
            x.at<cv::Point2f>(j).x = (*this)[j].x; 
            x.at<cv::Point2f>(j).y = (*this)[j].y; 
            
            midPt += ofPoint( (*this)[j].x,  (*this)[j].y);
        }
        midPt /= (float)size();
        
        cv::Moments m = cv::moments(x); 
        
        if (m.mu11 == 0.0f) m.mu11 = 0.00001f;  
        float d = (m.mu20 - m.mu02);  
        float b = d/m.mu11;  
        float quadratic = ((-1.0f*b) + sqrtf(b*b + 4.0f))/2.0f;  
        theta = atanf(quadratic);  
        
        if (((d < 0) && (quadratic < 1.0f)) ||  
            ((d > 0) && (quadratic > 1.0f))) { theta += HALF_PI;}  
        
        
        if (theta < 0) theta += PI;
        if (theta > PI/2) theta -= PI;
        
        //cout << theta << endl;
        
        if (fabs(theta - -PI/2) < 0.4){
            theta += PI/2;  
        } 
        
        
        //cout << theta << endl;
    }
    
    
    void drawStats(){
        ofLine(midPt, midPt + ofPoint(100 * cos(theta), 100 * sin(theta)));
    }
    
    void drawFrom00(){
        
        
        ofRectangle rect = getBoundingBox();
        
        
        for (int i = 0; i < size(); i++)
            (*this)[i] -= ofPoint(rect.x, rect.y);
        
        ofBeginShape();
        for (int i = 0; i < size(); i++)
            ofVertex((*this)[i]);
        ofEndShape();
        //draw();
        
        for (int i = 0; i < size(); i++)
            (*this)[i] += ofPoint(rect.x, rect.y);
        
        
    }
    
    
    void rotatePolyline(){
        
        for (int i = 0; i < size(); i++)
            (*this)[i] -= midPt;
        
        
        for (int i = 0; i < size(); i++)
            (*this)[i].rotate(-theta * RAD_TO_DEG, ofPoint(0,0,1));
        
        for (int i = 0; i < size(); i++)
            (*this)[i] += midPt;

    }
    
    
};

#endif
