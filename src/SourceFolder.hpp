//
//  SourceFolder.hpp
//  card_generator
//
//  Created by Andrew Wallace on 5/14/17.
//
//

#ifndef SourceFolder_hpp
#define SourceFolder_hpp

#include "ofMain.h"

class SourceFolder{
public:
    
    void setup(string _idName, string path);
    bool fileIsOK(ofFile thisFile);
    
    int getRandomIDNum();
    
    string idName;
    vector<ofImage> images;
    
    bool errorBadFolderPath;
    bool errorNoImages;
};

#endif /* SourceFolder_hpp */
