#include "ofApp.h"

string versionNum = "2.0";

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofBackground(250);

	cout << "loading files..." << endl;
    
    errors.clear();
    warnings.clear();
    
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
        
        //General settings
        numPacks = xml.getValue("NUM_PACKS", numPacks);
        
        outputFolder = xml.getValue("OUTPUT_FOLDER", outputFolder);
        exportPDF = xml.getValue("EXPORT_PDF", "TRUE") == "TRUE";
        exportPNGs = xml.getValue("EXPORT_PNGS", "FALSE") == "TRUE";
        
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
            string idName = xml.getAttribute("folder", "idName", "unkown", i);
            string path = xml.getValue("folder", "unkown", i);
            cout<<"created folder "+idName+" pointing to "+path<<endl;
            
            if (idName == "unkown"){
                errors.push_back("Image source folder "+ofToString(i)+" has no id name.");
            }
            else if (path == "unkown"){
                errors.push_back("Image source folder '"+idName+"' does not list a folder path.");
            }
            else{
                SourceFolder folder;
                folder.setup(idName, path);
                if (folder.errorBadFolderPath){
                    errors.push_back("Image source folder path "+path+" does not exist");
                }
                else if (folder.errorNoImages){
                    errors.push_back("Image source folder '"+idName+"' has no valid image files.");
                    errors.push_back("    images must use .jpg .jpeg .png or .gif");
                }
                else{
                    sourceFolders.push_back(folder);
                }
            }
        }
        
        xml.popTag();
        
        //rarity tiers
        xml.pushTag("rarity_tiers");
        int numTiers = xml.getNumTags("tier");
        
        cout<<endl<<"loading "<<numTiers<<" rarity tiers"<<endl;
        
        for (int i=0; i<numTiers; i++){
            string referenceName = xml.getAttribute("tier", "name", "UNNAMED RARITY TIER", i);
            xml.pushTag("tier", i);
            cout<<"Creating tier "<<referenceName<<endl;;
            
            int cardCount = xml.getValue("count", 0);
            int numFolders = xml.getNumTags("folder");
            
            if (cardCount <= 0){
                errors.push_back("Rarity tier '"+ referenceName +"' has no card count.");
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
                        warnings.push_back("Folder '"+sourceName+"' in tier '"+referenceName+"' could not be found.");
                        warnings.push_back("    Make sure it is declared in the img_source section of settings.xml.");
                    }
                    else{
                        tier.addFolder(sourceIDNum, chance);
                    }
                }
                
                if (tier.imgSourceFolderIDs.size() == 0){
                    warnings.push_back("Skipping rarity tier '"+referenceName+"' because it had no viable source folders");
                }else{
                    rarityTiers.push_back(tier);
                }
            }
            
            xml.popTag();
        }
        
        xml.popTag();
        
    }else{
        errors.push_back("settings.xml file not found. Check readme for info.");
    }
    
    //make sure there is at least one imag source
    if (sourceFolders.size() == 0){
        errors.push_back("No image source folders. Check the <img_sources> tag in settings.xml.");
    }
    
    //make sure we have at least one rarity tier
    if (rarityTiers.size() == 0){
        errors.push_back("No rarity tiers with at least one valid source folder.");
    }
    
    //make sure there will be output
    if (!exportPDF && !exportPNGs){
        errors.push_back("Either EXPORT_PDF or EXPORT_PNGS must be set to TRUE. Check settings.xml.");
    }
    
    //count the number of cards that will be in each pack
    numCardsPerPack = 0;
    for (int i=0; i<rarityTiers.size(); i++){
        numCardsPerPack += rarityTiers[i].cardCount;
    }
    
    
    if (errors.size() == 0){
        cardW = sourceFolders[0].images[0].getWidth() + cardPadding;
        cardH = sourceFolders[0].images[0].getHeight() + cardPadding;
    }
    int pagePadding = 0;
    
    font.load("frabk.ttf", (int)((float)cardH * 0.033), true, false, true);
    
    fbo.allocate(cardW*3+pagePadding+edgePadding*2-cardPadding, cardH*3+pagePadding+edgePadding*2-cardPadding, GL_RGB);
    
    if (!printAllCards){
        for (int i=0; i<numPacks; i++){
            buildPack();
        }
    }else{
        addAllCards();
    }
    
    curCard = 0;
    
    if (errors.size() == 0 && exportPDF){
        ofBeginSaveScreenAsPDF(outputFolder+"/boosters.pdf", true, false, ofRectangle(0,0,fbo.getWidth(),fbo.getHeight()));
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    if (errors.size() > 0){
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
                    sourceFolders[sourceID].images[imgID].draw(cardW*c+edgePadding, cardH*r+edgePadding, cardW, cardH);
                    
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
                        ofDrawRectangle(outterBox);
                        ofSetColor(0);
                        font.drawString(packNumber, 0, 0);
                        ofPopMatrix();
                
                    }
				}
				curCard++;
				
            }
        }
        
        if (curCard >= cards.size() && exportPDF){
            ofEndSaveScreenAsPDF();
        }
		
        fbo.end();
        
        if (exportPNGs){
            ofPixels pixels;
            fbo.readToPixels(pixels);
            
            ofImage pic;
            pic.allocate(fbo.getWidth(), fbo.getHeight(), OF_IMAGE_COLOR);
            pic.setFromPixels(pixels);
            pic.save(outputFolder+"/page"+ofToString(curCard/9)+".png");
        }
		
    }
    else if (closeWhenDone){
        std::exit(1);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //reading out xml
    ofPushMatrix();
    ofTranslate(0,30);
    int settingsSpacing = 15;
    int settingCurY = 0;
    
    ofSetColor(0);
    
    //labels
    int labelX = 15;
    ofDrawBitmapString("Num packs to print:", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Allow duplicates in a pack:", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Output folder: ", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Export type: ", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Show pack numbers:", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Edge padding (px):", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Card padding (px):", labelX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString("Close when finished:", labelX, settingCurY);
    
    //values
    int valueX = 250;
    settingCurY = 0;
    ofDrawBitmapString(ofToString(numPacks), valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(allowDuplicates ? "TRUE" : "FALSE", valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(outputFolder, valueX, settingCurY);
    settingCurY += settingsSpacing;
    string exportType = "PDF & PNG";
    if (exportPDF && !exportPNGs)   exportType = "PDF";
    if (!exportPDF && exportPNGs)   exportType = "PNG";
    if (!exportPDF && !exportPNGs)   exportType = "None (That's bad)";
    ofDrawBitmapString(exportType, valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(showPackNumbers ? "TRUE" : "FALSE", valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(ofToString(edgePadding), valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(ofToString(cardPadding), valueX, settingCurY);
    settingCurY += settingsSpacing;
    ofDrawBitmapString(closeWhenDone ? "TRUE" : "FALSE", valueX, settingCurY);
    settingCurY += settingsSpacing;
    
    //Printing info
    if (errors.size() == 0){
        settingCurY += settingsSpacing * 2;
        
        float prc = (float)MIN(curCard, cards.size()) / (float)cards.size();
        
        ofSetColor(0);
        if (!printAllCards){
            ofDrawBitmapString("Printing: "+ ofToString( MIN(curCard/numCardsPerPack, cards.size()/numCardsPerPack) )+" out of "+ofToString(numPacks)+" packs", 15, settingCurY);
        }else{
            ofDrawBitmapString("Printing: "+ ofToString( MIN(curCard, cards.size()) )+" out of "+ofToString(cards.size())+" cards", 15, settingCurY);
        }
        settingCurY += settingsSpacing;
        
        ofDrawBitmapString("        : "+ ofToString( (int)(prc * 100) )+"%", 15, settingCurY);
        settingCurY += settingsSpacing * 2;
        
        int numBars = 35;
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
        ofDrawBitmapString(progress, 15, settingCurY);
        settingCurY += settingsSpacing;
    }
    
    //show errors
    if (errors.size() > 0){
        settingCurY += settingsSpacing * 2;
        ofSetColor(255,0,0);
        
        ofDrawBitmapString("ERRORS found. Cannot run until fixed.", labelX, settingCurY);
        settingCurY += settingsSpacing;
        ofDrawBitmapString("Adjust settings.xml and relaunch the application.", labelX, settingCurY);
        settingCurY += settingsSpacing * 1.5;
        
        for (int i=0; i<errors.size(); i++){
            ofDrawBitmapString(errors[i], labelX, settingCurY);
            settingCurY += settingsSpacing;
        }
    }
    
    //show warnings
    if (warnings.size() > 0 && errors.size() == 0){
        settingCurY += settingsSpacing * 2;
        ofSetColor(100,100,0);
        
        ofDrawBitmapString("WARNINGS - likely to cause problems with output", labelX, settingCurY);
        settingCurY += settingsSpacing * 1.5;
        
        for (int i=0; i<warnings.size(); i++){
            ofDrawBitmapString(warnings[i], labelX, settingCurY);
            settingCurY += settingsSpacing;
        }
    }
    
    
    
    
    
    ofPopMatrix();
    
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
    if (errors.size() > 0){
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
                warnings.push_back("Not enough card images in folder '"+sourceFolders[card.sourceFolderID].idName+"' to guarantee no duplicates.");
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
