#pragma once

#ifdef _WIN32

#include "AudioPlayer.h"
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

using namespace Microsoft::WRL;

namespace Ship {
	class WasapiAudioPlayer : public AudioPlayer, public IMMNotificationClient {
	public:
		WasapiAudioPlayer() : refcount(1), bufferFrameCount(0), initialized(false), started(false) {  };

		bool Init(void);
		int Buffered(void);
		int GetDesiredBuffered(void);
		void Play(const uint8_t* Buffer, uint32_t BufferLen);

	protected:
		virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
		virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
		virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
		virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId);
		virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);
		void ThrowIfFailed(HRESULT res);
		bool SetupStream(void);

	private:
		ComPtr<IMMDeviceEnumerator> DeviceEnumerator;
		ComPtr<IMMDevice> device;
		ComPtr<IAudioClient> client;
		ComPtr<IAudioRenderClient> rclient;
		LONG refcount;
		UINT32 bufferFrameCount;
		bool initialized;
		bool started;
	};
}
#endif
