#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>  

#include <TROOT.h>
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
#include "THStack.h"

#include "MyDebug.hh"

using namespace std;

class MyPlot: public virtual MyDebug{
public:
    //Constructor and Destructor
    MyPlot(string category_name, string file_label, vector<bool> is_signal, vector<string> label, double systerror): _category_name(category_name), _file_label(file_label), _is_signal(is_signal), _label(label), _luminosity(3000), _systerror(systerror){};
    ~MyPlot() {_linecol.clear();_col.clear();}
    
    //interface
    void run_plot();
    
    //Plot Style Functions
    void Set_Plot_Style();
    void NormalizeHists( TH1* hist );
    
    //read data
    void read_data();
    void search_tagger_input();
    
    //Plot
    void plot_bdt_response();
    void plot_bdt_response_stack();
    void plot_roc();
    void plot_roc_eff();
    void plot_ssqrtb();
    void plot_tagger_input(string variable,double min, double max,TFile* file);

    //others
    vector<string> split(string str, char delimiter);
    
private:
    //pt-Range
    string          _category_name;
    string  	    _file_label;
    vector<string>  _label;
    vector<bool>    _is_signal;
    TString         _fin;
    string 	    _dirname_var;
    string 	    _dirname_bdt;

    vector<string>  _label_tmp;
    
    //array for bdt distribution
    vector<vector<double> > _test;
    vector<vector<double> > _train;
    
    //number of events
    vector<double> _number;
    
    //color
    vector<Int_t> _linecol;
    vector<Int_t> _col;

    double _luminosity;
    double _systerror;
};

vector<string> MyPlot::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  
  return internal;
}

void MyPlot::run_plot()
{
    Set_Plot_Style();
    _fin = "files/TMVA_"+_file_label+"_"+_category_name+".root";

    string cmd;
    _dirname_var = "results/variables/"+_file_label;
    cmd = "mkdir "+_dirname_var;
    system(cmd.c_str());

    _dirname_bdt = "results/bdt_response/"+_file_label;
    cmd = "mkdir "+_dirname_bdt;
    system(cmd.c_str());
    
    //result plots
    this->read_data();
    this->plot_bdt_response();
    this->plot_bdt_response_stack();
    this->plot_roc();
    this->plot_roc_eff();
    this->plot_ssqrtb();
    
    //tagger input distributions
    this->search_tagger_input();
}

