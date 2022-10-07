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

void Embedding_Task(const char *filename = "mchdigits-data.root", const char *filename2 = "mchdigits-0.root")
{
//______________________
    //Variables declaration
    int entries_number = 0;
    auto TFtime = 0;
    vector<Digit>* digitou = 0;
    vector<Digit>* digitou2 = 0;
    vector<Digit> digitoutemp, digitoutemp2, digitstemp;
    vector<ROFRecord>* rof = 0;
    vector<ROFRecord>* rof2 = 0;
    vector<ROFRecord>* roftest = 0;
    vector<ROFRecord> roftemp, roftemp2, rofoutemp;
    MCTruthContainer<MCCompLabel> MCLabtemp;
    MCTruthContainer<MCCompLabel>* MCLab = 0;
    MCTruthContainer<MCCompLabel>* MCLab2 = 0;
    vector<Digit>* digitouAll = 0;
    vector<ROFRecord>* rofAll = 0;
    MCTruthContainer<o2::MCCompLabel>* MCLabAll = 0;
    int counterErase = 0; // number of erased digits
    vector<int> eraseindex;    
    vector<int> eraseindex2;
    int counterRof = 0;
    int counterEraselab = 0;
    int counterDigit = 0;
    bool toBeErased = false;
    TH1F *hTime   = new TH1F("hTime","Time distribution",10000,-200000,500000);
    //TH1F *hTime1   = new TH1F("hTime1","Time distribution for rof 1",10000,-200000,500000);
    //TH1F *hTime2   = new TH1F("hTime2","Time distribution for rof 2",10000,-200000,500000);
    TH1F *hTimeMC   = new TH1F("hTimeMC","MC time distribution",1000,-200,10000);
    //TH1F *hTimeDif   = new TH1F("hTimeDif","Time difference between first and last digit distribution",7,-2,5);
    //TH1F *hTimeDifRof   = new TH1F("hTimeDifRof","Time difference between consecutive rofs distribution",10100,-100,10000);
    //TH1F *hTimeDifDig   = new TH1F("hTimeDifDig","Time difference between consecutive digits distribution",2000,-1000,1000);
    
    // Output file 
    TFile hfile("mchdigits.root","RECREATE");
    TTree *treeOutput = new TTree("o2sim","treeOutput");
    auto branchDig = treeOutput->Branch("MCHDigit", &digitouAll); //Digit tree
    auto branchRof = treeOutput->Branch("MCHROFRecords", &rofAll); //ReadOut Frames tree
    auto branchMCLab = treeOutput->Branch("MCHMCLabels", &MCLabAll); //MC labels tree

    // Real data input extraction
    TFile *file = TFile::Open(filename,"read");
    if(!file){cout << "Error :: The data file is not found, please check"<< endl;return;}
    TTree *tree= (TTree*) file->Get("o2sim");
    if(!tree){	cout << "Error :: The data Tree is not found, please check"<< endl; return;}
    tree->SetBranchAddress("MCHDigit",&digitou);
    tree->SetBranchAddress("MCHROFRecords",&rof);
    
    //Get real data input file entries
    tree->GetEntry(0);
    entries_number = tree->GetEntries();
    digitstemp = *digitou;
    roftemp = *rof;
    MCLab = 0;
    //for (Int_t n = 1; n<entries_number; n++) { // for loop over entries filling digit and rof arrays
    //  	tree->GetEntry(n);
    //  	digitstemp.insert( digitstemp.end(), digitou->begin(), digitou->end() );
    //  	roftemp.insert( roftemp.end(), rof->begin(), rof->end() );
    //}
    digitou = &digitstemp;
    rof = &roftemp;
    
    digitoutemp = *digitou;
    rofoutemp = *rof;
    for (Int_t i = 0; i<int(rof->size()); i++) { // for loop over entries counting the rofs and ordering their parameters
   	 rofoutemp.at(i).setDataRef(counterRof, rof->at(i).getNEntries());
   	 counterRof += rof->at(i).getNEntries();
   	 //cout << rof->at(i).getBCData() << endl;
    }
    rof = &rofoutemp;
    
    for (Int_t u = 0; u<int(rof->size()); u++) {
    	//hTimeDif->Fill(digitou->at(rof->at(u).getLastIdx()).getTime()-digitou->at(rof->at(u).getFirstIdx()).getTime());
    	//hTimeDifRof->Fill(digitou->at(rof->at(u+1).getFirstIdx()).getTime()-digitou->at(rof->at(u).getLastIdx()).getTime());
    	for (Int_t i = rof->at(u).getFirstIdx(); i<int(rof->at(u).getLastIdx()); i++) {
	    	//cout << "Data - rof " << u << ", digit time : " << digitou->at(i).getTime() << endl;
	    	hTime->Fill(digitou->at(i).getTime());
	}
    }
    //for (Int_t i = rof->at(10000).getFirstIdx(); i<int(rof->at(10000).getLastIdx()); i++) {
    //	    	hTime1->Fill(digitou->at(i).getTime());
    //}
    //for (Int_t i = rof->at(20000).getFirstIdx(); i<int(rof->at(20000).getLastIdx()); i++) {
    //	    	hTime2->Fill(digitou->at(i).getTime());
    //}
    //for (Int_t i = 0; i<int(digitou->size())-1; i++) {
    //	    	hTimeDifDig->Fill(digitou->at(i+1).getTime()-digitou->at(i).getTime());
    //}
	
    // for loop over digits to add null entries for MClabels (none in real data)
    for (Int_t i = 0; i<int(digitou->size()); i++) {
    	MCLabtemp.addElement(MCLabtemp.getNElements(), 0);
    }
    digitou = &digitoutemp;
    rof = &rofoutemp;
    
    // MC input extraction
    TFile *file2 = TFile::Open(filename2,"read");
    if(!file2){cout << "Error :: The MC file is not found, please check"<< endl;return;}
    TTree *tree2= (TTree*) file2->Get("o2sim");
    if(!tree2){cout << "Error :: The MC Tree is not found, please check"<< endl; return;}
    tree2->SetBranchAddress("MCHDigit",&digitou2);
    tree2->SetBranchAddress("MCHROFRecords",&rof2);
    tree2->SetBranchAddress("MCHMCLabels",&MCLab2);
    
    //Get MC data input file entries
    counterErase = 0;
    tree2->GetEntry(0);
    //digitoutemp2 = *digitou2;
    //roftemp2 = *rof2;
    
    for (Int_t u = 0; u<int(rof2->size()); u++) {
    	for (Int_t i = rof2->at(u).getFirstIdx(); i<int(rof2->at(u).getLastIdx()); i++) {
    	    	hTimeMC->Fill(digitou2->at(i).getTime());
    	}
    }
    
    	//for (Int_t j = int(rof2->at(i).getFirstIdx()); j<int(rof2->at(i).getLastIdx()); j++) {
	//    	for (Int_t k = j+1; k<int(rof2->at(i).getLastIdx()+1); k++) {
	//    		if(digitou2->at(j).getPadID()==digitou2->at(k).getPadID() and digitou2->at(j).getTime()==digitou2->at(k).getTime()){
	    			//cout << "MC - ROF " << i << ", digits " << j << " and " << k << " : Pad = " << digitou2->at(j).getPadID() << ", TF = " << digitou2->at(j).getTime() << ", ADC = " << digitou2->at(j).getADC() << " and " << digitou2->at(k).getADC() << endl;
	//    			digitoutemp2.at(k-counterErase).setADC(digitou2->at(j).getADC()+digitou2->at(k).getADC());
	//    			digitoutemp2.erase(digitoutemp2.begin() + j - counterErase);
	//    			roftemp2.at(i).setDataRef(roftemp2.at(i).getFirstIdx(), roftemp2.at(i).getNEntries()-1);
	//    			eraseindex2.push_back(j);
	//    			counterErase++;
	//    			break;
    	//		}
    	//	}
    	//}
    //}
    
    int u= 0;
    int jtemp = 0;
    counterDigit = 0;
    //for loop to locate the real data rof and location where MC digits should be merged
    for (Int_t i = 0; i<int(digitou2->size()); i++) {
    	while (digitou2->at(i).getTime() > digitou->at(rof->at(u).getFirstIdx()).getTime()){
    		//cout << "rof : " << u << " : Time " <<  digitou2->at(i).getTime() << "/" << digitou->at(rof->at(u).getFirstIdx()).getTime() << endl;
    		roftemp.at(u).setDataRef(rof->at(u).getFirstIdx() + counterDigit, rof->at(u).getNEntries());
    		u++;
    	}
    	int j = rof->at(u-1).getFirstIdx();
    	while (digitou2->at(i).getTime() > digitou->at(j).getTime()){
    		//cout << "digit : " << j << " : Time " <<  digitou2->at(i).getTime() << "/" << digitou->at(j).getTime() << endl;
    		j++;
    	}
    	toBeErased = false;
	jtemp = j;
	while (digitou2->at(i).getTime() == digitou->at(jtemp).getTime() and !toBeErased){
		if (digitou2->at(i).getPadID() == digitou->at(jtemp).getPadID()){
			digitstemp.at(jtemp + counterDigit).setADC(digitou2->at(i).getADC()+digitou->at(jtemp).getADC());
			toBeErased = true;
			cout << " Same Pad ID and timing " << endl;
		}
		jtemp++;
	}
	if (!toBeErased) {
    		digitstemp.insert(digitstemp.begin() + j-1 + counterDigit, digitou2->at(i));
    		while (int(MCLabtemp.getNElements()) < j-1 + counterDigit) {
    			MCLabtemp.addElement(MCLabtemp.getNElements(), 0);
    		}
    		MCLabtemp.addElement(MCLabtemp.getNElements(), MCLab2->getElement(i));
    		counterDigit++;
    		roftemp.at(u-1).setDataRef(rof->at(u-1).getFirstIdx(), rof->at(u-1).getNEntries() + 1);
    	}
    	cout << "rof : " << u-1 << " - Digit " << i << "/" << digitou2->size() << " : Time " << digitou->at(j-1).getTime() << "/" << digitou2->at(i).getTime() << "/" << digitou->at(j).getTime() << endl;
    }
    
    digitou2 = &digitoutemp2;
    rof2 = &roftemp2;
    
    // Add the real data and MC rofs, digits and MC labels
    //digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    digitouAll = &digitstemp;
    //rof->insert( rof->end(), rof2->begin(), rof2->end() );
    rofAll = &roftemp;
    MCLabAll = &MCLabtemp;
    
    // Fill tree
    treeOutput->Fill();
 
    // Write tree in output file
    hfile.cd();
    treeOutput->Write();
    hfile.Close();
    
    TCanvas * cTime = new TCanvas("cTime","Time Distribution");
    hTime->Draw();
    TCanvas * cTimeMC = new TCanvas("cTimeMC","MC Time Distribution");
    hTimeMC->Draw();
    //TCanvas * cTime1 = new TCanvas("cTime1","Time Distribution for rof 1");
    //hTime1->Draw();
    //TCanvas * cTime2 = new TCanvas("cTime2","Time Distribution for rof 2");
    //hTime2->Draw();
    //TCanvas * cTimeDif = new TCanvas("cTimeDif","Time difference Distribution between first and last digit");
    //hTimeDif->Draw();
    //TCanvas * cTimeDifRof = new TCanvas("cTimeDifRof","Time difference Distribution between two consecutive rofs");
    //hTimeDifRof->Draw();
    //TCanvas * cTimeDifDig = new TCanvas("cTimeDifDig","Time difference Distribution between two consecutive digits");
    //hTimeDifDig->Draw();
    
    return;
}
