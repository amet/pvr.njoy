#pragma once

#include "client.h"
#include <vector>
#include "utils/StdString.h"


struct PVRChannel
{
  int                     iUniqueId;
  int                     iChannelNumber;
  std::string             strChannelName;
  std::string             strIconPath;
  std::string             strStreamURL;
  
  PVRChannel() 
  { iUniqueId      = 0;
    iChannelNumber = 0;
    strChannelName = "";
    strIconPath    = "";
    strStreamURL   = "";    
  }
};

class N7Xml
{
public:
  N7Xml(void);
  ~N7Xml(void);
  int getChannelsAmount(void);
  PVR_ERROR requestChannelList(PVR_HANDLE handle, bool bRadio);
  PVR_ERROR requestEPGForChannel(PVR_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd);
  PVR_ERROR getSignal(PVR_SIGNAL_STATUS &qualityinfo);
  void list_channels(void);
private:
  std::vector<PVRChannel> m_channels;

};

