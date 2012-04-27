#
# Makefile for the Njoy N7 PVR AddOn
# needs to be placed in xbmc/pvrclients folder
# pvr.njoy folder needs to be moved to addons after build is done

LIBS   = -ldl
LIBDIR = pvr.njoy
LIB    = pvr.njoy/XBMC_Njoy.pvr

SRCS=client.cpp \
      N7Xml.cpp

include ../Makefile.include 

clean:
	-rm -f $(OBJS) $(LIB) *.P *~

INCLUDES += -I../xbmc

$(LIB): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -g $(OBJS) $(LIBS) $(LIBDIRS) $(SILIB) -o $(LIB)
