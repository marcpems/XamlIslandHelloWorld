// XamlIslandHelloWorldHost.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "XamlIslandHelloWorldHost.h"
#include "DesktopWindow.h"
#include <appmodel.h>

MIDL_DEFINE_GUID(IID, IID_IBroker, 0x8cf564c9, 0x2017, 0x4ee2, 0xa9, 0x5e, 0x20, 0x3b, 0x17, 0x9a, 0x12, 0xcd);
MIDL_DEFINE_GUID(CLSID, CLSID_Broker, 0x9ee8915b, 0x53c5, 0x4ee6, 0x91, 0x48, 0x20, 0x1f, 0x79, 0x0e, 0xac, 0x1a);

const wchar_t *AppModuleName = L"XamlIslandHelloWorld.dll";

// Broker class interface predefinition definition 
class IBroker : public IUnknown
{
public:
	virtual void Initialize(INativeCallback * callback) = 0;
	virtual Windows::UI::Xaml::UIElement  __stdcall GetUI() = 0;
	virtual void GetGoing(bool isPackaged) = 0;
};

IBroker* pBroker = nullptr;

template <typename T>
T convert_from_abi(::IUnknown* from)
{
	T to{ nullptr };

	winrt::check_hresult(from->QueryInterface(winrt::guid_of<T>(),
		reinterpret_cast<void**>(winrt::put_abi(to))));

	return to;
}

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

std::wstring ExePath()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

bool IsPackaged()
{
	bool returnValue = false;

	UINT32 length = 0;
	LONG rc = GetCurrentPackageFullName(&length, NULL);
	if (rc == ERROR_INSUFFICIENT_BUFFER)
	{
		PWSTR fullName = (PWSTR)malloc(length * sizeof(*fullName));
		if (fullName != NULL)
		{
			rc = GetCurrentPackageFullName(&length, fullName);
			if (rc == ERROR_SUCCESS)
			{
				returnValue = true;
			}
			free(fullName);
		}
	}

	return returnValue;
}

// Load the main application DLL, find the broker entry and prepare for application start
int LoadAppCode()
{
	HMODULE hControlApp = LoadLibraryEx(AppModuleName, NULL, NULL);
	if (hControlApp == nullptr)
	{
		auto errormsg = GetLastErrorAsString();
		MessageBox(nullptr, L"Failed to load C# component. Are the dependency files in place?", L"ERROR loading HostUWPApp.dll", 0);
		return 1;
	}

	// Now create the required UI object
	FnDllGetClassObject fnDllGetClassObject = (FnDllGetClassObject)GetProcAddress(hControlApp, "DllGetClassObject");

	IClassFactory* pClassFactory;

	HRESULT hr = fnDllGetClassObject(CLSID_Broker, __uuidof(IClassFactory), (void**)&pClassFactory);
	if (hr != S_OK)
	{
		//        cout << "Error creating class factory" << endl;
		return 1;
	}

	hr = pClassFactory->CreateInstance(nullptr, IID_IBroker, (void**)&pBroker);
	if (hr != S_OK)
	{
		//        cout << "Error obtaining interface pointer" << endl;
		return 1;
	}

	return 0;
}

// Main app entry point
int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	//init_apartment(apartment_type::multi_threaded);

	//// Request PMv2
	//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//if (LoadAppCode())
	//{
	//	// error starting. Exit the app
	//	return 1;
	//}

	//pBroker->Initialize(NativeCallback::Instance());
	DesktopWindow window;
	NativeCallback::Instance()->Init(window.GetMainWindow());

	//window.GetRootElement().Loaded([&](winrt::Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&) {
	//	bool isPackaged = IsPackaged();

	//	// start loading things
	//	pBroker->GetGoing(isPackaged);

	//	auto container = pBroker->GetUI();

	//	/// Add the content to the UI Island element 
	//	auto grid = window.GetRootElementGrid();
	//	grid.Children().Clear();
	//	grid.Children().Append(container);
	//	});

    MSG message;
    BOOL result = FALSE;

    while (GetMessage(&message, nullptr, 0, 0))
    {
//        window.PreTranslateMessage(&message, &result);
        if (!result)
        {
            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
        }
    }
	return 0;
}
