#pragma once

class INativeCallback : public IUnknown
{
public:
	virtual void ExitHostApp() = 0;
};


class NativeCallback : public INativeCallback
{
private:
	int m_ref = 0;
	HWND m_mainWindow = 0;
	static NativeCallback* singleton;

	NativeCallback()
	{
		m_ref = 1;
	}

public:

	static NativeCallback* Instance()
	{
		return singleton;
	}

	void Init(HWND mainWindow)
	{
		m_mainWindow = mainWindow;
	}

	// Inherited via IBlueStacksCallback
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void ** ppvObject) override;

	virtual ULONG __stdcall AddRef(void) override
	{
		m_ref++;
		return m_ref;
	}

	virtual ULONG __stdcall Release(void) override
	{
		m_ref--;

		return m_ref;
	}

	// Inherited via IBlueStacksCallback
	virtual void ExitHostApp() override
	{
		// Exit the app
		if (m_mainWindow)
		{
			::PostMessage(m_mainWindow, WM_CLOSE, 0, 0);
		}
	}
};
