#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    //set some defaults
    numPacks = 1;
    numCommonsPerPack = 11;
    numUncommonsPerPack = 3;
    numRaresPerPack = 1;
    
    sourceFolderCommons = "source/c";
    sourceFolderUncommons = "source/u";
    sourceFolderRares = "source/r";
    
    outputFolder = "output";
    
    closeWhenDone = false;
    showPackNumbers = true;
    
    edgePadding = 0;
    cardPadding = 0;
    
    //get some info from XML
    ofxXmlSettings xml;
    if (xml.loadFile("../settings.xml")){
        numPacks = xml.getValue("NUM_PACKS", numPacks);
        
        numCommonsPerPack = xml.getValue("NUM_COMMON", numCommonsPerPack);
        numUncommonsPerPack = xml.getValue("NUM_UNCOMMON", numUncommonsPerPack);
        numRaresPerPack = xml.getValue("NUM_RARE", numRaresPerPack);
        
        sourceFolderCommons = xml.getValue("COMMON_SOURCE_FOLDER", sourceFolderCommons);
        sourceFolderUncommons = xml.getValue("UNCOMMON_SOURCE_FOLDER", sourceFolderUncommons);
        sourceFolderRares = xml.getValue("RARE_SOURCE_FOLDER", sourceFolderRares);
        
        outputFolder = xml.getValue("OUTPUT_FOLDER", outputFolder);
        
        closeWhenDone = xml.getValue("CLOSE_WHEN_FINISHED", "FALSE") == "TRUE";
        
        showPackNumbers = xml.getValue("SHOW_PACK_NUMBERS", "TRUE") == "TRUE";
        
        edgePadding = xml.getValue("EDGE_PADDING", edgePadding);
        cardPadding = xml.getValue("CARD_PADDING", cardPadding);
        
        settingsFileNotFound = false;
    }else{
        settingsFileNotFound = true;
    }
    
    numCardsPerPack = numCommonsPerPack + numUncommonsPerPack + numRaresPerPack;
    
    
    loadCardImageFiles();
    
    
    
    cardW = commons[0].width + cardPadding;
    cardH = commons[1].height + cardPadding;
    int pagePadding = 0;
    
    font.loadFont("Helvetica.ttf", (int)((float)cardH * 0.033));
    
    fbo.allocate(cardW*3+pagePadding+edgePadding*2-cardPadding, cardH*3+pagePadding+edgePadding*2-cardPadding, GL_RGB);
    
    
    for (int i=0; i<numPacks; i++){
        buildPack();
    }
    
    curCard = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (curCard < cards.size()){
        fbo.begin();
        ofClear(255,255,255,0);
        ofSetColor(255);
        for (int r=0; r<3; r++){
            for (int c=0; c<3; c++){
                if (curCard < cards.size()){
                    ofSetColor(255);
                    cards[curCard].draw(cardW*c+edgePadding, cardH*r+edgePadding, false);
                    
                    //print the pack number
                    if (showPackNumbers){
                        string packNumber = ofToString(curCard/numCardsPerPack + 1);
                        
                        ofRectangle outterBox = font.getStringBoundingBox(packNumber, 0, 0);
                        float padding = cardH * 0.01;
                        outterBox.x -= padding;
                        outterBox.y -= padding;
                        outterBox.width += padding * 2;
                        outterBox.height += padding * 2;
                        
                        
                        ofPushMatrix();
                        int boxPadding = cardH * 0.03;
                        ofTranslate(cardW*c +edgePadding + boxPadding, cardH*(r+1) - cardPadding + edgePadding - boxPadding);
                        
                        ofSetColor(255,80);
                        ofRect(outterBox);
                        ofSetColor(0);
                        font.drawString(packNumber, 0, 0);
                        ofPopMatrix();
                    }
                }
                curCard++;
            }
        }
        fbo.end();
        
        ofPixels pixels;
        fbo.readToPixels(pixels);
        
        ofImage pic;
        pic.allocate(fbo.getWidth(), fbo.getHeight(), OF_IMAGE_COLOR);
        pic.setFromPixels(pixels);
        pic.saveImage(outputFolder+"/page"+ofToString(curCard/9)+".png");
    }
    else if (closeWhenDone){
        std::exit(1);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofSetColor(255);
    //fbo.draw(0, 0);
    //cards[0].draw(0, 0);
    
    float prc = (float)MIN(curCard, cards.size()) / (float)cards.size();
    
    ofSetColor(0);
    ofDrawBitmapString("Printing: "+ ofToString( MIN(curCard/numCardsPerPack, cards.size()/numCardsPerPack) )+" out of "+ofToString(numPacks)+" packs", 15, 20);
    ofDrawBitmapString("        : "+ ofToString( (int)(prc * 100) )+"%", 15, 35);
    
    int numBars = 35;//55;
    int progressPoint = numBars * prc;
    
    string progress = "<";
    for (int i=0; i<numBars; i++){
        if (i < progressPoint){
            progress += "=";
        }
        else if (i > progressPoint){
            progress += "-";
        }
        else{
            progress += "#";
        }
    }
    progress += ">";
    ofDrawBitmapString(progress, 15, 70);
    
    //card info
    ofDrawBitmapString("commons  : "+ofToString(commons.size()), 15, 100);
    ofDrawBitmapString("uncommons: "+ofToString(uncommons.size()), 15, 115);
    ofDrawBitmapString("rares    : "+ofToString(rares.size()), 15, 130);
    
    ofDrawBitmapString("Generator by Andy Wallace. AndyMakes.com", 15, ofGetHeight()-5);
    
    if (settingsFileNotFound){
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("SETTINGS FILE NOT FOUND", 15, 165);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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

//--------------------------------------------------------------
void ofApp::buildPack(){

    
    for (int i=0; i<numCommonsPerPack; i++){
        cards.push_back( commons[ (int)ofRandom(commons.size())] );
    }
    
    for (int i=0; i<numUncommonsPerPack; i++){
        cards.push_back( uncommons[ (int)ofRandom(uncommons.size())] );
    }
    
    for (int i=0; i<numRaresPerPack; i++){
        cards.push_back( rares[ (int)ofRandom(rares.size())] );
    }
    
}


//--------------------------------------------------------------
void ofApp::loadCardImageFiles(){
    
    ofDirectory commonDir, uncommonDir, rareDir;
    commonDir.listDir(sourceFolderCommons);
    uncommonDir.listDir(sourceFolderUncommons);
    rareDir.listDir(sourceFolderRares);
    
    commons.assign(commonDir.size(), ofImage());
    uncommons.assign(uncommonDir.size(), ofImage());
    rares.assign(rareDir.size(), ofImage());
    
    for (int i=0; i<commonDir.size(); i++){
        commons[i].loadImage(commonDir.getPath(i));
    }
    for (int i=0; i<uncommonDir.size(); i++){
        uncommons[i].loadImage(uncommonDir.getPath(i));
    }
    for (int i=0; i<rareDir.size(); i++){
        rares[i].loadImage(rareDir.getPath(i));
    }
    
}