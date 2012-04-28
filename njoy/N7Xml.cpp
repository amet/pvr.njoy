

#include "N7Xml.h"
#include "filesystem/FileCurl.h"
#include "utils/log.h"
#include "tinyXML/tinyxml.h"
#include "utils/RegExp.h"
#include "utils/XMLUtils.h"


using namespace XFILE;


N7Xml::N7Xml(void)
{
  list_channels();
  bool m_connected = false;
}

N7Xml::~N7Xml(void)
{
  m_channels.clear();
}

int N7Xml::getChannelsAmount()
{ 
	return m_channels.size();
}

void N7Xml::list_channels()
{
  CStdString strUrl;
  strUrl.Format("http://%s:%i/n7channel_nt.xml", g_strHostname.c_str(), g_iPort);
  CStdString strXML;
  
  CFileCurl http;
  
  http.SetTimeout(5);
  if(!http.Get(strUrl, strXML))
  {
    CLog::Log(LOGDEBUG, "N7Xml - Could not open connection to N7 backend.");
  }
  else
  {
    m_connected = true;
    CLog::Log(LOGDEBUG, "N7Xml - Connected to N7 backend.");    
    TiXmlDocument xml;
    xml.Parse(strXML.c_str());
    TiXmlElement* rootXmlNode = xml.RootElement();
    TiXmlElement* channelsNode = rootXmlNode->FirstChildElement("channel");
    if (channelsNode)
    {
      int iUniqueChannelId = 0;
      TiXmlNode *pChannelNode = NULL;
      while ((pChannelNode = channelsNode->IterateChildren(pChannelNode)) != NULL)
      {  
        CStdString strTmp;
        PVRChannel channel;
        
        /* unique ID */
        channel.iUniqueId = ++iUniqueChannelId;
        
        /* channel number */
        if (!XMLUtils::GetInt(pChannelNode, "number", channel.iChannelNumber))
          channel.iChannelNumber = channel.iUniqueId;
        
        /* channel name */
        if (!XMLUtils::GetString(pChannelNode, "title", strTmp))
          continue;
        channel.strChannelName = strTmp;
        
        /* icon path */
        const TiXmlElement* pElement = pChannelNode->FirstChildElement("media:thumbnail");
        channel.strIconPath = pElement->Attribute("url");
        
        /* channel url */
        if (!XMLUtils::GetString(pChannelNode, "guid", strTmp))
          channel.strStreamURL = "";
        else
          channel.strStreamURL = strTmp;
        
        m_channels.push_back(channel);
      }
    }  
  }
}


PVR_ERROR N7Xml::requestChannelList(PVR_HANDLE handle, bool bRadio)
{
  if (m_connected)
  {  
    std::vector<PVRChannel>::const_iterator item;
    PVR_CHANNEL tag;
    for( item = m_channels.begin(); item != m_channels.end(); ++item)
    {
      const PVRChannel& ch = *item; 
      memset(&tag, 0 , sizeof(tag));
      tag.iUniqueId       = ch.iUniqueId;
      tag.iChannelNumber  = ch.iChannelNumber;
      tag.strChannelName  = ch.strChannelName.c_str();
      tag.bIsRadio        = false;
      tag.strInputFormat  = "";
      tag.strStreamURL    = ch.strStreamURL.c_str();
      tag.strIconPath     = ch.strIconPath.c_str();
      tag.bIsHidden       = false;
      CLog::Log(LOGDEBUG, "N7Xml - Loaded channel - %s.", tag.strChannelName);
      PVR->TransferChannelEntry(handle, &tag);   
    }
  }
  else
  {
    CLog::Log(LOGDEBUG, "N7Xml - no channels loaded");
  }

	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::requestEPGForChannel(PVR_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd)
{
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR N7Xml::getSignal(PVR_SIGNAL_STATUS &qualityinfo)
{
	return PVR_ERROR_NO_ERROR;
}

