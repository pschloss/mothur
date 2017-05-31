//
//  sharedrabundfloatvectors.cpp
//  Mothur
//
//  Created by Sarah Westcott on 5/15/17.
//  Copyright © 2017 Schloss Lab. All rights reserved.
//

#include "sharedrabundfloatvectors.hpp"
#include "sharedutilities.h"

/***********************************************************************/
//reads a shared file
SharedRAbundFloatVectors::SharedRAbundFloatVectors(ifstream& f) : DataVector() {
    try {
        m->clearAllGroups();
        vector<string> allGroups;
        
        int num;
        string holdLabel, nextLabel, groupN;
        
        for (it = lookup.begin(); it != lookup.end(); it++) {  if (it->second != NULL) { delete it->second;  it->second = NULL; } }  lookup.clear();
        
        //are we at the beginning of the file??
        if (m->saveNextLabel == "") {
            f >> label;
            
            //is this a shared file that has headers
            if (label == "label") {
                //gets "group"
                f >> label; m->gobble(f);
                
                //gets "numOtus"
                f >> label; m->gobble(f);
                
                //eat rest of line
                label = m->getline(f); m->gobble(f);
                
                //parse labels to save
                istringstream iStringStream(label);
                m->sharedBinLabelsInFile.clear();
                while(!iStringStream.eof()){
                    if (m->control_pressed) { break; }
                    string temp;
                    iStringStream >> temp;  m->gobble(iStringStream);
                    
                    m->sharedBinLabelsInFile.push_back(temp);
                }
                
                f >> label >> groupN;
            }else {
                //read in first row since you know there is at least 1 group.
                f >> groupN;
                
                //make binlabels because we don't have any
                string snumBins = toString(num);
                m->sharedBinLabelsInFile.clear();
                for (int i = 0; i < num; i++) {
                    //if there is a bin label use it otherwise make one
                    string binLabel = "Otu";
                    string sbinNumber = toString(i+1);
                    if (sbinNumber.length() < snumBins.length()) {
                        int diff = snumBins.length() - sbinNumber.length();
                        for (int h = 0; h < diff; h++) { binLabel += "0"; }
                    }
                    binLabel += sbinNumber;
                    m->sharedBinLabelsInFile.push_back(binLabel);
                }
            }
        }else {
            label = m->saveNextLabel;
            
            //read in first row since you know there is at least 1 group.
            f >> groupN;
        }
        
        //reset labels, currentLabels may have gotten changed as otus were eliminated because of group choices or sampling
        m->currentSharedBinLabels = m->sharedBinLabelsInFile;
        
        holdLabel = label;
        allGroups.push_back(groupN);
        numBins = num;
        
        //add new vector to lookup
        RAbundFloatVector* temp = new RAbundFloatVector(f, label); m->gobble(f);
        push_back(temp, groupN);
        
        if (!(f.eof())) { f >> nextLabel; }
        
        //read the rest of the groups info in
        while ((nextLabel == holdLabel) && (f.eof() != true)) {
            f >> groupN;
            RAbundFloatVector* temp = new RAbundFloatVector(f, label); m->gobble(f);
            push_back(temp, groupN);
            allGroups.push_back(groupN);
            
            if (f.eof() != true) { f >> nextLabel; }
        }
        m->saveNextLabel = nextLabel;
        m->setAllGroups(allGroups);
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "SharedRAbundFloatVectors");
        exit(1);
    }
}
/***********************************************************************/
void SharedRAbundFloatVectors::printHeaders(ostream& output){
    try {
        string snumBins = toString(numBins);
        output << "label\tGroup\tnumOtus";
        if (m->sharedHeaderMode == "tax") {
            for (int i = 0; i < numBins; i++) {
                
                //if there is a bin label use it otherwise make one
                string binLabel = "PhyloType";
                string sbinNumber = toString(i+1);
                if (sbinNumber.length() < snumBins.length()) {
                    int diff = snumBins.length() - sbinNumber.length();
                    for (int h = 0; h < diff; h++) { binLabel += "0"; }
                }
                binLabel += sbinNumber;
                if (i < m->currentSharedBinLabels.size()) {  binLabel = m->currentSharedBinLabels[i]; }
                
                output << '\t' << binLabel ;
            }
            output << endl;
        }else {
            for (int i = 0; i < numBins; i++) {
                //if there is a bin label use it otherwise make one
                string binLabel = "Otu";
                string sbinNumber = toString(i+1);
                if (sbinNumber.length() < snumBins.length()) {
                    int diff = snumBins.length() - sbinNumber.length();
                    for (int h = 0; h < diff; h++) { binLabel += "0"; }
                }
                binLabel += sbinNumber;
                if (i < m->currentSharedBinLabels.size()) {  binLabel = m->currentSharedBinLabels[i]; }
                
                output  << '\t' << binLabel;
            }
            
            output << endl;
        }
        m->printedSharedHeaders = true;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "printHeaders");
        exit(1);
    }
}
/***********************************************************************/
void SharedRAbundFloatVectors::print(ostream& output){
    try {
        for (it = lookup.begin(); it != lookup.end(); it++) {
            if (m->control_pressed) { break; }
            output << label << '\t' << it->first << '\t';
            it->second->print(output, "noLabel");
        }
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "print");
        exit(1);
    }
}
/***********************************************************************/
int SharedRAbundFloatVectors::push_back(RAbundFloatVector* thisLookup, string group){
    try {
        lookup[group] = (thisLookup);
        if (label == "") { label = thisLookup->getLabel(); }
        return lookup.size();
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "push_back");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::getOTUTotal(int bin){
    try {
        float totalOTUAbund = 0;
        for (it = lookup.begin(); it != lookup.end(); it++) { totalOTUAbund += it->second->get(bin); }
        return totalOTUAbund;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getOTUTotal");
        exit(1);
    }
}
/***********************************************************************/
void SharedRAbundFloatVectors::setLabel(string l){
    try {
        label = l;
        for (it = lookup.begin(); it != lookup.end(); it++) { it->second->setLabel(l); }
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "setLabel");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::get(int bin, string group){
    try {
        float abund = 0;
        it = lookup.find(group);
        if (it == lookup.end()) { m->mothurOut("[ERROR]: can not find group " + group + ".\n"); m->control_pressed = true;  }
        else { abund = it->second->get(bin); }
        
        return abund;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "get");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::getNumSeqs(string group){
    try {
        float numSeqs = 0;
        it = lookup.find(group);
        if (it == lookup.end()) { m->mothurOut("[ERROR]: can not find group " + group + ".\n"); m->control_pressed = true;  }
        else { numSeqs = it->second->getNumSeqs(); }
        
        return numSeqs;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getNumSeqs");
        exit(1);
    }
}
/***********************************************************************/
void SharedRAbundFloatVectors::set(int bin, float binSize, string group){
    try {
        it = lookup.find(group);
        if (it == lookup.end()) { m->mothurOut("[ERROR]: can not find group " + group + ".\n"); m->control_pressed = true;  }
        else { it->second->set(bin, binSize); }
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "set");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::removeOTUs(vector<int> bins){
    try {
        float totalOTUAbund = 0;
        for (int i = 0; i < bins.size(); i++) { totalOTUAbund += removeOTU(bins[i]); }
        return totalOTUAbund;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundVectors", "removeOTU");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::removeOTU(int bin){
    try {
        float totalOTUAbund = 0;
        for (it = lookup.begin(); it != lookup.end(); it++) { totalOTUAbund += it->second->remove(bin); }
        numBins--;
        
        return totalOTUAbund;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "removeOTU");
        exit(1);
    }
}
/***********************************************************************/
vector<string> SharedRAbundFloatVectors::getNamesGroups(){
    try {
        vector<string> names;
        for (it = lookup.begin(); it != lookup.end(); it++) { names.push_back(it->first); }
        return names;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getNamesGroups");
        exit(1);
    }
}
/***********************************************************************/
float SharedRAbundFloatVectors::getNumSeqsSmallestGroup(){
    try {
        float smallest = 1e6;
        for (it = lookup.begin(); it != lookup.end(); it++) {
            if (it->second->getNumSeqs() < smallest) { smallest = it->second->getNumSeqs(); }
        }
        return smallest;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundVector", "getNumSeqsSmallestGroup");
        exit(1);
    }
}
/***********************************************************************/
RAbundVector SharedRAbundFloatVectors::getRAbundVector(){
    try {
        SharedUtil util;
        
        vector<string> Groups = m->getGroups();
        vector<string> allGroups = m->getAllGroups();
        util.setGroups(Groups, allGroups);
        m->setGroups(Groups);
        
        bool remove = false;
        for (it = lookup.begin(); it != lookup.end();) {
            //if this sharedrabund is not from a group the user wants then delete it.
            if (util.isValidGroup(it->first, m->getGroups()) == false) {
                remove = true;
                delete it->second; it->second = NULL;
                lookup.erase(it++);
            }else { it++; }
        }
        
        if (remove) { eliminateZeroOTUS(); }
        
        RAbundVector rav;
        for (int i = 0; i < numBins; i++) {
            float abund = getOTUTotal(i);
            rav.push_back((int)abund);
        }
        
        return rav;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getSharedRAbundVectors");
        exit(1);
    }
}
/***********************************************************************/
SAbundVector SharedRAbundFloatVectors::getSAbundVector(){
    try {
        RAbundVector rav = getRAbundVector();
        return rav.getSAbundVector();
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getSharedRAbundVectors");
        exit(1);
    }
}
/***********************************************************************/
OrderVector SharedRAbundFloatVectors::getOrderVector(map<string,int>* nameMap = NULL){
    try {
        RAbundVector rav = getRAbundVector();
        return rav.getOrderVector(NULL);
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getSharedRAbundVectors");
        exit(1);
    }
}
/***********************************************************************/
vector<RAbundFloatVector*> SharedRAbundFloatVectors::getSharedRAbundFloatVectors(){
    try {
        SharedUtil util;
        vector<string> Groups = m->getGroups();
        vector<string> allGroups = m->getAllGroups();
        util.setGroups(Groups, allGroups);
        m->setGroups(Groups);
        
        bool remove = false;
        for (it = lookup.begin(); it != lookup.end();) {
            //if this sharedrabund is not from a group the user wants then delete it.
            if (util.isValidGroup(it->first, m->getGroups()) == false) {
                remove = true;
                delete it->second; it->second = NULL;
                lookup.erase(it++);
            }else {
                it++;
            }
        }
        
        if (remove) { eliminateZeroOTUS(); }
        
        vector<RAbundFloatVector*> newLookup;
        for (it = lookup.begin(); it != lookup.end(); it++) {
            RAbundFloatVector* temp = new RAbundFloatVector(it->second->getRAbundFloatVector());
            newLookup.push_back(temp);
        }
        
        return newLookup;
        
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getSharedRAbundVectors");
        exit(1);
    }
}
/***********************************************************************/
vector<RAbundVector*> SharedRAbundFloatVectors::getSharedRAbundVectors(){
    try {
        SharedUtil util;
        
        vector<string> Groups = m->getGroups();
        vector<string> allGroups = m->getAllGroups();
        util.setGroups(Groups, allGroups);
        m->setGroups(Groups);
        
        bool remove = false;
        for (it = lookup.begin(); it != lookup.end();) {
            //if this sharedrabund is not from a group the user wants then delete it.
            if (util.isValidGroup(it->first, m->getGroups()) == false) {
                remove = true;
                delete it->second; it->second = NULL;
                lookup.erase(it++);
            }else { it++; }
        }
        
        if (remove) { eliminateZeroOTUS(); }
        
        vector<RAbundVector*> newLookup;
        for (it = lookup.begin(); it != lookup.end(); it++) {
            RAbundVector* temp = new RAbundVector(it->second->getRAbundVector());
            newLookup.push_back(temp);
        }

        return newLookup;
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "getSharedRAbundVectors");
        exit(1);
    }
}
/***********************************************************************/
void SharedRAbundFloatVectors::removeGroups(vector<string> g){
    try {
        SharedUtil util;
        
        vector<string> Groups = m->getGroups();
        vector<string> allGroups = m->getAllGroups();
        util.setGroups(Groups, allGroups);
        m->setGroups(Groups);
        
        bool remove = false;
        for (it = lookup.begin(); it != lookup.end();) {
            //if this sharedrabund is not from a group the user wants to remove.
            if (util.isValidGroup(it->first, g) == true) {
                remove = true;
                delete it->second; it->second = NULL;
                lookup.erase(it++);
            }else { it++; }
        }
        
        if (remove) { eliminateZeroOTUS(); }
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "removeGroups");
        exit(1);
    }
}

/**********************************************************************************************************************/
int SharedRAbundFloatVectors::eliminateZeroOTUS() {
    try {
        //for each bin
        vector<string> newBinLabels;
        string snumBins = toString(numBins);
        for (int i = 0; i < numBins; i++) {
            if (m->control_pressed) { return 0; }
            
            float total = getOTUTotal(i);
            
            //if they are not all zero add this bin
            if (total == 0) { removeOTU(i); }
            else {
                //if there is a bin label use it otherwise make one
                string binLabel = "Otu";
                string sbinNumber = toString(i+1);
                if (sbinNumber.length() < snumBins.length()) {
                    int diff = snumBins.length() - sbinNumber.length();
                    for (int h = 0; h < diff; h++) { binLabel += "0"; }
                }
                binLabel += sbinNumber;
                if (i < m->currentSharedBinLabels.size()) {  binLabel = m->currentSharedBinLabels[i]; }
                
                newBinLabels.push_back(binLabel);
            }
        }
        
        m->currentSharedBinLabels = newBinLabels;
        
        return 0;
        
    }
    catch(exception& e) {
        m->errorOut(e, "SharedRAbundFloatVectors", "eliminateZeroOTUS");
        exit(1);
    }
}
/***********************************************************************/

