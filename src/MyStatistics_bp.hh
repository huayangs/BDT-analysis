#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iomanip>
#include <tuple>

/*#include <TROOT.h>
 #include <TStyle.h>
 #include "TTree.h"
 #include "TApplication.h"
 #include "TString.h"
 #include "TFile.h"
 #include "TKey.h"
 #include "TDirectory.h"
 #include "TClass.h"
 #include "TCanvas.h"
 #include "TGraph.h"
 #include "TColor.h"
 #include "TH1.h"
 #include "TH2F.h"
 #include "TLegend.h"
 #include "THStack.h"*/

#include "MyDebug.hh"

using namespace std;

class MyStatistics: public virtual MyDebug{
public:
    //Constructor and Destructor
    MyStatistics(vector<string> categories, string file_label, double systerror): _categories(categories), _file_label(file_label), _systerror(systerror){};
    MyStatistics() {;}
    
    //interface
    void   run_statistics();
    tuple <double, double> get_significance(string category_name, double luminosity);
    void   write_significance(string category_name, int luminosity, double significance, double stbratio);
    
    //others
    vector<string> split(string str, char delimiter);
    
private:
    //input info
    double _systerror;
    vector<string>  _categories;
    string  _file_label;
};

vector<string> MyStatistics::split(string str, char delimiter)
{
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;
    
    while(getline(ss, tok, delimiter))
    {
        internal.push_back(tok);
    }
    
    return internal;
}

void MyStatistics::run_statistics()
{
    cout << "--info: Start Statistics Tool for process " << _file_label<<endl;
    
    int luminosity=3000;
    while (luminosity<=3000)
    {
        //double total_signifiance=0;
        for (int i=0; i<_categories.size(); i++)
        {
            string category_name = _categories[i];
            double significance, stbratio;
            tie(significance, stbratio) = get_significance(category_name,double(luminosity));
            //total_signifiance   += significance*significance;
            
            write_significance(category_name,luminosity,significance,stbratio);
        }
        //total_signifiance = sqrt(total_signifiance);
        //write_significance("combined",luminosity,total_signifiance);
        
        if (luminosity%3==0)
            luminosity = luminosity/3*10;
        else
            luminosity = luminosity*3;
    }
    cout << "        see results/summary/summary.txt for results"<< endl;
}



tuple <double, double> MyStatistics::get_significance(string category_name, double luminosity)
{
    TString fin = "files/TMVA_"+_file_label+"_"+category_name+".root";
    
    // define histogram
    double s[20000]={0};
    double b[20000]={0};
    
    // Open File
    if (debug()>7) cout << "        --debug: open TMVA file " << fin<<endl;
    TFile* file = TFile::Open( fin, "READ" );
    
    // --- Register the training and test trees
    TTree *tree     = (TTree*)file->Get("dataset/TestTree");
    
    float bdtout;
    float weight;
    float type;
    tree->SetBranchAddress("BDTG",&bdtout);
    tree->SetBranchAddress("type",&type);
    tree->SetBranchAddress("weight",&weight);

	cout <<"-------------" << bdtout << "---------------------" << endl;
    
    //Loop over all events
    int i=0;
    while (tree->GetEntry(i))
    {
        int bdt = int((bdtout+1.)*10000.);
        //fill all bins that have ibdt>bdt
        for(int ibdt =0;ibdt<bdt;ibdt++){
            if (type==0) s[ibdt]+=weight; else b[ibdt]+=weight;
        }
        i++;
    }
    
    //find max signifiance
    double significance =0, stbratio=0;
    for(int ibdt =0;ibdt<20000;ibdt++)
    {
        double bdt = double(ibdt)/10000.-1;
        double w;
        if(luminosity*s[ibdt]<3.0 || luminosity*b[ibdt]<3.0) break;
        if (b[ibdt]==0)
            w= luminosity*s[ibdt]/2.;
        else
            w= luminosity*s[ibdt]/(sqrt(luminosity*b[ibdt]) + _systerror*luminosity*b[ibdt] );
        if (w>significance) {
            significance = w;
            stbratio=s[ibdt]/b[ibdt];
        }
        
        if ((debug()>7)&&(bdt>0.8))
            cout << "BDTResponse, XS, XB, s/(sqrt(b)+e*b), Excl.Lim.: "<<bdt << ", " <<s[ibdt] << ", "<<b[ibdt] << ", "  << w << ", " << 10000.*2/w  << endl;
    }
    
    file->Close();
    return make_tuple(significance, stbratio);;
}


void MyStatistics::write_significance(string category_name, int luminosity, double significance, double stbratio)
{
    vector<string> words= this->split(_file_label,'_');
    
    string filename = "results/summary/summary.txt";
    ofstream summaryfile;
    summaryfile.open(filename.c_str(),std::ios_base::app);
    summaryfile  << setw(20) << category_name;
    summaryfile  << setw(20) << words[1];
    summaryfile  << setw(20) << words[3];
    summaryfile  << setw(20) << words[5];
    summaryfile  << setw(20) << words[7];
    summaryfile  << setw(20) << words[9];
    summaryfile  << setw(20) << words[11];
    //summaryfile  << setw(20) << luminosity;
    //summaryfile  << setprecision(5) << setw(10) << 2./significance;
    summaryfile  << setprecision(5) << setw(10) << significance;
    summaryfile  << setprecision(5) << setw(10) << stbratio << endl;
    summaryfile.close();
}

