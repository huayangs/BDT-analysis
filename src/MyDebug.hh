#ifndef MYDEBUG_HH
#define MYDEBUG_HH

#include <stdio.h>
#include <vector>
#include <dirent.h>

#include "TChain.h"
#include "fastjet/PseudoJet.hh"
#include "ExRootAnalysis/ExRootTreeReader.h"

using namespace std;

class MyDebug{
public:
    // Constructor and Destructor
    MyDebug():_debug_level(0) {};
    ~MyDebug()  {;}
    
    // set/get debug
    void        set_debug_level(int x) {_debug_level=x;}
    int         debug() {return _debug_level;}
    string      IntToString(int number) {stringstream ss; ss << number; return ss.str();}
    string      DoubleToString(double number) {stringstream ss; ss << number; return ss.str();}
    int         GetEventNumber(string file_name);
    double      GetSignalXS(string name);
    vector<string>  GetFilesInDir(string dirname,int n);
    int 	contains(vector<string> vec, string element);
    bool 	pass_trigger(vector<fastjet::PseudoJet> lep, vector<fastjet::PseudoJet> met);
    
private:
    // static variable containing the global debug level
    int         _debug_level;
};

bool MyDebug::pass_trigger(vector<fastjet::PseudoJet> lep, vector<fastjet::PseudoJet> met)
{
    bool output=false;
    if (_debug_level>13){
       	if (debug()>13) 	 	   cout << "                 trigger info:" <<endl;
       	if (debug()>13) {if (lep.size()>0) cout << "                   pT(l1)= "<< lep[0].pt()<<endl;}
        if (debug()>13) {if (lep.size()>1) cout << "                   pT(l2)= "<< lep[1].pt()<<endl;}
       	if (debug()>13) {if (met.size()>0) cout << "                   MET   = "<< met[0].pt()<<endl;}
       	if (debug()>13) {if ((lep.size()>0)&&(lep[0].pt()>30)) 			 cout << "                 pass trigger: single lepton"<<endl;}
        if (debug()>13) {if ((lep.size()>1)&&(lep[0].pt()>20)&&(lep[1].pt()>10)) cout << "                 pass trigger: two leptons"<<endl;}
       	if (debug()>13) {if ((met.size()>0)&&(met[0].pt()>10))			 cout << "                 pass trigger: MET"<<endl;}
    }
    if ((lep.size()>0)&&(lep[0].pt()>30)) output=true;
    if ((lep.size()>1)&&(lep[0].pt()>20)&&(lep[1].pt()>10)) output=true;
    if ((met.size()>0)&&(met[0].pt()>10)) output=true;
    return output;
}

int MyDebug::GetEventNumber(string file_name)
{
    TChain chain("Delphes");
    chain.Add(file_name.c_str());
    ExRootTreeReader *treeReaderDelphes = new ExRootTreeReader(&chain);
    return treeReaderDelphes->GetEntries();
}

vector<string> MyDebug::GetFilesInDir(string dirname, int n)
{
    vector<string> output;
    int count=0;
    
    if (_debug_level>5) cout << "        --debug: find files in directory " << dirname << endl;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dirname.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            string file= ent->d_name;
            if (file.find("root")!=std::string::npos)
            {
                if (_debug_level>5) cout << "              - found: " << file <<endl;
                output.push_back(file);
                count++;
                if (count==n) break;
            }
        }
        closedir (dir);
    }
    else
    {
        cout<< "--error: directory does not exist!"<<endl;
    }
    return output;
}

double MyDebug::GetSignalXS(string name)
{
    double output=0;
    
    string input_file="/home/fkling/work/MG5_aMC_v2_3_2_2/Proc-Blindspot/Signal_XX/Results/"+name+"/"+name+"_pythia_0.dat";
    ifstream infile(input_file.c_str());
    string line;
    for(int i = 0; infile.good(); i++)
    {
        getline(infile, line);
        istringstream iss(line);
        vector<string> cmd;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter<vector<string> >(cmd));
        
        if ((cmd.size()!=4)) continue;
        if ((cmd[0]=="Cross")&&(cmd[1]=="section")&&(cmd[2]=="(pb):")) output=atof(cmd[3].c_str());
    }
    if (output==0) cout << "--info: ERROR: couldn't read cross section from "<<input_file<<endl;
    return output;
}

int MyDebug::contains(vector<string> vec, string element)
{
    int output=-1;
    for (int i=0;i<vec.size();i++)
    {
        if (vec[i]==element) {output=i; break;}
    }
    return output;
}

#endif
