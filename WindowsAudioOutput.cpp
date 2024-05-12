#include "WindowsAudioOutput.h"

bool SetDefaultOutputByVectorIndex(int i, const std::vector<WindowsAudioPlaybackDevice>& deviceVector, WindowsAudioOutput& audioOutput);

WindowsAudioOutput::WindowsAudioOutput(void)
{
}

WindowsAudioOutput::~WindowsAudioOutput(void)
{
}

bool WindowsAudioOutput::SetDefaultAudioPlaybackDeviceById(std::wstring device_id)
{	
	IPolicyConfigVista *pPolicyConfig;
	ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);

	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(device_id.c_str(), reserved);
		pPolicyConfig->Release();
	}

	return SUCCEEDED(hr);
}

bool WindowsAudioOutput::SetDefaultAudioPlaybackDeviceByIndex(UINT device_index)
{
	HRESULT hr = CoInitialize(NULL);
	bool result = false;

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			IMMDeviceCollection *pDevices;
			
			hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);

			if (SUCCEEDED(hr))
			{
				UINT count;

				pDevices->GetCount(&count);

				if (SUCCEEDED(hr))
				{
					if (device_index >= 0 && device_index < count)
					{
						IMMDevice *pDevice;
						hr = pDevices->Item(device_index, &pDevice);

						if (SUCCEEDED(hr))
						{
							LPWSTR wstrID = NULL;

							hr = pDevice->GetId(&wstrID);

							if (SUCCEEDED(hr))
							{
								IPropertyStore *pStore;
								hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);

								if (SUCCEEDED(hr))
								{
									PROPVARIANT friendlyName;

									PropVariantInit(&friendlyName);
									hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

									if (SUCCEEDED(hr))
									{
										SetDefaultAudioPlaybackDeviceById(wstrID);
										PropVariantClear(&friendlyName);

										result = true;
									}

									pStore->Release();
								}
							}

							pDevice->Release();
						}
					}
				}

				pDevices->Release();
			}

			pEnum->Release();
		}
	}

	return result;
}

void WindowsAudioOutput::EnumerateAudioPlaybackDevices(ProcessAudioPlaybackDeviceCallback enumerate_callback)
{
	HRESULT hr = CoInitialize(NULL);

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			LPWSTR defaultDeviceId = NULL;
			IMMDevice *pDefaultDevice;

			HRESULT hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);

			if (SUCCEEDED(hr))
			{
				hr = pDefaultDevice->GetId(&defaultDeviceId);

				if (!SUCCEEDED(hr))
				{
					defaultDeviceId = NULL;
				}

				pDefaultDevice->Release();
			}

			IMMDeviceCollection *pDevices;
			
			hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);

			if (SUCCEEDED(hr))
			{
				UINT count;

				pDevices->GetCount(&count);

				if (SUCCEEDED(hr))
				{
					for (UINT i = 0; i < count; ++i)
					{
						IMMDevice *pDevice;
						HRESULT hr = pDevices->Item(i, &pDevice);

						if (SUCCEEDED(hr))
						{
							LPWSTR wstrID = NULL;

							hr = pDevice->GetId(&wstrID);

							if (SUCCEEDED(hr))
							{
								IPropertyStore *pStore;
								hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);

								if (SUCCEEDED(hr))
								{
									PROPVARIANT friendlyName;

									PropVariantInit(&friendlyName);
									hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

									if (SUCCEEDED(hr))
									{
										BOOL isDefault = wcscmp(wstrID, defaultDeviceId) == 0;

										enumerate_callback(wstrID, friendlyName.pwszVal, isDefault);
										PropVariantClear(&friendlyName);
									}

									pStore->Release();
								}
							}

							pDevice->Release();
						}
					}
				}

				pDevices->Release();
			}

			pEnum->Release();
		}
	}
}

