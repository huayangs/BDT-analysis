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

#include "MyCreate_A_HZ_bbll.hh"
#include "MyCreate_A_HZ_tatall.hh"
#include "MyCreate_A_HpW_tblv.hh"

using namespace std;

class MyCreate: public MyDebug{
public:
    //Constructor and Destructor
    MyCreate(string category_name, int mode, string file_label, vector<string> file_name, vector<double> crosssection, vector<int> nfiles, vector<string> directory): _category_name(category_name), _mode(mode), _file_label(file_label), _file_name(file_name), _crosssection(crosssection), _nfiles(nfiles) ,_directory(directory) {};
    ~MyCreate() {;}
    
    //Run Functions
    void run_create();
    
private:
    //process information
    string          _category_name;
    int             _mode;
    string	        _file_label;
    vector<string>  _file_name;
    vector<double>  _crosssection;
    vector<int>     _nfiles;
    vector<string>  _directory;
};

void MyCreate::run_create()
{
    if (_category_name=="A_HZ_bbll")
    {
        MyCreate_A_HZ_bbll * creater= new MyCreate_A_HZ_bbll(_mode, _file_label, _file_name, _crosssection, _nfiles,_directory);
        creater->set_debug_level(debug());
        creater->Create();
        delete creater;
    }
    
    if (_category_name=="A_HZ_tatall")
    {
        MyCreate_A_HZ_tatall * creater= new MyCreate_A_HZ_tatall(_mode, _file_label, _file_name, _crosssection, _nfiles,_directory);
        creater->set_debug_level(debug());
        creater->Create();
        delete creater;
    }
    
    if (_category_name=="A_HpW_tblv")
    {
        MyCreate_A_HpW_tblv * creater= new MyCreate_A_HpW_tblv(_mode, _file_label, _file_name, _crosssection, _nfiles,_directory);
        creater->set_debug_level(debug());
        creater->Create();
        delete creater;
    }
}












