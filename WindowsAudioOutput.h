#include <stdio.h>
#include <wchar.h>
#include <tchar.h>
#include <iostream> //
#include <string> //
#include <windows.h> // for sleeping without using much cpu
#include <vector>
#include "windows.h"
#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"
#include "WindowsAudioPlaybackDevice.h"

typedef void (*ProcessAudioPlaybackDeviceCallback)(LPWSTR, LPWSTR, BOOL);  

class WindowsAudioOutput
{
public:
	WindowsAudioOutput(void);
	~WindowsAudioOutput(void);
	bool SetDefaultAudioPlaybackDeviceById(std::wstring devID);
	bool SetDefaultAudioPlaybackDeviceByIndex(UINT device_index);
	void EnumerateAudioPlaybackDevices(ProcessAudioPlaybackDeviceCallback enumerate_callback);
	std::vector<WindowsAudioPlaybackDevice> GetAudioPlaybackDevices();
};
