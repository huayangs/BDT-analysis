#!/bin/bash

./MyAnalysis --background --usebackground=input/background_14TeV_bbll.txt   --usecath=input/cathegories_bbll.txt
./MyAnalysis --analysis   --usesignal=input/signal_14TeV_bbll_tb_1.5.txt           --usecath=input/cathegories_bbll.txt
./MyAnalysis --plot       --usebackground=input/background_14TeV_bbll.txt   --usesignal=input/signal_14TeV_bbll_tb_1.5.txt           --usecath=input/cathegories_bbll.txt --systerror=0.05
./MyAnalysis --statistics --usesignal=input/signal_14TeV_bbll_tb_1.5.txt           --usecath=input/cathegories_bbll.txt --systerror=0.05
