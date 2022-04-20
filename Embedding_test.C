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
using namespace o2::mch;

void Embedding_test(const char *filename = "toto.root", const char *filename2 = "mchdigits.root")
{
//______________________
    //Data extraction
    int entries_number = 0;
    auto TFtime = 0;
    vector<Digit>* digitou = 0;
    vector<Digit>* digitou2 = 0;
    vector<ROFRecord>* rof = 0;
    vector<ROFRecord>* rof2 = 0;
    vector<Digit>* digitouAll = 0;
    vector<ROFRecord>* rofAll = 0;
    TFile hfile("treeEmbed.root","RECREATE");
    TTree *treeOutput = new TTree("o2Embed","treeOutput");
    auto branchDig = treeOutput->Branch("digits", &digitouAll);
    auto branchRof = treeOutput->Branch("rofs", &rofAll);
    TH1F *hTF   = new TH1F("hTF","TF distribution",100,0,1200000);
    TH1F *hTF2   = new TH1F("hTF2","TF distribution 2",100,0,10000);

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
    tree->SetBranchAddress("digits",&digitou);
    tree->SetBranchAddress("rofs",&rof);
    tree->GetEntry(0);
    
    TFile *file2 = TFile::Open(filename2,"read");
    TTree *tree2= (TTree*) file2->Get("o2sim");
    tree2->SetBranchAddress("MCHDigit",&digitou2);
    tree2->SetBranchAddress("MCHROFRecords",&rof2);
    tree2->GetEntry(0);
    
    for (Int_t i = 0; i<int(digitou->size()); i++) {
    	 for (Int_t j = 0; j<int(digitou2->size()); j++) {
    		if(digitou->at(i).getPadID()==digitou2->at(j).getPadID() and digitou->at(i).getTime()==digitou2->at(j).getTime()){
    			cout << "digits " << i << " and " << j << " : Pad = " << digitou->at(i).getADC() << ", TF = " << digitou->at(i).getTime() << ", ADC = " << digitou->at(i).getADC() << " and " << digitou2->at(j).getADC() << endl;
    			digitou->at(i).setADC(digitou->at(i).getADC() + digitou2->at(j).getADC());
    			digitou2->erase(digitou2->begin() + j);
    		}
    	}
    }
    
    digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    digitouAll = digitou;
    rof->insert( rof->end(), rof2->begin(), rof2->end() );
    rofAll = rof;
    treeOutput->Fill();
    
    hfile.cd();
    treeOutput->Write();
    hfile.Close();
    
    return;
    
    //TCanvas * cTF = new TCanvas("cTF","Time Frame time");
    //hTF->Draw();
    //TCanvas * cTF2 = new TCanvas("cTF2","Time Frame time 2");
    //hTF2->Draw();
    
}
