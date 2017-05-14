#include "ofApp.h"

string versionNum = "1.1";

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofBackground(250);

	cout << "loading files..." << endl;
    
    errorsFound = false;
    
    //set some defaults
    numPacks = 0;
    
    allowDuplicates = false;
    
    outputFolder = "none";
    
    closeWhenDone = false;
    showPackNumbers = false;
    
    printAllCards = false;
    
    edgePadding = 0;
    cardPadding = 0;
    
    //get some info from XML
    ofxXmlSettings xml;
    if (xml.loadFile("../settings.xml")){
        settingsFileNotFound = false;
        
        //General settings
        numPacks = xml.getValue("NUM_PACKS", numPacks);
        
        outputFolder = xml.getValue("OUTPUT_FOLDER", outputFolder);
        
        closeWhenDone = xml.getValue("CLOSE_WHEN_FINISHED", "FALSE") == "TRUE";
        
        showPackNumbers = xml.getValue("SHOW_PACK_NUMBERS", "TRUE") == "TRUE";
        
        edgePadding = xml.getValue("EDGE_PADDING", edgePadding);
        cardPadding = xml.getValue("CARD_PADDING", cardPadding);
        
        printAllCards = xml.getValue("PRINT_ALL_CARDS_IN_ORDER", "FALSE") == "TRUE";
        
        allowDuplicates = xml.getValue("ALLOW_DUPLICATES", "FALSE") == "TRUE";
        
        packStartingNumber = xml.getValue("PACKS_START_COUNTING_FROM", 1);
        
        //image source folders
        xml.pushTag("img_sources");
        
        int imgSourceNum = xml.getNumTags("folder");
        for (int i=0; i<imgSourceNum; i++){
            string idName = xml.getAttribute("folder", "id", "none", i);
            string path = xml.getValue("folder", "none", i);
            cout<<"create folder "+idName+" pointing to "+path<<endl;
            
            if (idName == "none"){
                cout<<"MISSING ID NAME FOR SOURCE FOLDER"<<endl;
            }
            else if (path == "none"){
                cout<<"MISSING PATH FOR SOURCE FOLDER"<<endl;
            }
            else{
                SourceFolder folder;
                folder.setup(idName, path);
                sourceFolders.push_back(folder);
            }
        }
        
        xml.popTag();
        
        //rarity tiers
        xml.pushTag("rarity_tiers");
        int numTiers = xml.getNumTags("tier");
        
        cout<<endl<<"loading "<<numTiers<<" rarity tiers"<<endl;
        
        for (int i=0; i<numTiers; i++){
            string referenceName = xml.getAttribute("tier", "name", "none", i);
            xml.pushTag("tier", i);
            cout<<"Creating tier "<<referenceName<<endl;;
            
            int cardCount = xml.getValue("count", 0);
            int numFolders = xml.getNumTags("folder");
            
            if (cardCount <= 0){
                cout<<"tier "+ referenceName +" has no card count"<<endl;
            }
            else{
            
                RarityTier tier;
                tier.setup(referenceName, cardCount);
                
                for (int f=0; f<numFolders; f++){
                    float chance = xml.getAttribute("folder", "chance", 100, f);
                    string sourceName = xml.getValue("folder", "none", f);
                    
                    cout<<"adding folder "<<sourceName<<" with a "<<chance<<"% chance"<<endl;
                    
                    
                    //find the source folder
                    int sourceIDNum = -1;
                    for (int s=0; s<sourceFolders.size(); s++){
                        if (sourceFolders[s].idName == sourceName){
                            sourceIDNum = s;
                        }
                    }
                    
                    if (sourceIDNum == -1){
                        cout<<"skipping unkown source folder in tier "<<referenceName<<": "+sourceName<<endl;
                    }
                    else{
                        tier.addFolder(sourceIDNum, chance);
                    }
                }
                
                if (tier.imgSourceFolderIDs.size() == 0){
                    cout<<"skipping tier "+referenceName+" because it had no source folders"<<endl;
                }else{
                    rarityTiers.push_back(tier);
                }
            }
            
            xml.popTag();
        }
        
        xml.popTag();
        
    }else{
        settingsFileNotFound = true;
        errorsFound = true;
    }
    
    numCardsPerPack = 0;//numCommonsPerPack + numUncommonsPerPack + numRaresPerPack;
    for (int i=0; i<rarityTiers.size(); i++){
        numCardsPerPack += rarityTiers[i].cardCount;
    }
    
    //loadCardImageFiles();
    
    //make sure we have eough of each type
    if (!errorsFound){      //don't bother with this check if we already have errors
//        if (allowDuplicates){
//            if (commons.size() < 1)                         notEnoughCommons = true;
//            if (uncommons.size() < 1)                       notEnoughUncommons = true;
//            if (rares.size() < 1)                           notEnoughRares = true;
//        }else{
//            if (commons.size() < numCommonsPerPack)         notEnoughCommons = true;
//            if (uncommons.size() < numUncommonsPerPack)     notEnoughUncommons = true;
//            if (rares.size() < numRaresPerPack)             notEnoughRares = true;
//        }
    }
    
    if (notEnoughCommons || notEnoughUncommons || notEnoughRares){
        errorsFound = true;
    }
    
    if (!errorsFound){
        cardW = sourceFolders[0].images[0].getWidth() + cardPadding;
        cardH = sourceFolders[0].images[0].getHeight() + cardPadding;
    }
    int pagePadding = 0;
    
    font.loadFont("frabk.ttf", (int)((float)cardH * 0.033));
    
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
                    int sourceID = cards[curCard].sourceFolderID;
                    int imgID = cards[curCard].idNum;
                    sourceFolders[sourceID].images[imgID].draw(cardW*c+edgePadding, cardH*r+edgePadding, false);
                    
                    //print the pack number
                    if (showPackNumbers && !printAllCards){
                        string packNumber = ofToString(packStartingNumber + curCard/numCardsPerPack);
                        
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
//    int settingsX = 250;
//    settingCurY = 0;
//    ofDrawBitmapString(ofToString(numPacks), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor( notEnoughCommons ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(ofToString(numCommonsPerPack), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor( notEnoughUncommons ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(ofToString(numUncommonsPerPack), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor( notEnoughRares ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(ofToString(numRaresPerPack), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofDrawBitmapString(allowDuplicates ? "TRUE" : "FALSE", settingsX, settingCurY);
//    settingCurY += settingsSpacing * 2;
//    ofSetColor( folderNotFoundCommon ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(sourceFolderCommons, settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor( folderNotFoundUncommon ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(sourceFolderUncommons, settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor( folderNotFoundRare ? ofColor::red : ofColor::black);
//    ofDrawBitmapString(sourceFolderRares, settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofSetColor(0);
//    ofDrawBitmapString(outputFolder, settingsX, settingCurY);
//    settingCurY += settingsSpacing*2;
//    ofDrawBitmapString(showPackNumbers ? "TRUE" : "FALSE", settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofDrawBitmapString(ofToString(edgePadding), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofDrawBitmapString(ofToString(cardPadding), settingsX, settingCurY);
//    settingCurY += settingsSpacing;
//    ofDrawBitmapString(closeWhenDone ? "TRUE" : "FALSE", settingsX, settingCurY);
//    
//    if (errorsFound){
//        settingCurY += settingsSpacing * 3;
//        ofSetColor(255,0,0);
//        if (settingsFileNotFound){
//            ofSetColor(255, 0, 0);
//            ofDrawBitmapString("SETTINGS FILE NOT FOUND\nCHECK README FOR INFO", 15, settingCurY);
//        }else{
//            string errorMessage = "ERRORS FOUND\n";
//            
//            if (folderNotFoundCommon)   errorMessage += "Comons source folder not found\n";
//            if (folderNotFoundUncommon)   errorMessage += "Uncomons source folder not found\n";
//            if (folderNotFoundRare)   errorMessage += "Rare source folder not found\n";
//            
//            if (notEnoughCommons)   errorMessage += "Not enough common card images ("+ofToString(commons.size())+")\n";
//            if (notEnoughUncommons)   errorMessage += "Not enough uncommon card images ("+ofToString(uncommons.size())+")\n";
//            if (notEnoughRares)   errorMessage += "Not enough rare card images ("+ofToString(rares.size())+")\n";
//            
//            errorMessage += "CHECK settings.xml AND TRY AGAIN";
//            ofDrawBitmapString(errorMessage, 15, settingCurY);
//        }
//    }
    
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
//        ofDrawBitmapString("commons  : "+ofToString(commons.size()), 15, 100);
//        ofDrawBitmapString("uncommons: "+ofToString(uncommons.size()), 15, 115);
//        ofDrawBitmapString("rares    : "+ofToString(rares.size()), 15, 130);
        
        ofPopMatrix();
    }
    
    ofSetColor(0);
    ofDrawBitmapString("Generator by Andy Wallace. AndyMakes.com", 15, ofGetHeight()-5);
    ofDrawBitmapString("version "+versionNum, ofGetWidth()-100, 15);
    
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
    
    vector<Card> pack;
    
    for (int r=0; r<rarityTiers.size(); r++){
        for (int c=0; c<rarityTiers[r].cardCount; c++){
            Card card = rarityTiers[r].getCard();   //this will lock in the rarity level
            bool goodID = false;
            int attemptCount = 0;
            int maxAttaempts = 10000;
            while (!goodID && attemptCount<maxAttaempts){
                attemptCount++;
                card.idNum = sourceFolders[card.sourceFolderID].getRandomIDNum();   //this will select a random card from that folder
                //cout<<"trying id "<<card.idNum<<" in source "+sourceFolders[card.sourceFolderID].idName<<endl;
                goodID = true;
                
                if (!allowDuplicates){
                    for (int i=0; i<pack.size(); i++){
                        if (pack[i].sourceFolderID == card.sourceFolderID && pack[i].idNum == card.idNum){
                            goodID = false;
                        }
                    }
                }
            }
            if (attemptCount == maxAttaempts){
                cout<<"not enough card images in folder "+sourceFolders[card.sourceFolderID].idName<<" to guarantee no duplicates. Ocurred on tier "<<rarityTiers[r].referenceName<<endl;
            }
            pack.push_back(card);
        }
    }
    
    for (int i=0; i<pack.size(); i++){
        cards.push_back(pack[i]);
    }
    
}

