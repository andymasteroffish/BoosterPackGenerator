#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include <string>     // std::string, std::stoi

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void buildPack();
    void addAllCards();
    
    void loadCardImageFiles();
	bool fileIsOK(ofFile thisFile);
    
    ofTrueTypeFont font;
    
    int numPacks;
    int packSize;
    int cardW, cardH;
    
    
    //what we're drawing from
    string sourceFolderCommons, sourceFolderUncommons, sourceFolderRares;
    vector<ofImage> commons, uncommons, rares;
    int numCommonsPerPack, numUncommonsPerPack, numRaresPerPack;
    int numCardsPerPack;
    bool allowDuplicates;
    
    bool printAllCards; //if this is true, instead of making booster packs, we just print one of each card
    
    //what's being pinted
    vector<ofImage> cards;
    ofFbo fbo;
    int curCard;
    string outputFolder;
    bool showPackNumbers;
    
    int edgePadding, cardPadding;
    
    bool settingsFileNotFound;
    bool closeWhenDone;
    
};
