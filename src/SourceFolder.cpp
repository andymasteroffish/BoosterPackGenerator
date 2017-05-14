//
//  SourceFolder.cpp
//  card_generator
//
//  Created by Andrew Wallace on 5/14/17.
//
//

#include "SourceFolder.hpp"

void SourceFolder::setup(string _idName, string path){
    idName = _idName;
    
    ofDirectory dir;
    dir.listDir(path);
    
    images.clear();
    
    if (!dir.isDirectory()){
        cout<<path <<" is not a directory!"<<endl;
    }else{
        //folderNotFoundCommon = false;
    }
    
//    if (errorsFound){
//        return;
//    }
    
    for (int i=0; i<dir.size(); i++){
        if (fileIsOK(dir.getFile(i))) {
            //cout << "load " << dir.getPath(i) << endl;
            ofImage newCard;
            newCard.loadImage(dir.getPath(i));
            images.push_back(newCard);
        }
    }
    
    cout<<"loaded "<<images.size()<<" cards into "+idName<<endl;
}

bool SourceFolder::fileIsOK(ofFile thisFile) {
    string ext = thisFile.getExtension();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif") {
        return true;
    }
    cout << thisFile.getFileName() <<" was rejected because it is not a recognized file type" << endl;
    return false;
    
}


int SourceFolder::getRandomIDNum(){
    return (int)ofRandom(0, images.size());
}
