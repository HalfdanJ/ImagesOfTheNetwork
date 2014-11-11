#pragma once

#include "ofMain.h"
#include "ofxSniffer.h"
#include "ofEvents.h"
#include "ofxThreadedImageLoader.h"
#include <queue>

#include "ofxJSON.h"

#define NUM_IMAGES 100
#define NUM_LOADERS 1

struct imageElement {
    ofImage img;
    ofVec2f pos;
    float alpha;
    int age;
    float fadeSpeed;
};

struct textElement {
    string text;
    ofVec2f pos;
    float alpha;
    int age;
    float fadeSpeed;
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void exit();
    void draw();

    
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxSniff sniffer;
    
    void newRawPacket(Packet & packet);
    void newHttpPacket(ofxSnifferHttpPacket & packet);
    

    imageElement images[NUM_IMAGES];
    vector<textElement> texts;
    vector<string> loadingUrls;
    queue<string> imagesToLoad;
    int imageidx;
    
    ofTrueTypeFont font;
    float connectAlpha;
    
    int loaderidx;
    
    vector<string> imageExtensions;
    
    ofShader shader;
    ofFbo fbo;
    ofImage connect;
    
    ofxJSONElement json;
};

