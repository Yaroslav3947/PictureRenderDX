#include "videoplayer.h"

void DebugPrint(const char *message) { OutputDebugStringA(message); }

VideoPlayer::VideoPlayer(HWND hwmd)
    : m_reader(nullptr),
      m_renderer(nullptr),
      m_hwnd(hwmd),
      m_pSession(nullptr) {
}

//-----------------------------------------------------------------------------
// IUnknown Methods
//-----------------------------------------------------------------------------
STDMETHODIMP VideoPlayer::QueryInterface(REFIID iid, void **ppv) {
  if (ppv == nullptr) {
    return E_POINTER;
  }

  *ppv = nullptr;

  if (iid == IID_IUnknown || iid == IID_IMFAsyncCallback) {
    *ppv = static_cast<IMFAsyncCallback *>(this);
  } else if (iid == IID_IMFSourceReaderCallback) {
    *ppv = static_cast<IMFSourceReaderCallback *>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();

  return S_OK;
}

ULONG VideoPlayer::AddRef() { return InterlockedIncrement(&m_nRefCount); }

ULONG VideoPlayer::Release() {
  ULONG uCount = InterlockedDecrement(&m_nRefCount);
  if (uCount == 0) {
    delete this;
  }
  return uCount;
}

HRESULT VideoPlayer::Initialize() {
  HRESULT hr = MFStartup(MF_VERSION);
  if (FAILED(hr)) {
    return hr;
  }
  /// TODO: init or cmp with idea hw to make it all work
  m_renderer = std::make_unique<Renderer>(m_hwnd);
  if (m_renderer == nullptr) {
    return E_FAIL;
  }

  return S_OK;
}

HRESULT VideoPlayer::CloseSession() {
  HRESULT hr = S_OK;

  if (m_pSession) {
    hr = m_pSession->Stop();
    if (FAILED(hr)) {
      return hr;
    }

    hr = m_pSession->Close();
    if (FAILED(hr)) {
      return hr;
    }
  }

  return S_OK;
}

HRESULT VideoPlayer::StartPlayback() {
  PROPVARIANT varStart;
  PropVariantInit(&varStart);

  HRESULT hr = m_pSession->Start(&GUID_NULL, &varStart);
  if (SUCCEEDED(hr)) {  ////TODO: to think about enum PlayerState
                        // Playback started successfully
  }
  PropVariantClear(&varStart);
  return hr;
}

HRESULT VideoPlayer::CreateInstance(HWND hVideo, VideoPlayer **ppPlayer) {
  if (ppPlayer == nullptr) {
    return E_POINTER;
  }

  *ppPlayer = nullptr;

  VideoPlayer *pPlayer = new (std::nothrow) VideoPlayer(hVideo);
  if (pPlayer == nullptr) {
    return E_OUTOFMEMORY;
  }

  HRESULT hr = pPlayer->Initialize();
  if (FAILED(hr)) {
    delete pPlayer;
    return hr;
  }

  *ppPlayer = pPlayer;

  return S_OK;
}

HRESULT VideoPlayer::OpenURL(const WCHAR *sURL) {
  if (!sURL) return E_INVALIDARG;

  CloseSession();

  // Create the source reader
  ComPtr<IMFAttributes> pAttributes;
  HRESULT hr = MFCreateAttributes(pAttributes.GetAddressOf(), 1);

  hr = pAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);

  hr = pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

  if (SUCCEEDED(hr)) {
    hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK,
                                 static_cast<IMFSourceReaderCallback *>(this));
    if (SUCCEEDED(hr)) {
      hr = MFCreateSourceReaderFromURL(sURL, pAttributes.Get(),
                                       m_reader.GetAddressOf());
      if (SUCCEEDED(hr)) {
        hr = m_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0,
                                  nullptr, nullptr, nullptr, nullptr);
        return hr;
      }
    }
  }
  return hr;
}

//-----------------------------------------------------------------------------
// Playback Methods
//-----------------------------------------------------------------------------

HRESULT VideoPlayer::Play() {

  if (!m_pSession) {
    return E_UNEXPECTED;
  }
  return StartPlayback();
}

HRESULT VideoPlayer::Pause() {
  if (!m_pSession) return E_UNEXPECTED;

  HRESULT hr = m_pSession->Pause();
  if (SUCCEEDED(hr)) {
    // Handle successful pause operation
  }

  return hr;
}

HRESULT VideoPlayer::Stop() {
  if (!m_pSession) {
    return E_UNEXPECTED;
  }

  HRESULT hr = m_pSession->Stop();
  if (SUCCEEDED(hr)) {
    // Handle successful stop operation
  }
  return hr;
}

HRESULT VideoPlayer::Shutdown() {
  HRESULT hr = S_OK;

  hr = CloseSession();

  MFShutdown();

  if (m_pSession) {
    hr = m_pSession->Shutdown();
    if (FAILED(hr)) return hr;
  }

  return hr;
}

HRESULT VideoPlayer::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                                  DWORD dwStreamFlags, LONGLONG llTimestamp,
                                  IMFSample *pSample) {
  if (FAILED(hrStatus)) {
    return hrStatus;
  }

  if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
    return S_OK;
  }

  DebugPrint("OnReadSample()\n");

  if (pSample) {
    ID3D11Texture2D *pVideoTexture = nullptr;
    HRESULT hr = m_renderer->ExtractVideoFrame(pSample, &pVideoTexture);
    if (FAILED(hr)) {
      DebugPrint("OnReadSample():ERROR\n");
      return hr;
    }


    //hr = m_renderer->RenderVideoFrameToSwapChain(pVideoTexture);
    /*if (FAILED(hr)) {
      return hr;
    }*/

    
    //TODO: add delay

    hrStatus = m_reader->ReadSample(dwStreamIndex, 0, NULL, NULL, NULL, NULL);
    if (FAILED(hrStatus)) {
      return hrStatus;
    }
  }

  return S_OK;
}

VideoPlayer::~VideoPlayer() {
  Shutdown();
}
