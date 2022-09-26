#include "TMath.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TVector.h"
#include "TF1.h"
#include "THashList.h"
#include "TList.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TChain.h"
#include <list>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <typeinfo>
#include <algorithm>
#include "MCHGeometryCreator/Geometry.h"
#include "MCHGeometryTransformer/Transformations.h"
#include "MCHGeometryTest/Helpers.h"
#include "SimulationDataFormat/MCCompLabel.h"
#include "SimulationDataFormat/MCTruthContainer.h"
#include "DataFormatsMCH/Digit.h"
#include "DataFormatsMCH/ROFRecord.h"
#include "MCHMappingInterface/Segmentation.h"
#include "TGeoManager.h"


using namespace std;
using namespace o2;
using namespace o2::mch;
using namespace o2::dataformats;

void Embedding_test(const char *filename = "mchdigits-data.root", const char *filename2 = "mchdigits-0.root")
{
//______________________
    //Data extraction
    int entries_number = 0;
    auto TFtime = 0;
    vector<Digit>* digitou = 0;
    vector<Digit>* digitou2 = 0;
    vector<Digit> digitoutemp, digitoutemp2, digitstemp;
    vector<ROFRecord>* rof = 0;
    vector<ROFRecord>* rof2 = 0;
    vector<ROFRecord> roftemp, roftemp2, rofoutemp;
    MCTruthContainer<MCCompLabel> MCLabtemp;
    MCTruthContainer<MCCompLabel>* MCLab = 0;
    MCTruthContainer<MCCompLabel>* MCLab2 = 0;
    vector<Digit>* digitouAll = 0;
    vector<ROFRecord>* rofAll = 0;
    MCTruthContainer<o2::MCCompLabel>* MCLabAll = 0;
    
    TFile hfile("mchdigits.root","RECREATE");
    TTree *treeOutput = new TTree("o2sim","treeOutput");
    auto branchDig = treeOutput->Branch("MCHDigit", &digitouAll);
    auto branchRof = treeOutput->Branch("MCHROFRecords", &rofAll);
    auto branchMCLab = treeOutput->Branch("MCHMCLabels", &MCLabAll);

    TFile *file = TFile::Open(filename,"read");
    
    if(!file)
    {
     	cout << "Error :: The file is not found, please check"<< endl;
     	return;
    }
    TTree *tree= (TTree*) file->Get("o2sim");
    if(!tree)
    {
     	cout << "Error :: The Tree is not found, please check"<< endl;
     	return;
    }
    
    tree->SetBranchAddress("MCHDigit",&digitou);
    tree->SetBranchAddress("MCHROFRecords",&rof);
    
    tree->GetEntry(0);
    entries_number = tree->GetEntries();
    digitstemp = *digitou;
    roftemp = *rof;
    MCLab = 0;
    for (Int_t n = 1; n<entries_number; n++) {
      	tree->GetEntry(n);
      	digitstemp.insert( digitstemp.end(), digitou->begin(), digitou->end() );
      	roftemp.insert( roftemp.end(), rof->begin(), rof->end() );
    }
    digitou = &digitstemp;
    rof = &roftemp;
    TFile *file2 = TFile::Open(filename2,"read");
    TTree *tree2= (TTree*) file2->Get("o2sim");
    tree2->SetBranchAddress("MCHDigit",&digitou2);
    tree2->SetBranchAddress("MCHROFRecords",&rof2);
    tree2->SetBranchAddress("MCHMCLabels",&MCLab2);
    tree2->GetEntry(0);
    
    int counterErase = 0;
    int counterRof = 0;
    digitoutemp = *digitou;
    rofoutemp = *rof;
    vector<int> eraseindex;
    
    for (Int_t i = 0; i<int(rof->size()); i++) {
   	 rofoutemp.at(i).setDataRef(counterRof, rof->at(i).getNEntries());
   	 counterRof += rof->at(i).getNEntries();
    }
    rof = &rofoutemp;

    for (Int_t i = 0; i<int(rof->size()); i++) {
    	rofoutemp.at(i).setDataRef(rof->at(i).getFirstIdx()-counterErase, rof->at(i).getNEntries());
    	for (Int_t j = int(rof->at(i).getFirstIdx()); j<int(rof->at(i).getLastIdx()); j++) {
    	 	for (Int_t k = j+1; k<int(rof->at(i).getLastIdx()+1); k++) {
    			if(digitou->at(j).getPadID()==digitou->at(k).getPadID() and digitou->at(j).getTime()==digitou->at(k).getTime()){
    				//cout << "Data - ROF " << i << ", digits " << j << " and " << k << " : Pad = " << digitou->at(j).getPadID() << ", TF = " << digitou->at(j).getTime() << ", ADC = " << digitou->at(j).getADC() << " and " << digitou->at(k).getADC() << endl;
    				digitoutemp.at(k-counterErase).setADC(digitou->at(j).getADC()+digitou->at(k).getADC());
    				digitoutemp.erase(digitoutemp.begin() + j - counterErase);
    				rofoutemp.at(i).setDataRef(rofoutemp.at(i).getFirstIdx(), rofoutemp.at(i).getNEntries()-1);
    				eraseindex.push_back(j);
    				counterErase++;
    				break;
    			}
    		}
    	}
    }
    
    int counterEraselab = 0;
    for (Int_t i = 0; i<int(digitou->size()); i++) {
    	if ( find(eraseindex.begin(), eraseindex.end(), i) != eraseindex.end() ){
    	 	continue;
    	 }
    	 else{
    	 	MCLabtemp.addElement(MCLabtemp.getNElements(), 0);
    	 	counterEraselab++;
    	 	
    	 }
    }
    
    digitou = &digitoutemp;
    rof = &rofoutemp;
    
    counterErase = 0;
    digitoutemp2 = *digitou2;
    roftemp2 = *rof2;
    
    vector<int> eraseindex2;
   	 
    for (Int_t i = 0; i<int(rof2->size()); i++) {
    roftemp2.at(i).setDataRef(digitou->size()+rof2->at(i).getFirstIdx()-counterErase, rof2->at(i).getNEntries());
    
    roftemp2.at(i).setBCData(rof->at(0).getBCData()+rof2->at(i).getBCData());
    
    	 for (Int_t j = int(rof2->at(i).getFirstIdx()); j<int(rof2->at(i).getLastIdx()); j++) {
	    	 for (Int_t k = j+1; k<int(rof2->at(i).getLastIdx()+1); k++) {
	    		if(digitou2->at(j).getPadID()==digitou2->at(k).getPadID() and digitou2->at(j).getTime()==digitou2->at(k).getTime()){
	    			//cout << "Sim - ROF " << i << ", digits " << j << " and " << k << " : Pad = " << digitou2->at(j).getPadID() << ", TF = " << digitou2->at(j).getTime() << ", ADC = " << digitou2->at(j).getADC() << " and " << digitou2->at(k).getADC() << endl;
	    			digitoutemp2.at(k-counterErase).setADC(digitou2->at(j).getADC()+digitou2->at(k).getADC());
	    			digitoutemp2.erase(digitoutemp2.begin() + j - counterErase);
	    			roftemp2.at(i).setDataRef(roftemp2.at(i).getFirstIdx(), roftemp2.at(i).getNEntries()-1);
	    			eraseindex2.push_back(j);
	    			counterErase++;
	    			break;

    			}
    		}
    	}
    }
    
    for (Int_t i = 0; i<int(digitou2->size()); i++) {
    	if ( find(eraseindex2.begin(), eraseindex2.end(), i) != eraseindex2.end() ){
    	 	continue;
    	 }
    	 else {
    	 	MCLabtemp.addElement(MCLabtemp.getNElements(), MCLab2->getElement(i));
    	 }
    }
    
    digitou2 = &digitoutemp2;
    rof2 = &roftemp2;
    
    digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    digitouAll = digitou;
    rof->insert( rof->end(), rof2->begin(), rof2->end() );
    rofAll = rof;
    MCLabAll = &MCLabtemp;
    
    treeOutput->Fill();
    
    hfile.cd();
    treeOutput->Write();
    hfile.Close();
    
    return;
}
