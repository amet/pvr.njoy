/*
 *      Copyright (C) 2005-2012 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "client.h"
#include "xbmc_pvr_dll.h"
#include "N7Xml.h"


using namespace std;
using namespace ADDON;

/* User adjustable settings are saved here.
 * Default values are defined inside client.h
 * and exported to the other source files.
 */
CStdString   g_strHostname             = DEFAULT_HOST;
int          g_iPort                   = DEFAULT_PORT;

///* Client member variables */

int          m_iCurrentChannel        = 1;
ADDON_STATUS m_CurStatus              = ADDON_STATUS_UNKNOWN;
bool         g_bCreated               = false;
int          g_iClientID              = -1;
CStdString   g_szUserPath             = "";
CStdString   g_szClientPath           = "";
N7Xml		     *m_data			            = NULL;
CHelper_libXBMC_addon *XBMC           = NULL;
CHelper_libXBMC_pvr   *PVR            = NULL;


extern "C" {

/***********************************************************
 * Standard AddOn related public library functions
 ***********************************************************/

ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  if (!props)
    return ADDON_STATUS_UNKNOWN;

  PVR_PROPERTIES* pvrprops = (PVR_PROPERTIES*)props;

  XBMC = new CHelper_libXBMC_addon;
  if (!XBMC->RegisterMe(hdl))
    return ADDON_STATUS_UNKNOWN;

  PVR = new CHelper_libXBMC_pvr;
  if (!PVR->RegisterMe(hdl))
    return ADDON_STATUS_UNKNOWN;
  XBMC->Log(LOG_DEBUG, "Creating N7 PVR-Client");

  m_CurStatus    = ADDON_STATUS_UNKNOWN;
  g_iClientID    = pvrprops->iClientId;
  g_szUserPath   = pvrprops->strUserPath;
  g_szClientPath = pvrprops->strClientPath;

  /* Read setting "host" from settings.xml */
  char * buffer;
  buffer = (char*) malloc (1024);
  buffer[0] = 0; /* Set the end of string */

  if (XBMC->GetSetting("n7host", buffer))
  {  
    g_strHostname = buffer;
    XBMC->Log(LOG_ERROR, "n7host set to - %s", g_strHostname.c_str());
  }
  else
  {
    /* If setting is unknown fallback to defaults */
    XBMC->Log(LOG_INFO, "Couldn't get 'host' setting, falling back to '%s' as default", DEFAULT_HOST);
    g_strHostname = DEFAULT_HOST;
  }
  free (buffer);

  /* Read setting "port" from settings.xml */
  if (!XBMC->GetSetting("n7port", &g_iPort))
  {
    /* If setting is unknown fallback to defaults */
    XBMC->Log(LOG_ERROR, "Couldn't get 'n7port' setting, falling back to '%i' as default", DEFAULT_PORT);
    g_iPort = DEFAULT_PORT;
  }
  
  m_data = new N7Xml;
  g_bCreated = true;
  m_CurStatus = ADDON_STATUS_OK;
  return m_CurStatus;
}

void ADDON_Destroy()
{
  if (g_bCreated)
  {
    g_bCreated = false;
  }
  m_CurStatus = ADDON_STATUS_UNKNOWN;
}

ADDON_STATUS ADDON_GetStatus()
{
  return m_CurStatus;
}

bool ADDON_HasSettings()
{
  return true;
}

unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}

ADDON_STATUS ADDON_SetSetting(const char *settingName, const void *settingValue)
{
  string str = settingName;
  if (str == "n7host")
  {
    string tmp_sHostname;
    XBMC->Log(LOG_INFO, "Changed Setting 'host' from %s to %s", g_strHostname.c_str(), (const char*) settingValue);
    tmp_sHostname = g_strHostname;
    g_strHostname = (const char*) settingValue;
    if (tmp_sHostname != g_strHostname)
      return ADDON_STATUS_NEED_RESTART;
  }
  else if (str == "n7port")
  {
    XBMC->Log(LOG_INFO, "Changed Setting 'port' from %u to %u", g_iPort, *(int*) settingValue);
    if (g_iPort != *(int*) settingValue)
    {
	  g_iPort = *(int*) settingValue;
      return ADDON_STATUS_NEED_RESTART;
    }
  }
  return ADDON_STATUS_OK;
}

void ADDON_Stop()
{
  return;
}

void ADDON_FreeSettings()
{
  return;
}


/***********************************************************
 * PVR Client AddOn specific public library functions
 ***********************************************************/

