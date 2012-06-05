#
# Makefile for the Njoy N7 PVR AddOn
# needs to be placed in xbmc/pvrclients folder
# pvr.njoy folder needs to be moved to addons after build is done

LIBS   = -ldl
LIBDIR = pvr.njoy
PVR    = pvr.njoy/XBMC_Njoy.pvr
CLEAN_FILES = *.P *~ $(PVR)

pvr: $(PVR)

SRCS=client.cpp \
      N7Xml.cpp

include ../Makefile.include 

INCLUDES += -I../xbmc

$(PVR): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -g $(OBJS) $(LIBS) $(LIBDIRS) $(SILIB) -o $(PVR)
