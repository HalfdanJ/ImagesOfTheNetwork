#include "ofApp.h"



float easing(float t, float b, float c, float d) {
 /*   t /= d/2.;
    if (t < 1) return c/2.*t*t + b;
    t--;
    return -c/2. * (t*(t-2) - 1) + b;*/
    if (t < 1) return -c/2. * (sqrt(1 - t*t) - 1) + b;
    t -= 2;
    return c/2 * (sqrt(1 - t*t) + 1) + b;};


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
    
    fbo.allocate(1024,768);
    
    
    shader.load("shaders/noise");
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
    
    
    bool isImage = false;
    for(int i=0;i<imageExtensions.size();i++){
        if(extension.find(imageExtensions[i]) != -1){
            isImage = true;
        }
    }

    string url = "http://"+packet.host+packet.request;
    
//    cout<<extension<<endl;
    if(isImage ){

        
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
    } else if (extension.find("json") != -1){
        
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
            
            cout<<"JSON!!"<<endl;
            string filename = ofToString(loaderidx++)+".json";
            
            if(loaderidx > 500)
                loaderidx = 0;
            
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,0), ^{
                ofSaveURLTo(url, filename);
                
                std::ifstream t;
                t.open(ofToDataPath(filename).c_str());
                std::string buffer;
                std::string line;
                while(t){
                    std::getline(t, line);
                    buffer += line;
                }
                t.close();
                
                dispatch_async(dispatch_get_main_queue(), ^{
                   //cout<<buffer<<endl;
                    json = ofxJSONElement(buffer);
                    //cout<<json.getRawString(true)<<endl;;
                    
                    textElement txt;
                    txt.alpha = 1.0;
                    txt.age = 0;
                    txt.text = json.getRawString(true);
                    txt.pos.x = ofRandom(0,ofGetWidth()*0.6);
                    txt.pos.y = ofRandom(0,ofGetHeight()*0.6);
                    txt.fadeSpeed = ofRandom(1,1.8);

                    texts.push_back(txt);
                });
            });
        }
    } else {
        cout<<extension<<endl;
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
            images[imageidx].fadeSpeed = ofRandom(1,1.8);
            imageidx = (imageidx+1)%NUM_IMAGES;
        }
    }
    
    for(int i=0;i< NUM_IMAGES;i++){
        if(images[i].alpha > 0){
            images[i].age ++;
            images[i].alpha -= 0.003*images[i].fadeSpeed* 60.0/ofGetFrameRate();
        }
    }
    
    for(int i=0;i<texts.size();i++){
        texts[i].age ++;
        texts[i].alpha -= 0.006*texts[i].fadeSpeed* 60.0/ofGetFrameRate();

        if(texts[i].alpha <= 0){
            texts.erase(texts.begin()+i);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    
    ofClear(0,0,0);
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
            ofScale(1.3,1.3);
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
    fbo.end();

       ofSetRectMode(OF_RECTMODE_CORNER);
    
    shader.begin();
    
    //we want to pass in some varrying values to animate our type / color
//    shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
//    shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );
//    
//    //we also pass in the mouse position
//    //we have to transform the coords to what the shader is expecting which is 0,0 in the center and y axis flipped.
//    shader.setUniform2f("mouse", mouseX - ofGetWidth()/2, ofGetHeight()/2-mouseY );
//    
//    
//
    shader.setUniform1f("globalTime",ofGetElapsedTimef());
    fbo.draw(0,0);
    
//    shader.setUniformTexture("tex", fbo, 0);
    shader.end();
    
    for(int i=0;i<texts.size();i++){
        float a = easing(texts[i].alpha, 0, 1, 1);
        ofPushMatrix();
        ofTranslate(texts[i].pos);
        ofScale(2,2);
        int frames = 5;
        if(texts[i].age < frames){
            float t = easing(texts[i].age, 0, 0.1, frames);
            float s = 0.9 + t;
            ofScale(s,s);
        }
        
        ofSetColor(255, 255, 255, a*255*2);
        ofDrawBitmapString(texts[i].text,0,0);
        ofPopMatrix();
    }
    
    

}

//--------------------------------------------------------------
void ofApp::exit(){
   
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    } else {
        shader.load("shaders/noise");
    }

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