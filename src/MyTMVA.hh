#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TKey.h"
#include "TBranch.h"
//-----------------------------------------
#include "fastjet/PseudoJet.hh"

#include "ClassesLinkDef.h"
#include "DelphesClasses.h"
#include "DelphesFactory.h"
#include "DelphesFormula.h"
#include "DelphesHepMCReader.h"
#include "DelphesLHEFReader.h"
#include "DelphesModule.h"
#include "DelphesStream.h"
#include "SortableObject.h"
//----------------------------------------
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Config.h"
#include "TMVA/DataLoader.h"

#include "MyDebug.hh"

using namespace std;


class MyTMVA: public virtual MyDebug{
public:
    //Constructor and Destructor
    MyTMVA(string category_name, string file_label): _category_name(category_name), _file_label(file_label) {;}
    ~MyTMVA() {;}
    
    //Run Functions
    void run_TMVA();
    
private:
    string  _category_name;
    string  _file_label;
};


void MyTMVA::run_TMVA()
{
    cout << "--info: run TMVA"<<endl;
    
    bool batchMode(false);
    bool useDefaultMethods(true);
    
    // --------------------------------------------------------------------------------------------------
    // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
    TString outfileName =  "files/TMVA_"+_file_label+"_"+_category_name+".root";
    TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
    
    // Create the factory object.
    TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "!V:Silent:Color:!DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
    TMVA::DataLoader *loader=new TMVA::DataLoader("dataset");
    TString dirname = "files/weights_"+_category_name;
    //(TMVA::gConfig().GetIONames()).fWeightFileDir = dirname;
    cout << "        save weight file in directory " << dirname<<endl;
    
    // Define the input variables that shall be used for the MVA training
    TString fname = "files/observables_Background_"+_category_name+".root";
    TFile *inputvar = TFile::Open( fname );
    cout << "--info: Reading list of variables from file: " << inputvar->GetName() << endl;
    
    // Apply additional cuts. // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1"
    TCut mycuts = "";
    TCut mycutb = "";
    
    TIter next(inputvar->GetListOfKeys());
    TKey *key(0);
    while ((key = (TKey*)next()))
    {
        if (!TString(key->GetName()).EqualTo("Tree")) continue;
        if (!gROOT->GetClass(key->GetClassName())->InheritsFrom("TTree")) continue;
        
        TTree* mDir = (TTree*)key->ReadObj();
        TIter keyIt(mDir->GetListOfBranches());
        TBranch *branch;
        
        while ((branch = (TBranch*)keyIt()))
        {
            if (!TString(branch->GetName()).BeginsWith("var_")) continue;
            if (debug()>1) cout << "        -- debug: found variable " << TString(branch->GetName()) << endl;
            loader->AddVariable( TString(branch->GetName()), 'F' );
            //mycuts += (TString(branch->GetName()) + " > 0");
            //mycutb += (TString(branch->GetName()) + " > 0");
        }
    }
    loader->AddSpectator( "weight", 'F' );
    loader->AddSpectator( "type", 'F' );
    loader->AddSpectator( "category", 'F' );
    inputvar->Close();
    
    // Read training and test data
    TString fnamebg = "files/observables_Background_"+_category_name+".root";
    TFile *inputbg = TFile::Open( fnamebg );
    cout << "--info: Using input file for Background: " << inputbg->GetName() << endl;

    TString fnamesig = "files/observables_"+_file_label+"_"+_category_name+".root";
    TFile *inputsig = TFile::Open( fnamesig );
    cout << "--info: Using input file for Signal: " << inputsig->GetName() << endl;
    
    // --- Register the training and test trees
    TTree *background     = (TTree*)inputbg->Get("Tree");
    TTree *signal         = (TTree*)inputsig->Get("Tree");
    
    // You can add an arbitrary number of signal or background trees
    loader->AddBackgroundTree( background    );
    loader->AddSignalTree    ( signal        );
    
    // Set individual event weights (the variables must exist in the original TTree)
    loader->SetBackgroundWeightExpression("weight");
    loader->SetSignalWeightExpression    ("weight");
    
    // Tell the factory how to use the training and testing events
    cout << "--info: prepare BDT" << endl;
    //loader->PrepareTrainingAndTestTree(mycuts, mycutb,  "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:!V" );

    loader->PrepareTrainingAndTestTree("",  "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:!V" );

    // Boosted Decision Trees
    factory->BookMethod(loader, TMVA::Types::kBDT, "BDTG","!H:!V:NTrees=1000:MinNodeSize=1.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:UseRandomisedTrees:GradBaggingFraction=0.5:nCuts=20:MaxDepth=3" );
    
    //cout << mycuts << endl;
    
    cout << "        train BDT" << endl;
    // Train MVAs using the set of training events
    factory->TrainAllMethods();
    
    cout << "        test BDT" << endl;
    // ---- Evaluate all MVAs using the set of test events
    factory->TestAllMethods();
    cout << "        evaluate BDT" << endl;
    // ----- Evaluate and compare performance of all configured MVAs
    factory->EvaluateAllMethods();
    
    // --------------------------------------------------------------
    
    // Save the output
    outputFile->Close();
    
    cout << "--info: finished TMVA"<<endl;
    cout << "        wrote root file: " << outputFile->GetName() << endl;
    cout << endl;   // Clean up
    
    delete factory;
}

