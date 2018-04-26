#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <TROOT.h>
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

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

#include "ExRootAnalysis/ExRootTreeReader.h"

#include "MyDebug.hh"


using namespace std;

class MyCreate_A_HZ_bbll: public MyDebug{
public:
    //Constructor and Destructor
    MyCreate_A_HZ_bbll(int mode, string file_label, vector<string> file_name, vector<double> crosssection, vector<int> nfiles , vector<string> directory): _mode(mode), _file_label(file_label), _file_name(file_name), _crosssection(crosssection), _nfiles(nfiles), _directory(directory), _this_cathegorie("A_HZ_bbll") {};
    ~MyCreate_A_HZ_bbll() {;}
    
    //Run Functions
    void Create();
    
private:
    //process information
    int             _mode;
    string          _file_label;
    vector<string>  _file_name;
    vector<double>  _crosssection;
    vector<int>     _nfiles;
    vector<string>  _directory;
    
    string          _this_cathegorie;
};




void MyCreate_A_HZ_bbll::Create()
{
    cout << "--info: create ROOT-file for TMVA"<<endl;
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++
    //define vars
    
    double mll, mbb, mllbb, ptl1, ptl2, ptb1, ptb2, etmiss, eventht, weight, type;
    double category = 1;
    
    // create a new Root file
    string filename="files/observables_"+_file_label+"_"+_this_cathegorie+".root";
    //cout << filename << endl;
    //string path_to_file;
    //if (_mode==1)
    //{
    //    path_to_file = "/home/fkling/work/MG5_aMC_v2_3_2_2/Proc-Exotic/Signal_HZ_bbll/Results/";
    //    _crosssection[0]*=0.0673;
    //}
    //if (_mode==0)
    //{
    //    path_to_file = "/home/fkling/work/MG5_aMC_v2_3_2_2/Bkg-Exotic/";
    //}
    TFile *top = new TFile(filename.c_str(),"recreate");
    //cout << filename << "1" << endl;
    
    //Signal
    TTree  tree("Tree","");
    tree.Branch("var_mll",&mll, "var_mll/d");
    tree.Branch("var_mbb",&mbb, "var_mbb/d");
    tree.Branch("var_mllbb",&mllbb, "var_mllbb/d");
    tree.Branch("var_ptl1",&ptl1, "var_ptl1/d");
    tree.Branch("var_ptl2",&ptl2, "var_ptl2/d");
    tree.Branch("var_ptb1",&ptb1, "var_ptb1/d");
    tree.Branch("var_ptb2",&ptb2, "var_ptb2/d");
    tree.Branch("var_eventht",&eventht, "var_eventht/d");
    tree.Branch("var_etmiss",&etmiss, "var_etmiss/d");
    tree.Branch("weight",&weight, "weight/d");
    tree.Branch("type",&type, "type/d");
    tree.Branch("category",&category, "category/d");
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++
    //Load Input
    
    cout << "--info: initialize files"<<endl;
    
    //for weight of type
    vector<double> proc_nevent;
    
    //for file info
    vector<string> filename_in;
    vector<int> type_in;
    vector<double> proc_crosssection;
    double sigma, nevent; int nmax; string file_in;
    
    for (int i=0; i<_file_name.size(); i++)
    {
        proc_nevent.push_back(0);
        
        if (this->debug()>12) cout << "        --debug: type "<<i<<": "<<_file_name[i] <<endl;
        
        string pathname = _directory[i];
        vector<string> tmp_filename_in = this->GetFilesInDir(pathname,_nfiles[i]);
        for (int j=0; j<tmp_filename_in.size(); j++)
        {
            string file_name = pathname +"/"+ tmp_filename_in[j];
            if (this->debug()>12) cout << "        --debug: initialize file "<< file_name <<endl;
            
            nevent = this->GetEventNumber(file_name);
            proc_nevent[i]+=nevent;
            if (this->debug()>12) cout << "                 number of event in file: "<< nevent <<endl;
            
            filename_in.push_back(file_name.c_str());
            if (_mode==1) type_in.push_back(0); else type_in.push_back(i+1);
            proc_crosssection.push_back(_crosssection[i]);
            if (this->debug()>12) cout << "                 cross section: "<< _crosssection[i] << " fb" <<endl;
        }
    }
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++
    // Loop over files
    for (int j=0; j<filename_in.size();j++)
    {
        cout << "        Load file "<< filename_in[j] << " ("<< j+1 << "/" << filename_in.size() << " , " <<  double(j)/double(filename_in.size())*100. << "%)" << endl;
        type    = type_in[j];
        //only half the events will be used for tsting, the rest is for training and is not used for calulating significance. Therefore there is a factor of 2 in the next line
        if (_mode==1) weight  = 2*proc_crosssection[j]/proc_nevent[0]; else weight  = 2*proc_crosssection[j]/proc_nevent[type-1];
        if (_mode==1)
        {if (this->debug()>12) cout << "                 xs, event number, weight: "<<proc_crosssection[j]<<" fb, "<<proc_nevent[0]<< ", " << weight<<" fb" <<endl;}
        else
        {if (this->debug()>12) cout << "                 xs, event number, weight: "<<proc_crosssection[j]<<" fb, "<<proc_nevent[type-1]<< ", " << weight<<" fb" <<endl;}
        
        TChain chain("Delphes");
        chain.Add(filename_in[j].c_str());
        
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //Input Reader
        ExRootTreeReader *treeReaderDelphes = new ExRootTreeReader(&chain);
        
        Long64_t numberOfEntries = treeReaderDelphes->GetEntries();
        
        TClonesArray *PHOTO  = treeReaderDelphes->UseBranch("Photon");
        TClonesArray *ELEC   = treeReaderDelphes->UseBranch("Electron");
        TClonesArray *MUON   = treeReaderDelphes->UseBranch("Muon");
        TClonesArray *JET    = treeReaderDelphes->UseBranch("Jet");
        TClonesArray *ETMIS  = treeReaderDelphes->UseBranch("MissingET");
        TClonesArray *HT     = treeReaderDelphes->UseBranch("ScalarHT");
        TClonesArray *EVENT  = treeReaderDelphes->UseBranch("Event");
        
        //general information
        double E,Px,Py,Pz;
        double PT,Eta,Phi,M,Event_Weight,Charge;
        bool Btag,Tautag;
        int passcount=0;
        
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // loop over files
        for (Int_t entry=0; (entry<numberOfEntries) ; entry++)
        {
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //access information of reconstructed information
            treeReaderDelphes->ReadEntry(entry);
            if (entry%10000==0)
            {
                cout << "        --process events ";
                cout << setprecision(4) << setw(10) << entry << "/";
                cout << setprecision(4) << setw(10) << numberOfEntries << " , ";
                cout << setprecision(4) << setw(10) << Double_t(entry)/Double_t(numberOfEntries)*100. << "%" << endl;
            }
            
            if (debug()>13) cout << "        --debug: read event number "<< entry <<endl;
            
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //Define Delphes input
            
            vector<fastjet::PseudoJet> del_eflow;
            vector<double> pos_perp;
            
            vector<fastjet::PseudoJet> del_elec;
            vector<fastjet::PseudoJet> del_aelec;
            vector<fastjet::PseudoJet> del_muon;
            vector<fastjet::PseudoJet> del_amuon;
            vector<fastjet::PseudoJet> del_tau;
            vector<fastjet::PseudoJet> del_atau;
            vector<fastjet::PseudoJet> del_lep;
            
            vector<fastjet::PseudoJet> del_phot;
            vector<fastjet::PseudoJet> del_jet;
            vector<fastjet::PseudoJet> del_b;
            
            vector<fastjet::PseudoJet> del_met;
            double del_ht;
            
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //Get Delphes input
            if (debug()>13) cout << "                 get delphes input" <<endl;
            
            TIter itEvent((TCollection*)EVENT);
            LHEFEvent *event;
            itEvent.Reset();
            while( (event = (LHEFEvent*) itEvent.Next()) )
            {
                Event_Weight	= event->Weight;
            }
            
            TIter itElec((TCollection*)ELEC);
            Electron *elec;
            itElec.Reset();
            while( (elec = (Electron*) itElec.Next()) )
            {
                Eta	= elec->Eta;
                Phi	= elec->Phi;
                PT	= elec->PT;
                
                Charge  = elec->Charge;
                
                E  = PT * cosh(Eta);
                Px = PT * cos(Phi);
                Py = PT * sin(Phi);
                Pz = PT * sinh(Eta);
                fastjet::PseudoJet pseudojet_tmp(Px,Py,Pz,E);
                if (Charge<0) del_elec.push_back(pseudojet_tmp); else del_aelec.push_back(pseudojet_tmp);
                del_lep.push_back(pseudojet_tmp);
            }
            del_elec=sorted_by_pt(del_elec);
            del_aelec=sorted_by_pt(del_aelec);
            
            TIter itMuon((TCollection*)MUON);
            Muon *muon;
            itMuon.Reset();
            while( (muon = (Muon*) itMuon.Next()) )
            {
                Eta	= muon->Eta;
                Phi	= muon->Phi;
                PT	= muon->PT;
                
                Charge  = muon->Charge;
                
                E  = PT * cosh(Eta);
                Px = PT * cos(Phi);
                Py = PT * sin(Phi);
                Pz = PT * sinh(Eta);
                
                fastjet::PseudoJet pseudojet_tmp(Px,Py,Pz,E);
                if (Charge<0) del_muon.push_back(pseudojet_tmp); else del_amuon.push_back(pseudojet_tmp);
                del_lep.push_back(pseudojet_tmp);
            }
            del_muon=sorted_by_pt(del_muon);
            del_amuon=sorted_by_pt(del_amuon);
            del_lep=sorted_by_pt(del_lep);
            
            TIter itJet((TCollection*)JET);
            Jet *jet;
            itJet.Reset();
            while( (jet = (Jet*) itJet.Next()) )
            {
                Eta	= jet->Eta;
                Phi	= jet->Phi;
                PT	= jet->PT;
                M  	= jet->Mass;
                
                Btag 	= jet->BTag & (1 << 0) ;
                Tautag	= jet->TauTag;
                Charge  = jet->Charge;
                
                Px = PT * cos(Phi);
                Py = PT * sin(Phi);
                Pz = PT * sinh(Eta);
                E  = sqrt(PT*cosh(Eta)*PT*cosh(Eta) + M*M);
                
                fastjet::PseudoJet pseudojet_tmp(Px,Py,Pz,E);
                if ((Btag)&&(!Tautag)) del_b.push_back(pseudojet_tmp);
                if ((Tautag)&&(Charge>0)) del_atau.push_back(pseudojet_tmp);
                if ((Tautag)&&(Charge<0)) del_tau.push_back(pseudojet_tmp);
                if ((!Tautag)&&(!Btag)) del_jet.push_back(pseudojet_tmp);
            }
            del_b=sorted_by_pt(del_b);
            del_tau=sorted_by_pt(del_tau);
            del_atau=sorted_by_pt(del_atau);
            del_jet=sorted_by_pt(del_jet);
            
            TIter itGam((TCollection*)PHOTO);
            Photon *gam;
            itGam.Reset();
            while( (gam = (Photon*) itGam.Next()) )
            {
                Eta	= gam->Eta;
                Phi	= gam->Phi;
                PT	= gam->PT;
                E  	= gam->E;
                
                Px = PT * cos(Phi);
                Py = PT * sin(Phi);
                Pz = PT * sinh(Eta);
                fastjet::PseudoJet pseudojet_tmp(Px,Py,Pz,E);
                del_phot.push_back(pseudojet_tmp);
            }
            del_phot=sorted_by_pt(del_phot);
            
            TIter itmis((TCollection*)ETMIS);
            MissingET *met;
            itmis.Reset();
            while( (met = (MissingET*) itmis.Next()) )
            {
                Phi    	= met->Phi;
                E       = met->MET;
                Px      = E * cos(Phi);
                Py      = E * sin(Phi);
                
                fastjet::PseudoJet pseudojet_tmp(Px,Py,0.0,E);
                del_met.push_back(pseudojet_tmp);
            }
            del_met=sorted_by_pt(del_met);
            
            TIter itht((TCollection*)HT);
            ScalarHT *ht;
            itht.Reset();
            while( (ht = (ScalarHT*) itht.Next()) )
            {
                del_ht      = ht->HT;
            }
            
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //TRIGGER AND SECTION CUTS
            if (debug()>13) cout << "                 lepton selection" <<endl;
            if (debug()>13) cout << "                   number of electrons:      "<< del_elec.size()<<endl;
            if (debug()>13) cout << "                   number of anti-electrons: "<< del_aelec.size()<<endl;
            if (debug()>13) cout << "                   number of muons:          "<< del_muon.size()<<endl;
            if (debug()>13) cout << "                   number of antimuons:      "<< del_amuon.size()<<endl;
            if (debug()>13) cout << "                   number of bs:             "<< del_b.size()<<endl;
            if (debug()>13) cout << "                 pass trigger:               "<< pass_trigger(del_lep, del_met)<<endl;
            
            //trigger
            if (!pass_trigger(del_lep, del_met)) continue;
            
            //variables
            fastjet::PseudoJet del_l1;
            fastjet::PseudoJet del_l2;
            fastjet::PseudoJet del_b1;
            fastjet::PseudoJet del_b2;
            bool pass=false;
            
            //b,tau veto
            if (del_b.size()<2) continue;
            if (del_tau.size()>0) continue;
            if (del_atau.size()>0) continue;
            
            //muons
            if ((del_muon.size()==1)and(del_amuon.size()==1)and(del_elec.size()==0)and(del_aelec.size()==0))
            {
                del_l1=del_amuon[0];
                del_l2=del_muon[0];
                del_b1=del_b[0];
                del_b2=del_b[1];
                pass=true;
            }
            
            //electrons
            if ((del_muon.size()==0)and(del_amuon.size()==0)and(del_elec.size()==1)and(del_aelec.size()==1))
            {
                del_l1=del_aelec[0];
                del_l2=del_elec[0];
                del_b1=del_b[0];
                del_b2=del_b[1];
                pass=true;
            }
            
            
            //Continue when not enough leptons
            if (!pass) continue;
            
            //CALCULATE OBSERVABLES
            mll  = (del_l1+del_l2).m();
            mbb  = (del_b1+del_b2).m();
            mllbb= (del_l1+del_l2+del_b1+del_b2).m();
            ptl1 = del_l1.pt();
            ptl2 = del_l2.pt();
            ptb1 = del_b1.pt();
            ptb2 = del_b2.pt();
            etmiss = del_met[0].pt();
            eventht = del_ht;
            
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            if (debug()>13) cout << "                 fill tree" <<endl;
            //Fill Tree
            tree.Fill();
            passcount++;
            
        } // end loop over events
        
        cout << "        found "<<passcount<<" Events that pass '"<<_this_cathegorie<<"' selection criteria"<<endl;
        
    } // end loop over files
    
    // save histogram hierarchy in the file
    top->Write();
    top->Close();
    cout << "--info: finished writing ROOT-file"<<endl;
    
    cout << endl;
    
    //delete
    delete top;
    
}












