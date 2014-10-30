#include "ofApp.h"



float easing(float t, float b, float c, float d) {
    t /= d/2.;
    if (t < 1) return c/2.*t*t + b;
    t--;
    return -c/2. * (t*(t-2) - 1) + b;
};


//--------------------------------------------------------------
void ofApp::setup(){
    

    ofAddListener(sniffer.newHttpPacketEvent, this, &ofApp::newHttpPacket);
    
    sniffer.startSniffing("en0");
    //ofAddListener(sniffer.newRawPacketEvent, this, &ofApp::newRawPacket);
    
    imageidx = 0;
    
    ofBackground(0);
    
    imageExtensions.push_back("jpg");
    imageExtensions.push_back("jpeg");
    imageExtensions.push_back("png");
    imageExtensions.push_back("gif");
    
}

void ofApp::newRawPacket(Packet & packet){
    const Tins::IP &ip = packet.pdu()->rfind_pdu<Tins::IP>(); // Find the IP layer
    const Tins::TCP &tcp = packet.pdu()->rfind_pdu<Tins::TCP>(); // Find the TCP layer
    
    const Tins::RawPDU &raw = packet.pdu()->rfind_pdu<Tins::RawPDU>();
    
    
    std::cout << ip.src_addr() << ':' << tcp.sport() << " -> "
     << ip.dst_addr() << ':' << tcp.dport() << std::endl;
}



void ofApp::newHttpPacket(ofxLibtinsHttpPacket & packet){
    string extension = ofFilePath().getFileExt(packet.request);

    bool isImage =  std::find(imageExtensions.begin(), imageExtensions.end(), extension) != imageExtensions.end();

    
    cout<<extension<<endl;
    if(isImage ){

        string url = "http://"+packet.host+packet.request;
        
        bool found = false;
        for(int i=0;i<loadingUrls.size();i++){
            if(loadingUrls[i] == url){
                found = true;
                loadingUrls.erase(loadingUrls.begin()+i);
                break;
            }
        }
        
        if(!found){
            loadingUrls.push_back(url);
            
            string filename = ofToString(loaderidx++)+"."+extension;
            
            if(loaderidx > 500)
                loaderidx = 0;
            
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,0), ^{
                ofSaveURLTo(url, filename);
                dispatch_async(dispatch_get_main_queue(), ^{
                    imagesToLoad.push(filename);
                });
            });
            
        }
        
    }
}


//--------------------------------------------------------------
void ofApp::update(){
    if(imagesToLoad.size() > 0){
        string url = imagesToLoad.front();
        imagesToLoad.pop();
        
        bool load = images[imageidx].img.load(url);
        if(load){
            int sx = images[imageidx].img.getWidth() * 0.5;
            int sy = images[imageidx].img.getHeight() * 0.5;
            images[imageidx].age = 0;
            images[imageidx].alpha = 1.;
            images[imageidx].pos.x = ofRandom(sx,ofGetWidth()-sx);
            images[imageidx].pos.y = ofRandom(sy,ofGetHeight()-sy);
            images[imageidx].fadeSpeed = ofRandom(1,1.3);
            imageidx = (imageidx+1)%NUM_IMAGES;
        }
    }
    
    for(int i=0;i< NUM_IMAGES;i++){
        if(images[i].alpha > 0){
            images[i].age ++;
            images[i].alpha -= 0.004*images[i].fadeSpeed;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    for(int i=0;i< NUM_IMAGES;i++){
        int j = (i + imageidx - NUM_IMAGES)%NUM_IMAGES;
        while(j<0){
            j += NUM_IMAGES;
        }
        
        if(images[j].img.isAllocated() && images[j].alpha > 0){
            float a = easing(images[j].alpha, 0, 1, 1);
            ofSetColor(255, 255, 255,255*a);

            ofPushMatrix();
            ofTranslate(images[j].pos.x,images[j].pos.y);
            
            int frames = 10;
            if(images[j].age < frames){
                float t = easing(images[j].age, 0, 0.1, frames);
                float s = 0.9 + t;
                ofScale(s,s);
            }

            images[j].img.draw(0,0);
            ofPopMatrix();
        }
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    for(int i=0;i<NUM_LOADERS;i++){
        loader[i].stopThread();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}