//--------------------------------------------------------------
void ofApp::addAllCards(){
    
    for (int i=0; i<sourceFolders.size(); i++){
        for (int k=0; k<sourceFolders[i].images.size(); k++){
            Card thisCard;
            thisCard.sourceFolderID = i;
            thisCard.idNum = k;
        }
    }
    
    
}


////--------------------------------------------------------------
//void ofApp::loadCardImageFiles(){
//    
//    ofDirectory commonDir, uncommonDir, rareDir;
//    commonDir.listDir(sourceFolderCommons);
//    uncommonDir.listDir(sourceFolderUncommons);
//    rareDir.listDir(sourceFolderRares);
//    
//	commons.clear();
//	uncommons.clear();
//	rares.clear();
//    
//    if (!commonDir.isDirectory()){
//        folderNotFoundCommon = true;
//        errorsFound = true;
//    }else{
//        folderNotFoundCommon = false;
//    }
//    
//    if (!uncommonDir.isDirectory()){
//        folderNotFoundUncommon = true;
//        errorsFound = true;
//    }else{
//        folderNotFoundUncommon = false;
//    }
//    
//    if (!rareDir.isDirectory()){
//        folderNotFoundRare = true;
//        errorsFound = true;
//    }else{
//        folderNotFoundRare = false;
//    }
//    
//    if (errorsFound){
//        return;
//    }
//    
//    for (int i=0; i<commonDir.size(); i++){
//		if (fileIsOK(commonDir.getFile(i))) {
//			//cout << "load " << commonDir.getPath(i) << endl;
//			ofImage newCard;
//			newCard.loadImage(commonDir.getPath(i));
//			commons.push_back(newCard);
//		}
//    }
//    for (int i=0; i<uncommonDir.size(); i++){
//		if (fileIsOK(uncommonDir.getFile(i))) {
//			ofImage newCard;
//			newCard.loadImage(uncommonDir.getPath(i));
//			uncommons.push_back(newCard);
//		}
//    }
//    for (int i=0; i<rareDir.size(); i++){
//		if (fileIsOK(rareDir.getFile(i))) {
//			ofImage newCard;
//			newCard.loadImage(rareDir.getPath(i));
//			rares.push_back(newCard);
//		}
//    }
//    
//}
//
//
////--------------------------------------------------------------
//bool ofApp::fileIsOK(ofFile thisFile) {
//	string ext = thisFile.getExtension();
//	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
//	if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif") {
//		return true;
//	}
//	cout << thisFile.getFileName() <<" was rejected because it is not a recognized file type" << endl;
//	return false;
//	
//}
