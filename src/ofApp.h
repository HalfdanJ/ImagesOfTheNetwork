#pragma once

#include "ofMain.h"
#include "ofxLibtins.h"
#include "ofEvents.h"
#include "ofxThreadedImageLoader.h"
#include <queue>


#define NUM_IMAGES 20
#define NUM_LOADERS 1

struct imageElement {
    ofImage img;
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
    
    ofxLibtinsSimpleSniffer sniffer;
    
    void newRawPacket(Packet & packet);
    void newHttpPacket(ofxLibtinsHttpPacket & packet);
    
    ofxThreadedImageLoader loader[NUM_LOADERS];

    imageElement images[NUM_IMAGES];
    vector<string> loadingUrls;
    queue<string> imagesToLoad;
    int imageidx;
    
    int loaderidx;
    
    vector<string> imageExtensions;
};

