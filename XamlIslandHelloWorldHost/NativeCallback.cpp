#include "stdafx.h"

MIDL_DEFINE_GUID(IID, IID_INativeCallback, 0x91b1a95b, 0xbc74, 0x4d56, 0x8b, 0x9a, 0x3, 0x24, 0x86, 0xb1, 0x8a, 0x69);

NativeCallback* NativeCallback::singleton = new NativeCallback();

// Inherited via IBlueStacksCallback
HRESULT __stdcall NativeCallback::QueryInterface(REFIID riid, void ** ppvObject)
{
	HRESULT result = E_NOINTERFACE;

	// Only support qi for IUnknonw and IBlueStackCallback.
	// check it's one of these, addref and return this
	if (riid == IID_INativeCallback || riid == IID_IUnknown)
	{
		*ppvObject = this;
		AddRef();
		result = S_OK;
	}

	return result;
}