PVR_ERROR GetAddonCapabilities(PVR_ADDON_CAPABILITIES *pCapabilities)
{
  pCapabilities->bSupportsTimeshift          = false;
  pCapabilities->bSupportsEPG                = false;
  pCapabilities->bSupportsRecordings         = false;
  pCapabilities->bSupportsTimers             = false;
  pCapabilities->bSupportsTV                 = true;
  pCapabilities->bSupportsRadio              = false;
  pCapabilities->bSupportsChannelSettings    = true;
  pCapabilities->bSupportsChannelGroups      = false;
  pCapabilities->bHandlesInputStream         = true;
  pCapabilities->bHandlesDemuxing            = false;
  pCapabilities->bSupportsChannelScan        = false;

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR GetStreamProperties(PVR_STREAM_PROPERTIES* props)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

const char * GetBackendName()
{
  return "";
}

const char * GetBackendVersion()
{
  return "";
}

const char * GetConnectionString()
{
  return "";
}

PVR_ERROR GetDriveSpace(long long *iTotal, long long *iUsed)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR GetBackendTime(time_t *localTime, int *gmtOffset)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DialogChannelScan()
{
  return PVR_ERROR_NOT_POSSIBLE;
}

PVR_ERROR CallMenuHook(const PVR_MENUHOOK &menuhook)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

/*******************************************/
/** PVR EPG Functions                     **/

PVR_ERROR GetEPGForChannel(PVR_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

/*******************************************/
/** PVR Channel Functions                 **/

int GetChannelsAmount(void)
{
	return m_data->getChannelsAmount();
}

PVR_ERROR GetChannels(PVR_HANDLE handle, bool bRadio)
{
	return m_data->requestChannelList(handle, bRadio);
}

int GetChannelGroupsAmount(void)
{
  return 0;
}

PVR_ERROR GetChannelGroups(PVR_HANDLE handle, bool bRadio)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR GetChannelGroupMembers(PVR_HANDLE handle, const PVR_CHANNEL_GROUP &group)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DeleteChannel(const PVR_CHANNEL &channel)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR RenameChannel(const PVR_CHANNEL &channel)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR MoveChannel(const PVR_CHANNEL &channel)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DialogChannelSettings(const PVR_CHANNEL &channel)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DialogAddChannel(const PVR_CHANNEL &channel)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}


/*******************************************/
/** PVR Recording Functions               **/

int GetRecordingsAmount(void)
{
  return 0;
}

PVR_ERROR GetRecordings(PVR_HANDLE handle)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING &recording)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR RenameRecording(const PVR_RECORDING &recording)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

/*******************************************/
/** PVR Timer Functions                   **/

int GetTimersAmount(void)
{
  return 0;
}

PVR_ERROR GetTimers(PVR_HANDLE handle)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR AddTimer(const PVR_TIMER &timer)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR DeleteTimer(const PVR_TIMER &timer, bool bForceDelete)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR UpdateTimer(const PVR_TIMER &timer)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}


/*******************************************/
/** PVR Live Stream Functions             **/

bool OpenLiveStream(const PVR_CHANNEL &channel)
{
  return false;
}

void CloseLiveStream(void)
{
  return;
}

int ReadLiveStream(unsigned char *pBuffer, unsigned int iBufferSize)
{
	return -1;
}

int GetCurrentClientChannel()
{
  return m_iCurrentChannel;
}

bool SwitchChannel(const PVR_CHANNEL &channelinfo)
{
  return false;
}

PVR_ERROR SignalStatus(PVR_SIGNAL_STATUS &signalStatus)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}


/*******************************************/
/** PVR Recording Stream Functions        **/

bool OpenRecordedStream(const PVR_RECORDING &recinfo)
{
  return false;
}

void CloseRecordedStream(void)
{
  return;
}

int ReadRecordedStream(unsigned char *pBuffer, unsigned int iBufferSize)
{
  return 0;
}

long long SeekRecordedStream(long long iPosition, int iWhence)
{
	return -1;
}

long long PositionRecordedStream(void)
{
  return -1;
}

long long LengthRecordedStream(void)
{
	return 0;
}


/** UNUSED API FUNCTIONS */
DemuxPacket* DemuxRead() { return NULL; }
void DemuxAbort() {}
void DemuxReset() {}
void DemuxFlush() {}
long long SeekLiveStream(long long iPosition, int iWhence) { return -1; }
long long PositionLiveStream(void) { return -1; }
long long LengthLiveStream(void) { return -1; }
const char * GetLiveStreamURL(const PVR_CHANNEL &channelinfo) { return ""; }

} //end extern "C"
