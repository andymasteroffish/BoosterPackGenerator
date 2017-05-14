//
//  RarityTier.hpp
//  card_generator
//
//  Created by Andrew Wallace on 5/14/17.
//
//

#ifndef RarityTier_hpp
#define RarityTier_hpp

#include "ofMain.h"
#include "Card.hpp"

class RarityTier{
public:
    
    void setup(string _referenceName, int _cardCount);
    void addFolder(int folderID, float chance);
    
    Card getCard();
    
    string referenceName;
    int cardCount;
    vector<int>     imgSourceFolderIDs;
    vector<float>   folderWeights;
    
    float totalWeight;
    
};

#endif /* RarityTier_hpp */
