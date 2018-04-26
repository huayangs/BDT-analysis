TARGET         = main
####################################
CC             = g++ -std=c++0x -Wno-deprecated
OBJS           = $(TARGET).o
######### ROOT #####################
ROOTCFLAGS     = $(shell root-config --cflags)
ROOTLIBS       = $(shell root-config --libs) -lMLP -lMinuit -lTreePlayer -lTMVA -lXMLIO  -lMLP -lm
######### FASTJET ##################
FASTJETCFLAGS  = $(shell fastjet-config --cxxflags)
FASTJETLIBS    = $(shell fastjet-config --libs)
######### DELPHES ##################
DELPHESSYS     =  /Users/huayang/work/MG5_aMC_v2_3_3/Delphes
DELPHESLIBS    = -L${DELPHESSYS} -lDelphes
DELPHESCFLAGS  = -I${DELPHESSYS} -I${DELPHESSYS}/modules -I${DELPHESSYS}/classes
######### EXROOTANALYSIS ##################
EXROOTSYS      = /Users/huayang/work/MG5_aMC_v2_3_3/Delphes/external
EXROOTCFLAGS   = -I${EXROOTSYS}
######### HERE ##################
THISCFLAGS     = -I./

#########  FLAGS, LIBS  ############
CXXFLAGS       = ${ROOTCFLAGS}
CXXFLAGS      += $(FASTJETCFLAGS)
CXXFLAGS      += $(DELPHESCFLAGS)
CXXFLAGS      += $(EXROOTCFLAGS)
CXXFLAGS      += $(THISCFLAGS) 

CXXLIBS        = $(ROOTLIBS)
CXXLIBS       += $(FASTJETLIBS)
CXXLIBS       += $(DELPHESLIBS)

#########################################################

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) $(CXXLIBS) -o $@
.cc.o:
	$(CC) $(CXXFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS) *~
