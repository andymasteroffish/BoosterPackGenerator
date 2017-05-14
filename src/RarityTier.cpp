//
//  RarityTier.cpp
//  card_generator
//
//  Created by Andrew Wallace on 5/14/17.
//
//

#include "RarityTier.hpp"

void RarityTier::setup(string _referenceName, int _cardCount){
    referenceName = _referenceName;
    cardCount = _cardCount;
    imgSourceFolderIDs.clear();
    folderWeights.clear();
    
    totalWeight = 0;
}

void RarityTier::addFolder(int folderID, float chance){
    imgSourceFolderIDs.push_back(folderID);
    folderWeights.push_back(chance);
    totalWeight += chance;
}

Card RarityTier::getCard(){
    
    float roll = ofRandom(totalWeight);
    
    for (int i=0; i<imgSourceFolderIDs.size(); i++){
        roll -= folderWeights[i];
        if (roll < 0){
            Card thisCard;
            thisCard.sourceFolderID = imgSourceFolderIDs[i];
            return thisCard;
        }
    }
    
}
