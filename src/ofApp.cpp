#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofEnableAntiAliasing();

	cout << "loading files..." << endl;
    
    errorsFound = false;
    
    //set some defaults
    numPacks = 0;
    numCommonsPerPack = 0;
    numUncommonsPerPack = 0;
    numRaresPerPack = 0;
    
    notEnoughCommons = false;
    notEnoughUncommons = false;
    notEnoughRares = false;
    
    allowDuplicates = false;
    
    sourceFolderCommons = "none";
    sourceFolderUncommons = "none";
    sourceFolderRares = "none";
    
    outputFolder = "none";
    
    closeWhenDone = false;
    showPackNumbers = false;
    
    printAllCards = false;
    
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
        
        printAllCards = xml.getValue("PRINT_ALL_CARDS_IN_ORDER", "FALSE") == "TRUE";
        
        allowDuplicates = xml.getValue("ALLOW_DUPLICATES", "FALSE") == "TRUE";
        
        settingsFileNotFound = false;
    }else{
        settingsFileNotFound = true;
        errorsFound = true;
    }
    
    numCardsPerPack = numCommonsPerPack + numUncommonsPerPack + numRaresPerPack;
    
    loadCardImageFiles();
    
    //make sure we have eough of each type
    if (!errorsFound){      //don't bother with this check if we already have errors
        if (allowDuplicates){
            if (commons.size() < 1)                         notEnoughCommons = true;
            if (uncommons.size() < 1)                       notEnoughUncommons = true;
            if (rares.size() < 1)                           notEnoughRares = true;
        }else{
            if (commons.size() < numCommonsPerPack)         notEnoughCommons = true;
            if (uncommons.size() < numUncommonsPerPack)     notEnoughUncommons = true;
            if (rares.size() < numRaresPerPack)             notEnoughRares = true;
        }
    }
    
    if (notEnoughCommons || notEnoughUncommons || notEnoughRares){
        errorsFound = true;
    }
    
    if (!errorsFound){
        cardW = commons[0].getWidth() + cardPadding;
        cardH = commons[0].getHeight() + cardPadding;
    }
    int pagePadding = 0;
    
    font.loadFont("Helvetica.ttf", (int)((float)cardH * 0.033));
    
    fbo.allocate(cardW*3+pagePadding+edgePadding*2-cardPadding, cardH*3+pagePadding+edgePadding*2-cardPadding, GL_RGB);
    
    if (!printAllCards){
        for (int i=0; i<numPacks; i++){
            buildPack();
        }
    }else{
        addAllCards();
    }
    
    curCard = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    if (errorsFound){
        return;
    }
	
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
                    if (showPackNumbers && !printAllCards){
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
    
    //reading out xml
    ofPushMatrix();
    ofTranslate(0,40);
    int settingsSpacing = 15;
    int settingCurY = 0;
    
    ofSetColor(0);
    
    //labels
    ofDrawBitmapString("Num packs to print:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Num commons per pack:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Num uncommons per pack:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Num rares per pack:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Allow duplicates in a pack:", 15, settingCurY);
    settingCurY += settingsSpacing * 2;
    ofDrawBitmapString("Commons source folder:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Uncommons source folder:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Rares source folder", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Output folder", 15, settingCurY);
    settingCurY += settingsSpacing * 2;
    ofDrawBitmapString("Show pack numbers:", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Edge padding (px):", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Card padding (px):", 15, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Close when finished:", 15, settingCurY);
    
    //values
    int settingsX = 250;
    settingCurY = 0;
    ofDrawBitmapString(ofToString(numPacks), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor( notEnoughCommons ? ofColor::red : ofColor::black);
    ofDrawBitmapString(ofToString(numCommonsPerPack), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor( notEnoughUncommons ? ofColor::red : ofColor::black);
    ofDrawBitmapString(ofToString(numUncommonsPerPack), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor( notEnoughRares ? ofColor::red : ofColor::black);
    ofDrawBitmapString(ofToString(numRaresPerPack), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(allowDuplicates ? "TRUE" : "FALSE", settingsX, settingCurY);
    settingCurY += settingsSpacing * 2;
    ofSetColor( folderNotFoundCommon ? ofColor::red : ofColor::black);
    ofDrawBitmapString(sourceFolderCommons, settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor( folderNotFoundUncommon ? ofColor::red : ofColor::black);
    ofDrawBitmapString(sourceFolderUncommons, settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor( folderNotFoundRare ? ofColor::red : ofColor::black);
    ofDrawBitmapString(sourceFolderRares, settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofSetColor(0);
    ofDrawBitmapString(outputFolder, settingsX, settingCurY);
    settingCurY += settingsSpacing*2;
    ofDrawBitmapString(showPackNumbers ? "TRUE" : "FALSE", settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(ofToString(edgePadding), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(ofToString(cardPadding), settingsX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(closeWhenDone ? "TRUE" : "FALSE", settingsX, settingCurY);
    
    if (errorsFound){
        settingCurY += settingsSpacing * 3;
        ofSetColor(255,0,0);
        if (settingsFileNotFound){
            ofSetColor(255, 0, 0);
            ofDrawBitmapString("SETTINGS FILE NOT FOUND\nCHECK README FOR INFO", 15, settingCurY);
        }else{
            string errorMessage = "ERRORS FOUND\n";
            
            if (folderNotFoundCommon)   errorMessage += "Comons source folder not found\n";
            if (folderNotFoundUncommon)   errorMessage += "Uncomons source folder not found\n";
            if (folderNotFoundRare)   errorMessage += "Rare source folder not found\n";
            
            if (notEnoughCommons)   errorMessage += "Not enough common card images ("+ofToString(commons.size())+")\n";
            if (notEnoughUncommons)   errorMessage += "Not enough uncommon card images ("+ofToString(uncommons.size())+")\n";
            if (notEnoughRares)   errorMessage += "Not enough rare card images ("+ofToString(rares.size())+")\n";
            
            errorMessage += "CHECK settings.xml AND TRY AGAIN";
            ofDrawBitmapString(errorMessage, 15, settingCurY);
        }
    }
    
    ofPopMatrix();
    
    //Printing info
    if (!errorsFound){
        ofPushMatrix();
        ofTranslate(0, 300);
        
        float prc = (float)MIN(curCard, cards.size()) / (float)cards.size();
        
        ofSetColor(0);
        if (!printAllCards){
            ofDrawBitmapString("Printing: "+ ofToString( MIN(curCard/numCardsPerPack, cards.size()/numCardsPerPack) )+" out of "+ofToString(numPacks)+" packs", 15, 20);
        }else{
            ofDrawBitmapString("Printing: "+ ofToString( MIN(curCard, cards.size()) )+" out of "+ofToString(cards.size())+" cards", 15, 20);
        }
        ofDrawBitmapString("        : "+ ofToString( (int)(prc * 100) )+"%", 15, 35);
        //cout << ofToString((int)(prc * 100)) << "%" << endl;
        
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
        
        ofPopMatrix();
    }
    
    ofSetColor(0);
    ofDrawBitmapString("Generator by Andy Wallace. AndyMakes.com", 15, ofGetHeight()-5);
    
    
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
    if (errorsFound){
        return;
    }
    
    //jesus christ. make a function for this.
    
    vector<int> idsUsed;

    for (int i=0; i<numCommonsPerPack; i++){
        int thisCardID = (int)ofRandom(commons.size());
        bool goodID = allowDuplicates;
        while (!goodID){
            thisCardID = (int)ofRandom(commons.size());
            goodID = true;
            for (int k=0; k<idsUsed.size(); k++){
                if (thisCardID == idsUsed[k]){
                    goodID = false;
                }
            }
        }
        idsUsed.push_back(thisCardID);
        cards.push_back( commons[thisCardID] );
    }
    
    idsUsed.clear();
    for (int i=0; i<numUncommonsPerPack; i++){
        int thisCardID = (int)ofRandom(uncommons.size());
        bool goodID = allowDuplicates;
        while (!goodID){
            thisCardID = (int)ofRandom(uncommons.size());
            goodID = true;
            for (int k=0; k<idsUsed.size(); k++){
                if (thisCardID == idsUsed[k]){
                    goodID = false;
                }
            }
        }
        idsUsed.push_back(thisCardID);
        cards.push_back( uncommons[thisCardID] );
    }
    
    idsUsed.clear();
    for (int i=0; i<numRaresPerPack; i++){
        int thisCardID = (int)ofRandom(rares.size());
        bool goodID = allowDuplicates;
        while (!goodID){
            thisCardID = (int)ofRandom(rares.size());
            goodID = true;
            for (int k=0; k<idsUsed.size(); k++){
                if (thisCardID == idsUsed[k]){
                    goodID = false;
                }
            }
        }
        idsUsed.push_back(thisCardID);
        cards.push_back( rares[thisCardID] );
    }
    
}

//--------------------------------------------------------------
void ofApp::addAllCards(){
    
    for (int i=0; i<commons.size(); i++){
        cards.push_back( commons[i] );
    }
    
    for (int i=0; i<uncommons.size(); i++){
        cards.push_back( uncommons[i] );
    }
    for (int i=0; i<rares.size(); i++){
        cards.push_back( rares[i] );
    }
    
}


//--------------------------------------------------------------
void ofApp::loadCardImageFiles(){
    
    ofDirectory commonDir, uncommonDir, rareDir;
    commonDir.listDir(sourceFolderCommons);
    uncommonDir.listDir(sourceFolderUncommons);
    rareDir.listDir(sourceFolderRares);
    
	commons.clear();
	uncommons.clear();
	rares.clear();
    
    if (!commonDir.isDirectory()){
        folderNotFoundCommon = true;
        errorsFound = true;
    }else{
        folderNotFoundCommon = false;
    }
    
    if (!uncommonDir.isDirectory()){
        folderNotFoundUncommon = true;
        errorsFound = true;
    }else{
        folderNotFoundUncommon = false;
    }
    
    if (!rareDir.isDirectory()){
        folderNotFoundRare = true;
        errorsFound = true;
    }else{
        folderNotFoundRare = false;
    }
    
    if (errorsFound){
        return;
    }
    
    for (int i=0; i<commonDir.size(); i++){
		if (fileIsOK(commonDir.getFile(i))) {
			//cout << "load " << commonDir.getPath(i) << endl;
			ofImage newCard;
			newCard.loadImage(commonDir.getPath(i));
			commons.push_back(newCard);
		}
    }
    for (int i=0; i<uncommonDir.size(); i++){
		if (fileIsOK(uncommonDir.getFile(i))) {
			ofImage newCard;
			newCard.loadImage(uncommonDir.getPath(i));
			uncommons.push_back(newCard);
		}
    }
    for (int i=0; i<rareDir.size(); i++){
		if (fileIsOK(rareDir.getFile(i))) {
			ofImage newCard;
			newCard.loadImage(rareDir.getPath(i));
			rares.push_back(newCard);
		}
    }
    
}


//--------------------------------------------------------------
bool ofApp::fileIsOK(ofFile thisFile) {
	string ext = thisFile.getExtension();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif") {
		return true;
	}
	cout << thisFile.getFileName() <<" was rejected because it is not a recognized file type" << endl;
	return false;
	
}