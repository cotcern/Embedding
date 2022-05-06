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

void Embedding_test(const char *filename = "toto.root", const char *filename2 = "mchdigits-0.root")
{
//______________________
    //Data extraction
    int entries_number = 0;
    auto TFtime = 0;
    vector<Digit>* digitou = 0;
    vector<Digit>* digitou2 = 0;
    vector<int> v1;
    vector<ROFRecord>* rof = 0;
    vector<ROFRecord>* rof2 = 0;
    o2::dataformats::MCTruthContainer<o2::MCCompLabel>* MCLab = 0;
    const o2::dataformats::MCTruthContainer<o2::MCCompLabel> MCLabtemp;
    o2::dataformats::MCTruthContainer<o2::MCCompLabel>* MCLab2 = 0;
    vector<Digit>* digitouAll = 0;
    vector<Digit>* digitouAlltemp = 0;
    vector<ROFRecord>* rofAll = 0;
    o2::dataformats::MCTruthContainer<o2::MCCompLabel>* MCLabAll = 0;
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
    tree->SetBranchAddress("digits",&digitou);
    tree->SetBranchAddress("rofs",&rof);
    //MCLab = 0;
    tree->GetEntry(0);
    
    TFile *file2 = TFile::Open(filename2,"read");
    TTree *tree2= (TTree*) file2->Get("o2sim");
    //tree2->Show(0);
    //tree2->Print();
    tree2->SetBranchAddress("MCHDigit",&digitou2);
    tree2->SetBranchAddress("MCHROFRecords",&rof2);
    tree2->SetBranchAddress("MCHMCLabels",&MCLab2);
    tree2->GetEntry(0);
    
    digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    digitouAll = digitou;
    //digitouAlltemp = digitouAll;
    int counter = 0;
    
    for (Int_t i = 0; i<int(digitouAll->size())-1; i++) {
    	v1.push_back(digitouAll->at(i).getPadID());
    	 for (Int_t j = i+1; j<int(digitouAll->size()); j++) {
    		if(digitouAll->at(i).getPadID()==digitouAll->at(j).getPadID() and digitouAll->at(i).getTime()==digitouAll->at(j).getTime()){
    			cout << "digits " << i << " and " << j << " : Pad = " << digitouAll->at(i).getPadID() << ", TF = " << digitouAll->at(i).getTime() << ", ADC = " << digitouAll->at(i).getADC() << " and " << digitouAll->at(j).getADC() << endl;
    			//cout << digitouAlltemp->size() << endl;
    			//digitouAll->at(j).setPadID(28671 + counter + 1);
    			//digitouAlltemp->erase(digitouAlltemp->begin() + j - counter);
    			counter++;
    		}
    	}
    }
    
    cout << *max_element(v1.begin(), v1.end()) << endl;
    cout << *min_element(v1.begin(), v1.end()) << endl;
    
    //digitouAll = digitouAlltemp;
    
    //digitou->insert( digitou->end(), digitou2->begin(), digitou2->end() );
    //digitouAll = digitou;
    rof->insert( rof->end(), rof2->begin(), rof2->end() );
    rofAll = rof;
    //MCLab->insert( MCLab->end(), MCLab2->begin(), MCLab2->end() );
    //MCLab2->mergeAtBack(MCLabtemp);
    MCLabAll = MCLab2;
    treeOutput->Fill();
    
    hfile.cd();
    treeOutput->Write();
    hfile.Close();
    
    return;
}