std::vector<WindowsAudioPlaybackDevice> WindowsAudioOutput::GetAudioPlaybackDevices()
{
	HRESULT hr = CoInitialize(NULL);
	std::vector<WindowsAudioPlaybackDevice> devices;

	if (SUCCEEDED(hr))
	{
		IMMDeviceEnumerator *pEnum = NULL;
		
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&pEnum);
		
		if (SUCCEEDED(hr))
		{
			LPWSTR defaultDeviceId = NULL;
			IMMDevice *pDefaultDevice;

			HRESULT hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);

			if (SUCCEEDED(hr))
			{
				hr = pDefaultDevice->GetId(&defaultDeviceId);

				if (!SUCCEEDED(hr))
				{
					defaultDeviceId = NULL;
				}

				pDefaultDevice->Release();
			}

			IMMDeviceCollection *pDevices;
			
			hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);

			if (SUCCEEDED(hr))
			{
				UINT count;

				pDevices->GetCount(&count);

				if (SUCCEEDED(hr))
				{
					for (UINT i = 0; i < count; ++i)
					{
						IMMDevice *pDevice;
						HRESULT hr = pDevices->Item(i, &pDevice);

						if (SUCCEEDED(hr))
						{
							LPWSTR wstrID = NULL;

							hr = pDevice->GetId(&wstrID);

							if (SUCCEEDED(hr))
							{
								IPropertyStore *pStore;
								hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);

								if (SUCCEEDED(hr))
								{
									PROPVARIANT friendlyName;

									PropVariantInit(&friendlyName);
									hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

									if (SUCCEEDED(hr))
									{
										WindowsAudioPlaybackDevice device;

										device.id = wstrID;
										device.name = friendlyName.pwszVal;
										device.mydefault = wcscmp(wstrID, defaultDeviceId) == 0;
										
										devices.push_back(device);

										PropVariantClear(&friendlyName);
									}

									pStore->Release();
								}
							}

							pDevice->Release();
						}
					}
				}

				pDevices->Release();
			}

			pEnum->Release();
		}
	}

	return devices;
}

int main() {
	// create instance of WindowsAudioOutput
	WindowsAudioOutput audioOutput; 

	// get audio output devices
	std::vector<WindowsAudioPlaybackDevice> playbackDevices = audioOutput.GetAudioPlaybackDevices();

	// print info
	int tempIndex = 0;
	int currentlyActiveDeviceVectorIndex = -1;
	int amountDevices = playbackDevices.size();

	std::wcout << L"-------------------------\nAvailable output devices:\n" << std::endl;
	for (const auto& device : playbackDevices) {
		std::wcout << L"\tVector index: " << tempIndex << std::endl;
		std::wcout << L"\tDevice ID: " << device.id << std::endl;
		std::wcout << L"\tDevice Name: " << device.name << std::endl;
		// remember vector index of current default device
		if (device.mydefault) {
			currentlyActiveDeviceVectorIndex = tempIndex;
		}
		std::wcout << L"\tIs Default: " << (device.mydefault ? L"Yes" : L"No") << std::endl;
		std::wcout << std::endl;

		// increase current index
		tempIndex++;
	}
	std::wcout << L"-------------------------" << std::endl;

	// ensure you found default device
	if (currentlyActiveDeviceVectorIndex == -1) {
		std::wcout << L"Failed to detect current output device. Terminating." << std::endl;
		return -1;
	}

	//std::wcout << L"Current default index: " << currentlyActiveDeviceVectorIndex << std::endl;
	//std::wcout << L"Total amount of sound devices: " << amountDevices << std::endl;

	// jump to next output device if "End" button is pressed
	while (true) {
		if (GetAsyncKeyState(VK_END) & 0x8000) {
			// EVENT: END key was pressed so jump to next output device
			//		1. get target device index
			int newValidIndex = (currentlyActiveDeviceVectorIndex + 1) % amountDevices; // ensure we cycle only between existing device indices
			//		2. swap output device to this device
			bool success = SetDefaultOutputByVectorIndex(newValidIndex, playbackDevices, audioOutput);
			if (success) {
				std::wcout << L"Succcessfully changed output device!\n" << std::endl;
				// update current output device index
				currentlyActiveDeviceVectorIndex = newValidIndex;
			}
			else {
				std::wcout << L"Failed to change output device! Terminating!" << std::endl;
				return -1;
			}
			//std::cout << "END key pressed" << std::endl;
			
			Sleep(200); // dont register press as multiple presses
		}
		Sleep(10); // dont kill cpu
	}

	// let program running without using much cpu
	//Sleep(INFINITE);

	return 0;
}

bool SetDefaultOutputByVectorIndex(int i, const std::vector<WindowsAudioPlaybackDevice>& deviceVector, WindowsAudioOutput& audioOutput) {
	const auto& device = deviceVector[i];
	std::wcout << L"Will now change output to device: " << device.name << std::endl;
	bool success = audioOutput.SetDefaultAudioPlaybackDeviceById(device.id);
	return success;
}
