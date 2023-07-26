#pragma once 

#include "WinInclude.h"
#include "Renderer.h"

class VideoPlayer : public IMFAsyncCallback, public IMFSourceReaderCallback {
 public:
  static HRESULT CreateInstance(HWND hwnd, VideoPlayer **ppPlayer);
  // Playback
  HRESULT OpenURL(const WCHAR *sURL);
  HRESULT Play();
  HRESULT Pause();
  HRESULT Stop();
  HRESULT Shutdown();

  // IUnknown methods
  STDMETHODIMP QueryInterface(REFIID iid, void **ppv) override;
  STDMETHODIMP_(ULONG) AddRef() override;
  STDMETHODIMP_(ULONG) Release() override;

  // IMFAsyncCallback methods
  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *) override { return S_OK; }
  STDMETHODIMP OnFlush(DWORD) override { return S_OK; }
  STDMETHODIMP
  GetParameters(DWORD *, DWORD *) override {  ////TODO: complete this method
    return E_NOTIMPL;
  }
  STDMETHODIMP
  Invoke(IMFAsyncResult *pResult) override {  ////TODO: complete this method
    return S_OK;
  }

  // IMFSourceReaderCallback methods
  HRESULT OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                       DWORD dwStreamFlags, LONGLONG llTimestamp,
                       IMFSample *pSample) override;

  virtual ~VideoPlayer();

 protected:
  VideoPlayer(HWND hwnd);

  HRESULT Initialize();
  HRESULT CloseSession();
  //    HRESULT CreateSession();
  HRESULT StartPlayback();

 protected:
  ComPtr<IMFSourceReader> m_reader;
  std::unique_ptr<Renderer> m_renderer;

  long m_nRefCount;
  HWND m_hwnd;  // Video window.

  ComPtr<IMFMediaSession> m_pSession;
};