void MyPlot::Set_Plot_Style()
{
    //No Info 
    gErrorIgnoreLevel = kWarning;
    
    //ZAxis Color
   /* const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);*/
    
    //General
    gStyle->SetOptStat(kFALSE);
    gStyle->SetCanvasBorderMode(0);
    
    //Frame
    gStyle->SetFrameBorderMode(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetFrameLineColor(1);
    gStyle->SetFrameLineWidth(2);
    
    //Lines
    gStyle->SetLineWidth(1);
    gStyle->SetFillColor(0);
    gStyle->SetCanvasColor(0);
    
    //Margins
    gStyle->SetPadLeftMargin(0.10);
    gStyle->SetPadBottomMargin(0.10);
    gStyle->SetPadRightMargin(0.01);
    gStyle->SetPadTopMargin(0.01);
    
    //text
    gStyle->SetTextFont(132);
    gStyle->SetTextSize(0.06);
    
    //Axis Title
    gStyle->SetTitleFont(62,"XYZ");
    gStyle->SetTitleSize(0.045,"X");
    gStyle->SetTitleSize(0.045,"Y");
    gStyle->SetTitleOffset(1.1,"X");
    gStyle->SetTitleOffset(1.1,"Y");
    
    //Axis Lable
    gStyle->SetLabelFont(42,"XYZ");
    gStyle->SetLabelSize(0.04,"X");
    gStyle->SetLabelSize(0.04,"Y");
    gStyle->SetLabelOffset(0.012,"X");
    gStyle->SetLabelOffset(0.012,"Y");
    
    gStyle->SetErrorX(0);
    
    
    //colors
    Int_t mycol;
    Int_t mylinecol;
    
    mycol     = TColor::GetColor( "#7d99d1" );
    mylinecol = TColor::GetColor( "#0000ee" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);
    
    mycol = TColor::GetColor( "#ff0000" );
    mylinecol = TColor::GetColor( "#ff0000" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);
    
    mycol = TColor::GetColor( "#4ca64c" );
    mylinecol = TColor::GetColor( "#008000" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);
    
    mycol = TColor::GetColor( "#ffa500" );
    mylinecol = TColor::GetColor( "#e69500" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#00ffff" );
    mylinecol = TColor::GetColor( "#00ffff" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#ff00ff" );
    mylinecol = TColor::GetColor( "#ff00ff" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#4900ff" );
    mylinecol = TColor::GetColor( "#4900ff" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#89ff00" );
    mylinecol = TColor::GetColor( "#89ff00" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#ff00a7" );
    mylinecol = TColor::GetColor( "#ff00a7" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#0092ff" );
    mylinecol = TColor::GetColor( "#0092ff" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);

    mycol = TColor::GetColor( "#ff8000" );
    mylinecol = TColor::GetColor( "#ff8000" );
    _col.push_back(mycol);
    _linecol.push_back(mylinecol);
}

void MyPlot::NormalizeHists( TH1* hist )
{
    if (hist->GetSumw2N() == 0) hist->Sumw2();
    if(hist->GetSumOfWeights()!=0)
    {
        Float_t dx = (hist->GetXaxis()->GetXmax() - hist->GetXaxis()->GetXmin())/hist->GetNbinsX();
        hist->Scale( 1.0/hist->GetSumOfWeights() );
    }
}

void MyPlot::search_tagger_input()
{
    
    // Open File
    TFile* file = TFile::Open( _fin, "READ" );
    
    // search for the right histograms in full list of keys
    TIter tnext(file->GetListOfKeys());
    TKey *tkey(0);
    tkey =(TKey*)tnext();
    TDirectory* mDir = (TDirectory*)tkey->ReadObj();
    TIter next(mDir->GetListOfKeys());
    TKey *key(0);
    while ((key = (TKey*)next()))
    {
        if (debug()>15) cout << "        KEY: " << TString(key->GetName()) << endl;
        
        if (!TString(key->GetName()).EqualTo("Method_BDTG")) continue;
        if (!gROOT->GetClass(key->GetClassName())->InheritsFrom("TDirectory")) continue;
        
        if (debug()>15) cout << "              -- accepted" << TString(key->GetName()) << endl;
        
        TDirectory* mDir = (TDirectory*)key->ReadObj();
        TIter keyIt(mDir->GetListOfKeys());
        TKey *titkey;
        
        while ((titkey = (TKey*)keyIt())) {
            
            if (debug()>15) cout << "              TITKEY: " << TString(titkey->GetName()) << endl;
            
            if (!TString(titkey->GetName()).EqualTo("BDTG")) continue;
            if (!gROOT->GetClass(titkey->GetClassName())->InheritsFrom("TDirectory")) continue;
            
            if (debug()>15) cout << "                      -- accepted " << TString(titkey->GetName()) << endl;
    
            TDirectory* titDir = (TDirectory*)titkey->ReadObj();
            TIter plotIt(titDir->GetListOfKeys());
            TKey *plotkey;
            
            while ((plotkey = (TKey*)plotIt())) {
                
                if (debug()>15) cout << "                      PLOTKEY: " << TString(plotkey->GetName()) << endl;
                
                if (!TString(plotkey->GetName()).BeginsWith("var_")) continue;
                if (!TString(plotkey->GetName()).EndsWith("__Signal")) continue;
                if (!gROOT->GetClass(plotkey->GetClassName())->InheritsFrom("TH1")) continue;
                
                if (debug()>15) cout << "                               -- accepted " << TString(plotkey->GetName()) << endl;
                
                //Now I found the histogram!!!
                
                TString plotName;
                plotName = plotkey->GetName();
                
                TH1* histo = dynamic_cast<TH1*>(titDir->Get( plotName));
                
                plotName.ReplaceAll("var_","");
                plotName.ReplaceAll("__Signal","");
                
                plot_tagger_input(string(plotName), histo->GetXaxis()->GetXmin(), histo->GetXaxis()->GetXmax(),file);
                
            }
        }
    }
    file->Close();
}
            
    
            
void MyPlot::plot_tagger_input(string variable,double min, double max,TFile* file)
    {
    
    cout << "--info: create plot for Tagger input variable distribution: " << variable <<" in range ("<<min<<", "<<max<<")"<< endl;
               
    //Define Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetLogy();

    //Define Histograms and Plot Style
    if (debug()>7) cout << "        --debug: define histograms"<<endl;
    vector<TH1*> h;
    
    TH1* h_trash = new TH1F("", "", 40,  min, max);
    h_trash->SetLineColor( kWhite );
    
    vector<string> label_tmp;
    for (int i=0;i<_label.size();i++)
    {
	if (this->contains(label_tmp,_label[i])==-1)
	{
        	TH1* h_vec = new TH1F("", "", 40,  min, max);
        
        	h_vec->SetLineColor( _linecol[label_tmp.size()] );
        	h_vec->SetLineWidth( 1 );
        	if (_is_signal[i]==true) h_vec->SetFillStyle( 1001 );   else h_vec->SetFillStyle(3554 );
        	if (_is_signal[i]==true) h_vec->SetFillColor( _col[label_tmp.size()]); else h_vec->SetFillColor(_linecol[label_tmp.size()]);
        	h.push_back(h_vec);
		label_tmp.push_back(_label[i]);
	}
    }

    // Open File
    //TFile* file = TFile::Open( _fin, "READ" );
    cout << "        start to read data from: " << _fin << endl;
    
    //Define Variables
    float var;
    float weight;
    float type;
    int id;
    int i=0;
    
    // --- Register the Train Tree
    if (debug()>7) cout << "        --debug: define train tree"<<endl;
    TTree *treetrain     = (TTree*)file->Get("dataset/TrainTree");
    treetrain->SetBranchAddress(("var_"+variable).c_str(),&var);
    treetrain->SetBranchAddress("type",&type);
    treetrain->SetBranchAddress("weight",&weight);
    
    //Loop over all events - TrainTree
    if (debug()>7) cout << "        --debug: read train tree"<<endl;
    i=0;
    while (treetrain->GetEntry(i))
    {
        h[this->contains(label_tmp,_label[int(type)])]->Fill(var, weight);
        i++;
    }
    
    // --- Register the Test Tree
    if (debug()>7) cout << "        --debug: define test tree"<<endl;
    TTree *treetest     = (TTree*)file->Get("dataset/TestTree");
    treetest->SetBranchAddress(("var_"+variable).c_str(),&var);
    treetest->SetBranchAddress("type",&type);
    treetest->SetBranchAddress("weight",&weight);
    
    //Loop over all events - TestTree
    if (debug()>7) cout << "        --debug: read test tree"<<endl;
    while (treetest->GetEntry(i))
    {
       h[this->contains(label_tmp,_label[int(type)])]->Fill(var, weight);
        i++;
    }

    //Normalize Histograms
    if (debug()>7) cout << "        --debug: normalize histograms"<<endl;
    /*for (int i=0;i<label_tmp.size();i++)
    {
        NormalizeHists(h[i]);
    }*/
    
    // frame limits
    float xmin = min;
    float xmax = max;
    float ymin = 0.0001;
    float ymax = 1;
    for (int i=0; i<h.size(); i++)
    {
        if (h[i]->GetMaximum()*10>ymax) ymax=h[i]->GetMaximum()*10;
    }

    
    // build a frame
    if (debug()>7) cout << "        --debug: define frame"<<endl;
    int nb = 500;
    TH2F* frame = new TH2F( "", "", nb, xmin, xmax, nb, ymin, ymax );
    frame->GetXaxis()->SetTitle( variable.c_str() );
    frame->GetYaxis()->SetTitle("d#sigma^{ }/^{ }dx [fb/bin]");
    frame->GetYaxis()->SetLimits( ymin, ymax );
    
    // Draw legend
    if (debug()>7) cout << "        --debug: define legend"<<endl;
    TLegend *legend= new TLegend( c->GetLeftMargin(), 1 - c->GetTopMargin() - 0.1,
                                 c->GetLeftMargin() + 0.44 , 1 - c->GetTopMargin() );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    for (int i=0;i<=int(double(label_tmp.size())/2.);i++)
    {
        legend->AddEntry(h[i],TString(label_tmp[i])     , "F");
    }
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );
    
    TLegend *legend2= new TLegend( 1 - c->GetRightMargin() - 0.44, 1 - c->GetTopMargin() - 0.1,
                                  1 - c->GetRightMargin(), 1 - c->GetTopMargin() );
    legend2->SetFillStyle( 1 );
    legend2->SetTextFont(12);
    legend2->SetBorderSize(1);
    for (int i=int(double(label_tmp.size())/2.)+1;i<label_tmp.size();i++)
    {
        legend2->AddEntry(h[i],TString(label_tmp[i])     , "F");
    }
    if (label_tmp.size()%2==1) legend2->AddEntry(h_trash,TString("")     , "F");
    legend2->SetMargin( 0.1 );
    
    // Draw
    if (debug()>7) cout << "        --debug: draw"<<endl;
    frame->Draw();
    
    for (int itype=0;itype<label_tmp.size();itype++)
    {
        h[itype]->Draw("samehist");
    }
    legend->Draw("same");
    legend2->Draw("same");
    
    // save canvas to file
    string filename_out = _dirname_var+"/"+_file_label+"_"+_category_name+"_"+ variable+".pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());
    
    // delete
    for(vector<TH1*>::iterator hist=h.begin(); hist!=h.end(); ++hist)
    {
      if(*hist!=NULL){delete *hist,*hist=NULL;}
    }
    delete h_trash;
    delete frame;
    delete legend;
    delete legend2;
    delete c;
}



void MyPlot::read_data()
{
    _test.clear();
    _train.clear();
    _number.clear();
   
    for (int i=0;i<_label.size();i++)
    {
	if (this->contains(_label_tmp,_label[i])==-1)
	{
       	    vector<double>  test_vec;
            vector<double>  train_vec;
            for (int j=0;j<200;j++)
            {
            	test_vec.push_back(0);
            	train_vec.push_back(0);
            }
            _test.push_back(test_vec);
            _train.push_back(train_vec);
            _number.push_back(0);
	    _label_tmp.push_back(_label[i]);
	}
    }
    
    // Open File
    TFile* file = TFile::Open( _fin, "READ" );
    cout << "--info: start to read data from: " << _fin << endl;
    
    //Define Variables
    float bdtout;
    float weight;
    float type;
    int id;
    int i=0;
    
    // --- Register the Train Tree
    TTree *treetrain     = (TTree*)file->Get("dataset/TrainTree");
    treetrain->SetBranchAddress("BDTG",&bdtout);
    treetrain->SetBranchAddress("classID",&id);
    treetrain->SetBranchAddress("type",&type);
    treetrain->SetBranchAddress("weight",&weight);
    
    //Loop over all events - TrainTree
    i=0;
    cout << "        read Training Tree" << endl;
    while (treetrain->GetEntry(i))
    {
        //get bdt from bdt output
        int bdt = int((bdtout+1.)*100.);
        _train[this->contains(_label_tmp,_label[int(type)])][bdt]+=weight;
        i++;
    }
    cout << "        found number of entries: " << i << endl;

    // --- Register the Test Tree
    TTree *treetest     = (TTree*)file->Get("dataset/TestTree");
    treetest->SetBranchAddress("BDTG",&bdtout);
    treetest->SetBranchAddress("classID",&id);
    treetest->SetBranchAddress("type",&type);
    treetest->SetBranchAddress("weight",&weight);
    
    //Loop over all events - TestTree
    i=0;
    cout << "        read Test Tree" << endl;
    if (debug()>7) cout << "        --debug: very signal like backgrounds" << endl;
    while (treetest->GetEntry(i))
    {
        //get bdt from bdt output
        int bdt = int((bdtout+1.)*100.);
        _test[this->contains(_label_tmp,_label[int(type)])][bdt]+=weight;
        _number[this->contains(_label_tmp,_label[int(type)])]+=weight;
        i++;
    }
    cout << "        found number of entries: " << i << endl;
    file->Close();
}


void MyPlot::plot_bdt_response()
{
    cout << "--info: create plot for BDT-response distribution"<< endl;

    //Define Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetLogy();
    
    //Define Histograms and Plot Style
    vector<TH1*> h_test;
    vector<TH1*> h_train;
    
    if (debug()>7) cout << "        --debug: define histograms"<<endl;
    for (int i=0;i<_label_tmp.size();i++)
    {
        TH1* h_train_vec = new TH1F("", "", 40, -1, 1);
        TH1* h_test_vec = new TH1F("", "", 40, -1, 1);
        
        h_test_vec->SetLineColor( _linecol[i] );
        h_test_vec->SetLineWidth( 1 );
        if (_is_signal[i]==true) h_test_vec->SetFillStyle( 1001 );  else h_test_vec->SetFillStyle(3554 );
        if (_is_signal[i]==true) h_test_vec->SetFillColor( _col[i]); else h_test_vec->SetFillColor(_linecol[i]);
        
        h_train_vec->SetMarkerColor( _linecol[i] );
        h_train_vec->SetMarkerSize( 3 );
        h_train_vec->SetMarkerStyle( 2 );
        h_train_vec->SetLineWidth( 1 );
        h_train_vec->SetLineColor( _linecol[i] );
        
        h_test.push_back(h_test_vec);
        h_train.push_back(h_train_vec);
    }
    
    //Fill Histograms
    if (debug()>7) cout << "        --debug: fill histograms"<<endl;
    for(int ibdt =0;ibdt<200;ibdt++)
    {
        double bdt = double(ibdt)/100.-1;
        
        for (int itype=0;itype<_label_tmp.size();itype++)
        {
            h_test[itype]->Fill(bdt,_test[itype][ibdt]);
            h_train[itype]->Fill(bdt,_train[itype][ibdt]);
        }
    }
 
    //Normalize Histograms
    if (debug()>7) cout << "        --debug: normalize histograms"<<endl;
    for (int i=0;i<_label_tmp.size();i++)
    {
        NormalizeHists(h_test[i]);
        NormalizeHists(h_train[i]);
    }
    
    // frame limits
    float xmin = -1;
    float xmax = 1;
    float ymin = 0.001;
    float ymax = 10;
    
    // build a frame
    if (debug()>7) cout << "        --debug: define frame"<<endl;
    int nb = 500;
    TH2F* frame = new TH2F( "", "", nb, xmin, xmax, nb, ymin, ymax );
    frame->GetXaxis()->SetTitle( "BDT response" );
    frame->GetYaxis()->SetTitle("(1/N) dN^{ }/^{ }dx");
    frame->GetYaxis()->SetLimits( ymin, ymax );
    
    // Draw legend
    if (debug()>7) cout << "        --debug: define legend"<<endl;
    TLegend *legend= new TLegend( c->GetLeftMargin(), 1 - c->GetTopMargin() - 0.2,
                                 c->GetLeftMargin() + 0.44 , 1 - c->GetTopMargin() );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    for (int i=0;i<_label_tmp.size();i++)
    {
        legend->AddEntry(h_test[i],TString(_label_tmp[i])     + " (test sample)" , "F");
    }
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );
    
    TLegend *legend2= new TLegend( 1 - c->GetRightMargin() - 0.44, 1 - c->GetTopMargin() - 0.2,
                                  1 - c->GetRightMargin(), 1 - c->GetTopMargin() );
    legend2->SetFillStyle( 1 );
    legend2->SetTextFont(12);
    legend2->SetBorderSize(1);
    for (int i=0;i<_label_tmp.size();i++)
    {
        legend2->AddEntry(h_train[i],TString(_label_tmp[i])     + " (training sample)" , "F");
    }
    legend2->SetMargin( 0.1 );
    
    // Draw
    if (debug()>7) cout << "        --debug: draw histograms"<<endl;
    frame->Draw();
    
    legend->Draw("same");
    legend2->Draw("same");
    
    for (int i=0;i<_label_tmp.size();i++)
    {
        h_test[i]->Draw("samehist");
    }
    
    for (int i=0;i<_label_tmp.size();i++)
    {
        h_train[i]->Draw("same hist p");
    }
    
    // save canvas to file
    string filename_out = _dirname_bdt+"/"+_file_label+"_"+_category_name+"_bdt_response.pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());
    
    // delete
    for(vector<TH1*>::iterator hist=h_test.begin(); hist!=h_test.end(); ++hist)
    {
      if(*hist!=NULL){delete *hist,*hist=NULL;}
    }
    for(vector<TH1*>::iterator hist=h_train.begin(); hist!=h_train.end(); ++hist)
    {
      if(*hist!=NULL){delete *hist,*hist=NULL;}
    }
    delete frame;
    delete legend;
    delete legend2;
    delete c;
}

void MyPlot::plot_bdt_response_stack()
{
    cout << "--info: create plot for BDT-response distribution"<< endl;
    
    //Define Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetLogy();
    
    //Define Histograms and Plot Style
    vector<TH1*> h_test;
    
    //Define stack
    THStack *stack = new THStack("hs","");
    
    if (debug()>7) cout << "        --debug: define histograms"<<endl;
    for (int i=0;i<_label_tmp.size();i++)
    {
        TH1* h_test_vec = new TH1F("", "", 20, 0.6, 1);
        
        h_test_vec->SetLineColor( _linecol[i] );
        h_test_vec->SetLineWidth( 1 );
        if (_is_signal[i]==true) h_test_vec->SetFillStyle( 1001 );  else h_test_vec->SetFillStyle(3554 );
        if (_is_signal[i]==true) h_test_vec->SetFillColor( _col[i]); else h_test_vec->SetFillColor(_linecol[i]);
        
        h_test.push_back(h_test_vec);
    }
    
    //Fill Histograms
    if (debug()>7) cout << "        --debug: fill histograms"<<endl;
    for(int ibdt =0;ibdt<200;ibdt++)
    {
        double bdt = double(ibdt)/100.-1;
        
        for (int itype=0;itype<_label_tmp.size();itype++)
        {
            h_test[itype]->Fill(bdt,_test[itype][ibdt]);
        }
        
    }

    for (int i=1;i<_label_tmp.size();i++)
    {
        stack->Add(h_test[i]);
    }
    stack->Add(h_test[0]);
   
    
    // frame limits
    float xmin = 0.6;
    float xmax = 1;
    float ymin = 0.0001;
    float ymax = stack->GetMaximum()*50;
    
    // build a frame
    if (debug()>7) cout << "        --debug: define frame"<<endl;
    int nb = 500;
    TH2F* frame = new TH2F( "", "", nb, xmin, xmax, nb, ymin, ymax );
    frame->GetXaxis()->SetTitle( "BDT response" );
    frame->GetYaxis()->SetTitle("dN^{ }/^{ }dx [fb]");
    frame->GetYaxis()->SetLimits( ymin, ymax );
    
    // Draw legend
    if (debug()>7) cout << "        --debug: define legend"<<endl;
    TLegend *legend= new TLegend( c->GetLeftMargin()+0.22, 1 - c->GetTopMargin() - 0.2,
                                 1 -c->GetRightMargin() - 0.22 , 1 - c->GetTopMargin() );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    for (int i=0;i<_label_tmp.size();i++)
    {
        legend->AddEntry(h_test[i],TString(_label_tmp[i])     + " (test sample)" , "F");
    }
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );
    
    // Draw
    if (debug()>7) cout << "        --debug: draw histograms"<<endl;
    frame->Draw();
    
    legend->Draw("same");
    

    stack->Draw("samehist");
    
    // save canvas to file
    string filename_out = _dirname_bdt+"/"+_file_label+"_"+_category_name+"_bdt_response_stack.pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());
    
    // delete
    delete stack;
    delete frame;
    delete legend;
    delete c;
}


void MyPlot::plot_roc()
{
    cout << "--info: create plot for ROC curve"<< endl;
    
    //Define Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetGrid();
    c->SetLogy();

    //number of events passing a cut of ibdt: sum_xx[ibdt]
    double sum[_label_tmp.size()][200];
    for (int i=0;i<_label_tmp.size();i++)
    	for (int j=0;j<200;j++)
		sum[i][j]=0;
    double eff[_label_tmp.size()][200];
    for (int i=0;i<_label_tmp.size();i++)
    	for (int j=0;j<200;j++)
		eff[i][j]=0;
    
    //get sums
    if (debug()>7) cout << "        --debug: get sums"<<endl;
    for(int bdt =0;bdt<200;bdt++)
    {
        for(int ibdt =0;ibdt<bdt;ibdt++)
        {
            for (int itype=0;itype<_label_tmp.size(); itype++)
            {
                sum[itype][ibdt]+=_test[itype][bdt];
            }
        }
    }
    
    //Get efficicies
    if (debug()>7) cout << "        --debug: get efficiencies"<<endl;
    for(int ibdt =0;ibdt<199;ibdt++)
    {
        for (int itype=0; itype<_label_tmp.size(); itype++ )
        {
            if (_is_signal[itype]==true)
            {
                eff[itype][ibdt] = sum[itype][ibdt]/double(_number[itype]);
                if (debug()>10) cout << "        --debug: eff[" <<itype<<"]["<<ibdt<<"]=" << eff[itype][ibdt]<<endl;
            }
            else
            {
                if (sum[itype][ibdt]!=0) eff[itype][ibdt] = 1./(sum[itype][ibdt]/double(_number[itype]));else eff[itype][ibdt]=0;
                if (debug()>10) cout << "        --debug: eff[" <<itype<<"]["<<ibdt<<"]=" << eff[itype][ibdt]<<endl;
            }
        }
    }
    
    //Graphs - MyBTagger
    if (debug()>7) cout << "        --debug: define graphs"<<endl;
    vector<TGraph*> graph;
    for (int itype=1; itype<_label_tmp.size();itype++)
    {
        TGraph* graph_vec = new TGraph(199,eff[0],eff[itype]);
        graph_vec->SetLineColor( _linecol[itype] );
        graph_vec->SetLineWidth( 3 );
        graph_vec->SetFillStyle( 0 );
        graph.push_back(graph_vec);
    }
    
    // frame limits
    float xmin = 0;
    float xmax = 1;
    float ymin = 1;
    float ymax = 10000*1.05;
    
    // build a frame
    if (debug()>7) cout << "        --debug: build frame"<<endl;
    int nb = 500;
    TH2F* frame = new TH2F( "", "", nb, xmin, xmax, nb, ymin, ymax );
   	frame->GetXaxis()->SetTitle( "Signal efficiency" );
   	frame->GetYaxis()->SetTitle( "1/(Background efficiency)" );
   	frame->GetYaxis()->SetNdivisions( 512 );
    
    // Draw legend
    TLegend *legend= new TLegend( c->GetLeftMargin()+0.02, c->GetBottomMargin() + 0.02,
                                 c->GetLeftMargin() + 0.25 , c->GetBottomMargin() + 0.3 );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    for (int itype=1; itype<_label_tmp.size();itype++)
    {
        legend->AddEntry(graph[itype-1],TString(_label_tmp[itype])  , "F");
    }
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );

    // Draw
    if (debug()>7) cout << "        --debug: draw"<<endl;
    frame->Draw();
    legend->Draw("same");

    for (int itype=1; itype<_label_tmp.size(); itype++)
    {
        graph[itype-1]->Draw("same");
    }

    // save canvas to file
    string filename_out =  _dirname_bdt+"/"+_file_label+"_"+_category_name+"_roc_curve_inv.pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());
    
    // delete
    delete frame;
    delete legend;
    delete c;
}



void MyPlot::plot_roc_eff()
{
    cout << "--info: create plot for ROC curve"<< endl;
    
    //Define Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetGrid();
    
    //number of events passing a cut of ibdt: sum_xx[ibdt]
    double sum[_label_tmp.size()][200];
    for (int i=0;i<_label_tmp.size();i++)
    	for (int j=0;j<200;j++)
		sum[i][j]=0;
    double eff[_label_tmp.size()][200];
    for (int i=0;i<_label_tmp.size();i++)
    	for (int j=0;j<200;j++)
		eff[i][j]=0;
    
    //get sums
    if (debug()>7) cout << "        --debug: get sums"<<endl;
    for(int bdt =0;bdt<200;bdt++)
    {
        for(int ibdt =0;ibdt<bdt;ibdt++)
        {
            for (int itype=0;itype<_label_tmp.size(); itype++)
            {
                sum[itype][ibdt]+=_test[itype][bdt];
            }
        }
    }
    
    //Get efficicies
    if (debug()>7) cout << "        --debug: get efficiencies"<<endl;
    for(int ibdt =0;ibdt<199;ibdt++)
    {
        for (int itype=0; itype<_label_tmp.size(); itype++ )
        {
            if (_is_signal[itype]==true)
            {
                eff[itype][ibdt] = sum[itype][ibdt]/double(_number[itype]);
                if (debug()>10) cout << "        --debug: eff[" <<itype<<"]["<<ibdt<<"]=" << eff[itype][ibdt]<<endl;
            }
            else
            {
                if (sum[itype][ibdt]!=0) eff[itype][ibdt] = 1-(sum[itype][ibdt]/double(_number[itype]));else eff[itype][ibdt]=1;
                if (debug()>10) cout << "        --debug: eff[" <<itype<<"]["<<ibdt<<"]=" << eff[itype][ibdt]<<endl;
            }
        }
    }
    
    //Graphs - MyBTagger
    if (debug()>7) cout << "        --debug: define graphs"<<endl;
    vector<TGraph*> graph;
    for (int itype=1; itype<_label_tmp.size();itype++)
    {
        TGraph* graph_vec = new TGraph(199,eff[0],eff[itype]);
        graph_vec->SetLineColor( _linecol[itype] );
        graph_vec->SetLineWidth( 3 );
        graph_vec->SetFillStyle( 0 );
        graph.push_back(graph_vec);
    }
    
    // frame limits
    float xmin = 0;
    float xmax = 1;
    float ymin = 0;
    float ymax = 1.01;
    
    // build a frame
    if (debug()>7) cout << "        --debug: build frame"<<endl;
    int nb = 500;
    TH2F* frame = new TH2F( "", "", nb, xmin, xmax, nb, ymin, ymax );
   	frame->GetXaxis()->SetTitle( "Signal efficiency" );
   	frame->GetYaxis()->SetTitle( "Background efficiency" );
   	frame->GetYaxis()->SetNdivisions( 512 );
    
    // Draw legend
    TLegend *legend= new TLegend( c->GetLeftMargin()+0.02, c->GetBottomMargin() + 0.02,
                                 c->GetLeftMargin() + 0.25 , c->GetBottomMargin() + 0.3 );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    for (int itype=1; itype<_label_tmp.size();itype++)
    {
        legend->AddEntry(graph[itype-1],TString(_label_tmp[itype])  , "F");
    }
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );
    
    // Draw
    if (debug()>7) cout << "        --debug: draw"<<endl;
    frame->Draw();
    legend->Draw("same");
    
    for (int itype=1; itype<_label_tmp.size(); itype++)
    {
        graph[itype-1]->Draw("same");
    }
    
    // save canvas to file
    string filename_out =  _dirname_bdt+"/"+_file_label+"_"+_category_name+"_roc_curve_eff.pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());
    
    // delete
    delete frame;
    delete legend;
    delete c;    
}

void MyPlot::plot_ssqrtb()
{
    cout << "--info: create plot for Significance"<< endl;
    
    //Set Canvas
    TCanvas* c = new TCanvas("","",1000,725);
    c->SetGrid();
    
    // define histogram
    double s[200]={0};
    double b[200]={0};
    TH1F* curve = new TH1F( "", "", 201, -1, 1);
    TH1F* curve_syst = new TH1F( "", "", 201, -1, 1);
    curve->SetLineWidth(3);
    curve->SetLineColor(1);
    curve_syst->SetLineWidth(3);
    curve_syst->SetLineColor(2);
    
    // Open File
    if (debug()>7) cout << "        --debug: open TMVA file " << _fin<<endl;
    TFile* file = TFile::Open( _fin, "READ" );
    
    // --- Register the training and test trees
    TTree *tree     = (TTree*)file->Get("dataset/TestTree");
    
    float bdtout;
    float weight;
    float type;
    tree->SetBranchAddress("BDTG",&bdtout);
    tree->SetBranchAddress("type",&type);
    tree->SetBranchAddress("weight",&weight);
    
    //Loop over all events
    int i=0;
    while (tree->GetEntry(i))
    {
        int bdt = int((bdtout+1.)*100.);
        //fill all bins that have ibdt>bdt
        for(int ibdt =0;ibdt<bdt;ibdt++)
            {if (type==0) s[ibdt]+=weight; else b[ibdt]+=weight;}
        i++;
    }
    
    for(int ibdt =0;ibdt<200;ibdt++)
    {
        double bdt = double(ibdt)/100.-1;
        double w;
        if (b[ibdt]==0) w=_luminosity*s[ibdt]/2.; else w= _luminosity*s[ibdt]/(sqrt(_luminosity*b[ibdt]) );
        curve->Fill(bdt,w);
        if (b[ibdt]==0) w=_luminosity*s[ibdt]/2.; else w= _luminosity*s[ibdt]/(sqrt(_luminosity*b[ibdt]) + _systerror*_luminosity*b[ibdt] );
        curve_syst->Fill(bdt,w);
        if (debug()>7) if (bdt>0.8) cout << "BDTResponse, XS, XB, s/(sqrt(b)+0.1b), Excl.Lim.: "<<bdt << ", " <<s[ibdt] << ", "<<b[ibdt] << ", "  << w << ", " << 100.*2/w  << endl;
    }
    
    // define frame
    TH2F* frame = new TH2F( "", "", 200, -1, 1, 500, 0, 1 );
    frame->GetXaxis()->SetTitle( "BDT output" );
    frame->GetYaxis()->SetTitle( "S/(Sqrt(B)+xB)" );
    frame->GetYaxis()->SetLimits( 0, 1.1*curve->GetMaximum() );
    
    // Draw legend
    TLegend *legend= new TLegend( c->GetLeftMargin()+0.02, c->GetTopMargin() - 0.16,
                                 c->GetLeftMargin() + 0.25 , c->GetTopMargin() - 0.02 );
    legend->SetFillStyle( 1 );
    legend->SetTextFont(12);
    legend->AddEntry(curve,"0% syst. error"  , "F");
    legend->AddEntry(curve,TString((DoubleToString(100*_systerror)+"% syst. error").c_str())  , "F");
    legend->SetBorderSize(1);
    legend->SetMargin( 0.2  );
    
    
    //+++++++++++++++++++++++
    // Draw
    frame->Draw();
    curve->Draw("csame");
    curve_syst->Draw("csame");
    
    // save canvas to file
    string filename_out =  _dirname_bdt+"/"+_file_label+"_"+_category_name+"_significance.pdf";
    cout << "        see " <<  filename_out<< endl;
    gPad->RedrawAxis();
    c->Print(filename_out.c_str());

    double significance = curve_syst->GetMaximum();
      
    delete curve_syst;
    delete curve;
    delete frame;
    delete c;

    /*vector<string> words= this->split(_file_label,'_');
   
    string filename = "results/summary/summary_approx.txt";
    ofstream summaryfile;
    summaryfile.open(filename.c_str(),std::ios_base::app);
    summaryfile  << setw(20) << _category_name;
    summaryfile  << setw(20) << words[1];
    summaryfile  << setw(20) << words[3];
    summaryfile  << setw(20) << words[5];
    summaryfile  << setw(20) << words[7];
    summaryfile  << setw(20) << words[9];
    summaryfile  << setw(20) << words[11];
    summaryfile  << setw(20) << _luminosity;
    summaryfile  << setprecision(5) << setw(10) << 2./significance;
    summaryfile  << setprecision(5) << setw(10) << significance << endl;
    summaryfile.close();*/
}

