#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include "MyCreate.hh"
#include "MyTMVA.hh"
#include "MyPlot.hh"
#include "MyDebug.hh"
#include "MyStatistics.hh"

using namespace std;

class MyInterface: public virtual MyDebug{
public:
    //Constructor and Destructor
    MyInterface() {;}
    ~MyInterface() {;}
    
    //Run Functions
    int run_interface( int argc, char **argv );
    
private:
};

int MyInterface::run_interface( int argc, char **argv )
{
    //Get fastjet banner
    cout << "--info: start"<<endl;
    gErrorIgnoreLevel=kError;
    
    //Input
    int debug_level=0;
    bool do_analysis=false;
    bool do_background=false;
    bool do_plot=false;
    bool do_statistics=false;
    string backgroundfile="input/background.txt";
    string signalfile="input/signal.txt";
    string cathfile="input/cathegories.txt";
    double systerror=0.1;
    
    for(int i=1; i<argc; i++)
    {
        if(string(argv[i])=="--help")
        {
            cout <<"Input Format ./MyAnalysis <option>" << endl;
            cout <<" --help                 : get this help"<< endl;
            cout <<" --debug=x              : set debug level to x"<< endl;
            cout <<" --background           : create rootfiles for background events"<< endl;
            cout <<" --analysis             : run the entire analysis"<< endl;
            cout <<" --plot                 : plot results"<< endl;
            cout <<" --statistics           : perform statistical analysis"<< endl;
            cout <<" --usesignal=x          : use signal file x"<< endl;
            cout <<" --usebackground=x      : use background file x"<< endl;
            cout <<" --usecath=x            : use cathegory file x"<< endl;
            cout <<" --systerror=x          : use systematic error x, default: x=0.1"<< endl;
            
            return 0;
        }
        
        if(string(argv[i]).substr(0,8)=="--debug=")
        {
            cout <<"        set debug level to "<<string(argv[i]).substr(8)<<endl;
            debug_level=stoi(string(argv[i]).substr(8));
            this->set_debug_level(debug_level);
        }
        
        if(string(argv[i])=="--background")
        {
            cout <<"        create background files"<<endl;
            do_background=true;
        }
        
        if(string(argv[i])=="--analysis")
        {
            cout <<"        perform entire analysis"<<endl;
            do_analysis=true;
        }
        
        if(string(argv[i])=="--plot")
        {
            cout <<"        create plots"<<endl;
            do_plot=true;
        }
        
        if(string(argv[i])=="--statistics")
        {
            cout <<"        perform statistical analysis"<<endl;
            do_statistics=true;
        }
        
        if(string(argv[i]).substr(0,12)=="--usesignal=")
        {
            cout <<"        use signal file "<<string(argv[i]).substr(12)<<endl;
            signalfile=string(argv[i]).substr(12);
        }
        
        if(string(argv[i]).substr(0,16)=="--usebackground=")
        {
            cout <<"        use background file "<<string(argv[i]).substr(16)<<endl;
            backgroundfile=string(argv[i]).substr(16);
        }
        
        if(string(argv[i]).substr(0,10)=="--usecath=")
        {
            cout <<"        use cathegory file "<<string(argv[i]).substr(10)<<endl;
            cathfile=string(argv[i]).substr(10);
        }
        
        if(string(argv[i]).substr(0,12)=="--systerror=")
        {
            cout <<"        use systematic error"<<string(argv[i]).substr(12)<<endl;
            systerror=stof(string(argv[i]).substr(12));
        }
    }
    
    //Define input variables
    vector<string>  sig_file_name;
    vector<double>  sig_crosssection;
    vector<int>     sig_nfiles;
    vector<string>  sig_directory;
    
    vector<string>  bg_file_name;
    vector<double>  bg_crosssection;
    vector<int>     bg_nfiles;
    vector<string>  bg_directory;
    
    vector<bool>    is_signal;
    vector<string>  label;
    
    //Read signal from file
    string input_file_signal=signalfile;
    cout << "--info: read input from " << input_file_signal<<endl;
    ifstream infile_signal(input_file_signal.c_str());
    string line_signal;
    for(int i = 0; infile_signal.good(); i++)
    {
        getline(infile_signal, line_signal);
        istringstream iss(line_signal);
        vector<string> cmd;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter<vector<string> >(cmd));
        
        if ((cmd.size()!=13)) continue;
        if (cmd[0].substr(0,1)=="#") continue;
        
        string point = "mH_"+cmd[0]+"_mA_"+cmd[1]+"_mC_"+cmd[2]+"_tb_"+cmd[3]+"_cba_"+cmd[4]+"_m122_"+cmd[5];
        
        if (debug_level>5) cout <<"        --debug: add process with "<<endl;
        if (debug_level>5) cout <<"                 filename = "<< point <<endl;
        if (debug_level>5) cout <<"                 number of files = "<<cmd[10] <<endl;
        if (debug_level>5) cout <<"                 cross section = "<< cmd[11]<< " fb"<<endl;
        if (debug_level>5) cout <<"                 label = Signal"<<endl;
        if (debug_level>5) cout <<"                 diretcory = "<<cmd[12]<<endl;
        
        sig_file_name.push_back(point);
        sig_nfiles.push_back(atoi(cmd[10].c_str()));
        sig_crosssection.push_back(atof(cmd[11].c_str()));
        sig_directory.push_back(cmd[12].c_str());
    }
    is_signal.push_back(true);
    label.push_back("Signal");
    
    //Read background from file
    string input_file_bg=backgroundfile;
    cout << "--info: read input from " << input_file_bg<<endl;
    ifstream infile_bg(input_file_bg.c_str());
    string line_bg;
    for(int i = 0; infile_bg.good(); i++)
    {
        getline(infile_bg, line_bg);
        istringstream iss(line_bg);
        vector<string> cmd;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter<vector<string> >(cmd));
        
        if ((cmd.size()!=5)) continue;
        if (cmd[0].substr(0,1)=="#") continue;
        
        if (debug_level>5) cout <<"        --debug: add process with "<<endl;
        if (debug_level>5) cout <<"                 filename = "<<cmd[0]<<endl;
        if (debug_level>5) cout <<"                 number of files = "<<cmd[1]<<endl;
        if (debug_level>5) cout <<"                 cross section = "<<cmd[2] << " pb"<<endl;
        if (debug_level>5) cout <<"                 label = "<<cmd[3]<<endl;
        if (debug_level>5) cout <<"                 diretcory = "<<cmd[4]<<endl;
        
        bg_file_name.push_back(cmd[0]);
        bg_nfiles.push_back(atoi(cmd[1].c_str()));
        bg_crosssection.push_back(1000.*atof(cmd[2].c_str()));
        bg_directory.push_back(cmd[4].c_str());
        label.push_back(cmd[3]);
        is_signal.push_back(false);
    }
    
    //Read cathegories from file
    vector<string> cathegories;
    string cathegorie_file=cathfile;
    cout << "--info: read input from " << cathegorie_file<<endl;
    ifstream infile_cathegorie(cathegorie_file.c_str());
    string line_cathegorie;
    for(int i = 0; infile_cathegorie.good(); i++)
    {
        getline(infile_cathegorie, line_cathegorie);
        istringstream iss(line_cathegorie);
        vector<string> cmd;
        copy(istream_iterator<string>(iss),
             istream_iterator<string>(),
             back_inserter<vector<string> >(cmd));
        
        if ((cmd.size()!=1)) continue;
        if (cmd[0].substr(0,1)=="#") continue;
        
        cathegories.push_back(cmd[0]);
        cout <<"        do cathegorie "<< cmd[0]<<endl;
    }
    cout <<endl;
    
    //Background ONLY mode
    if (do_background)
    {
        for (int i=0; i<cathegories.size(); i++)
        {
            //Create File
            MyCreate * creater= new MyCreate(cathegories[i],0,"Background",bg_file_name, bg_crosssection, bg_nfiles,bg_directory);
            creater->set_debug_level(debug_level);
            creater->run_create();
            delete creater;
        }
    }
    
    //Analysis mode
    if (do_analysis)
    {
        for (int i=0; i<sig_file_name.size();i++)
        {
            vector<string> tmp_file_name; tmp_file_name.push_back(sig_file_name[i]);
            vector<double> tmp_crosssection; tmp_crosssection.push_back(sig_crosssection[i]);
            vector<int>    tmp_nfiles; tmp_nfiles.push_back(sig_nfiles[i]);
            vector<string> tmp_directory; tmp_directory.push_back(sig_directory[i]);
            
            for (int j=0; j<cathegories.size(); j++)
            {
                MyCreate * creater= new MyCreate(cathegories[j],1, tmp_file_name[0], tmp_file_name, tmp_crosssection, tmp_nfiles, tmp_directory);
                creater->set_debug_level(debug_level);
                creater->run_create();
                delete creater;
                
                MyTMVA * tmva=new MyTMVA(cathegories[j],tmp_file_name[0]);
                tmva->set_debug_level(debug_level);
                tmva->run_TMVA();
                delete tmva;
            }
        }
    }
    
    //Plot mode
    if (do_plot)
    {
        for (int i=0; i<sig_file_name.size();i++)
        {
            for (int j=0; j<cathegories.size(); j++)
            {
                MyPlot * plot=new MyPlot(cathegories[j],sig_file_name[i], is_signal, label,systerror);
                plot->set_debug_level(debug_level);
                plot->run_plot();
                delete plot;
            }
        }
    }
    
    //Statistics mode
    if (do_statistics)
    {
        for (int i=0; i<sig_file_name.size();i++)
        {
            MyStatistics * statistics=new MyStatistics(cathegories,sig_file_name[i],systerror);
            statistics->set_debug_level(debug_level);
            statistics->run_statistics();
            delete statistics;
        }
    }
    
    return 0;
}
