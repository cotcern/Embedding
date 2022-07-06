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
    vector<int> v1;
    vector<ROFRecord>* rof = 0;
    vector<ROFRecord>* rof2 = 0;
    vector<ROFRecord> roftemp, roftemp2, rofoutemp;
    MCTruthContainer<MCCompLabel>* MCLab = 0;
    MCTruthContainer<MCCompLabel> MCLabtemp;
    MCTruthContainer<MCCompLabel>* MCLab2 = 0;
    vector<Digit>* digitouAll = 0;
    vector<ROFRecord>* rofAll = 0;
    MCTruthContainer<o2::MCCompLabel>* MCLabAll = 0;
    TFile hfile("treeEmbed.root","RECREATE");
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
    //tree->Show(0);
    //tree->Print();
    if(!tree)
    {
     	cout << "Error :: The Tree is not found, please check"<< endl;
     	return;
    }
    tree->SetBranchAddress("MCHDigit",&digitou);
    tree->SetBranchAddress("MCHROFRecords",&rof);
    //MCLab = 0;
    tree->GetEntry(0);
    entries_number = tree->GetEntries();
    digitstemp = *digitou;
    roftemp = *rof;
    MCLab = 0;
    for (Int_t n = 1; n<entries_number; n++) {
      	tree->GetEntry(n);
      	digitstemp.insert( digitstemp.end(), digitou->begin(), digitou->end() );
      	roftemp.insert( roftemp.end(), rof->begin(), rof->end() );
      	//for (Int_t m = 0; m<int(digitou->size()); m++) {
      		//MCLab->mergeAtBack(MCLabtemp);
      		//MCLabtemp.addElements(0,MCLabtemp2);
      		//cout << "here" << endl;
      	//}
      	//cout << roftemp.size() << endl;
      	//cout << roftemp.at(roftemp.size()-1).getFirstIdx() << "," << roftemp.at(roftemp.size()-1).getLastIdx() << endl;
    }
    digitou = &digitstemp;
    rof = &roftemp;
    TFile *file2 = TFile::Open(filename2,"read");
    TTree *tree2= (TTree*) file2->Get("o2sim");
    //tree2->Show(0);
    //tree2->Print();
    tree2->SetBranchAddress("MCHDigit",&digitou2);
    tree2->SetBranchAddress("MCHROFRecords",&rof2);
    tree2->SetBranchAddress("MCHMCLabels",&MCLab2);
    tree2->GetEntry(0);
    //MCLabtemp = *MCLab2;
    
    int counterErase = 0;
    int counterRof = 0;
    digitoutemp = *digitou;
    rofoutemp = *rof;
    
    for (Int_t i = 0; i<int(digitou->size()); i++) {
   	 MCLabtemp.addElement(i, 0);
    }
    
    for (Int_t i = 0; i<int(rof->size()); i++) {
   	 rofoutemp.at(i).setDataRef(counterRof, rof->at(i).getNEntries());
   	 counterRof += rof->at(i).getNEntries();
    }
    rof = &rofoutemp;
    
    for (Int_t i = 0; i<int(rof->size()); i++) {
   	 if (i%100 == 0) {
   	 	cout << "Data : " << rof->at(i)  << endl;
   	 }
    }

    for (Int_t i = 0; i<int(rof->size()); i++) {
    	rofoutemp.at(i).setDataRef(rof->at(i).getFirstIdx()-counterErase, rof->at(i).getNEntries());
    	for (Int_t j = int(rof->at(i).getFirstIdx()); j<int(rof->at(i).getLastIdx()); j++) {
    	 	for (Int_t k = j+1; k<int(rof->at(i).getLastIdx()+1); k++) {
    			if(digitou->at(j).getPadID()==digitou->at(k).getPadID() and digitou->at(j).getTime()==digitou->at(k).getTime()){
    				//cout << "Data - ROF " << i << ", digits " << j << " and " << k << " : Pad = " << digitou->at(j).getPadID() << ", TF = " << digitou->at(j).getTime() << ", ADC = " << digitou->at(j).getADC() << " and " << digitou->at(k).getADC() << endl;
    				digitoutemp.at(k-counterErase).setADC(digitou->at(j).getADC()+digitou->at(k).getADC());
    				digitoutemp.erase(digitoutemp.begin() + j - counterErase);
    				rofoutemp.at(i).setDataRef(rofoutemp.at(i).getFirstIdx(), rofoutemp.at(i).getNEntries()-1);
    				counterErase++;
    				break;
    			}
    		}
    	}
    }
    digitou = &digitoutemp;
    rof = &rofoutemp;
    
    cout << digitou->size() << endl;
    cout << MCLab2->getNElements() << " / " << digitou2->size() << "//" << MCLab2->getTruthArray().size()  << endl;
    
    counterErase = 0;
    digitoutemp2 = *digitou2;
    roftemp2 = *rof2;
    for (Int_t i = 0; i<int(rof2->size()); i++) {
   	 //cout << "Sim : " << rof2->at(i) << endl;
	}
   	 
    for (Int_t i = 0; i<int(rof2->size()); i++) {
    roftemp2.at(i).setDataRef(digitou->size()+rof2->at(i).getFirstIdx()-counterErase, rof2->at(i).getNEntries());
    //roftemp2.at(i).setBCData(rof->at(0).getBCData());
    	 for (Int_t j = int(rof2->at(i).getFirstIdx()); j<int(rof2->at(i).getLastIdx()); j++) {
	    	 for (Int_t k = j+1; k<int(rof2->at(i).getLastIdx()+1); k++) {
	    		if(digitou2->at(j).getPadID()==digitou2->at(k).getPadID() and digitou2->at(j).getTime()==digitou2->at(k).getTime()){
	    			//cout << "Sim - ROF " << i << ", digits " << j << " and " << k << " : Pad = " << digitou2->at(j).getPadID() << ", TF = " << digitou2->at(j).getTime() << ", ADC = " << digitou2->at(j).getADC() << " and " << digitou2->at(k).getADC() << endl;
	    			digitoutemp2.at(k-counterErase).setADC(digitou2->at(j).getADC()+digitou2->at(k).getADC());
	    			digitoutemp2.erase(digitoutemp2.begin() + j - counterErase);
	    			roftemp2.at(i).setDataRef(roftemp2.at(i).getFirstIdx(), roftemp2.at(i).getNEntries()-1);
	    			counterErase++;
	    			break;

    			}
    		}
    	}
    }
    
    for (Int_t i = 0; i<int(digitou2->size()); i++) {
    	cout << "here : " << MCLab2->getNElements() << " / " << digitou2->size() << " , " << MCLabtemp.getNElements() << " , " << MCLab2->getElement(i) << "//" << MCLab2->getTruthArray().at(i)  << endl;
    	//MCCompLabel MCLabtemp2(MCLab2->getLabels(i)[0]);
   	//MCLabtemp.addElement(i, 0);
    }
    
    cout << digitou->size() << " + " << digitou2->size() << " = " << digitou->size() + digitou2->size() << endl;
    digitou2 = &digitoutemp2;
    rof2 = &roftemp2;
    int n_1 = digitou->size();
    digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    digitouAll = digitou;
    cout << digitou->size() << endl;
    rof->insert( rof->end(), rof2->begin(), rof2->end() );
    rofAll = rof;
    
    //for (Int_t m = 0; m<n_1; m++) {
      	//cout << MCLab2->getIndexedSize() << "," << MCLab2->getNElements() << endl;
      	//MCLab2->mergeAtBack(MCLabtemp);
    //}
    
    for (Int_t i = 0; i<int(rof->size()); i++) {
   	 //cout << "Embed : " << rof->at(i) << endl;
   	 }
    MCLabAll = MCLab2;
    treeOutput->Fill();
    
    hfile.cd();
    treeOutput->Write();
    hfile.Close();
    
    return;
}
