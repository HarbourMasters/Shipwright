#ifdef _WIN32
#include "WasapiAudioPlayer.h"
#include "spdlog/spdlog.h"

// These constants are currently missing from the MinGW headers.
#ifndef AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
# define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM  0x80000000
#endif
#ifndef AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY
# define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY  0x08000000
#endif

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

namespace Ship {
	void WasapiAudioPlayer::ThrowIfFailed(HRESULT res) {
		if (FAILED(res)) {
			throw res;
		}
	}

	bool WasapiAudioPlayer::SetupStream(void) {
		try {
			ThrowIfFailed(DeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device));
			ThrowIfFailed(device->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, IID_PPV_ARGS_Helper(&client)));

			WAVEFORMATEX desired;
			desired.wFormatTag = WAVE_FORMAT_PCM;
			desired.nChannels = 2;
			desired.nSamplesPerSec = 32000;
			desired.nAvgBytesPerSec = 32000 * 2 * 2;
			desired.nBlockAlign = 4;
			desired.wBitsPerSample = 16;
			desired.cbSize = 0;

			ThrowIfFailed(client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, 2000000, 0, &desired, nullptr));

			ThrowIfFailed(client->GetBufferSize(&bufferFrameCount));
			ThrowIfFailed(client->GetService(IID_PPV_ARGS(&rclient)));

			started = false;
			initialized = true;
		}
		catch (HRESULT res) {
			return false;
		}

		return true;
	}

	bool WasapiAudioPlayer::Init(void) {
		try {
			ThrowIfFailed(CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&DeviceEnumerator)));
		}
		catch (HRESULT res) {
			return false;
		}

		ThrowIfFailed(DeviceEnumerator->RegisterEndpointNotificationCallback(this));

		return true;
	}

	int WasapiAudioPlayer::Buffered(void) {
		if (!initialized) {
			if (!SetupStream()) {
				return 0;
			}
		}
		try {
			UINT32 padding;
			ThrowIfFailed(client->GetCurrentPadding(&padding));
			return padding;
		}
		catch (HRESULT res) {
			return 0;
		}
	}

	int WasapiAudioPlayer::GetDesiredBuffered(void) {
		return 1680;
	}

	void WasapiAudioPlayer::Play(const uint8_t* Buffer, uint32_t BufferLen) {
		if (!initialized) {
			if (!SetupStream()) {
				return;
			}
		}
		try {
			UINT32 frames = BufferLen / 4;

			UINT32 padding;
			ThrowIfFailed(client->GetCurrentPadding(&padding));

			UINT32 available = bufferFrameCount - padding;
			if (available < frames) {
				frames = available;
			}
			if (available == 0) {
				return;
			}

			BYTE* data;
			ThrowIfFailed(rclient->GetBuffer(frames, &data));
			memcpy(data, Buffer, frames * 4);
			ThrowIfFailed(rclient->ReleaseBuffer(frames, 0));

			if (!started && padding + frames > 1500) {
				started = true;
				ThrowIfFailed(client->Start());
			}
		}
		catch (HRESULT res) {
		}
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::OnDeviceAdded(LPCWSTR pwstrDeviceId) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) {
		if (flow == eRender && role == eConsole) {
			// This callback runs on a separate thread,
			// but it's not important how fast this write takes effect.
			initialized = false;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE WasapiAudioPlayer::AddRef() {
		return InterlockedIncrement(&refcount);
	}

	ULONG STDMETHODCALLTYPE WasapiAudioPlayer::Release() {
		ULONG rc = InterlockedDecrement(&refcount);
		if (rc == 0) {
			delete this;
		}
		return rc;
	}

	HRESULT STDMETHODCALLTYPE WasapiAudioPlayer::QueryInterface(REFIID riid, VOID** ppvInterface) {
		if (riid == __uuidof(IUnknown)) {
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (riid == __uuidof(IMMNotificationClient)) {
			AddRef();
			*ppvInterface = (IMMNotificationClient*)this;
		}
		else {
			*ppvInterface = nullptr;
			return E_NOINTERFACE;
		}
		return S_OK;
	}
}
#endif