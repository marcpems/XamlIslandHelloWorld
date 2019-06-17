#include "stdafx.h"
#include "DesktopWindow.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::Foundation::Numerics;

extern "C" IMAGE_DOS_HEADER __ImageBase;

DesktopWindow::DesktopWindow() noexcept
{
    WNDCLASS wc{};
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    wc.lpszClassName = L"BlueStacks.Win32";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    RegisterClass(&wc);
    WINRT_ASSERT(!m_window);

    LoadStringW(wc.hInstance, IDS_APP_TITLE, m_title, MAX_LOADSTRING);

    WINRT_VERIFY(CreateWindow(wc.lpszClassName,
        m_title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, wc.hInstance, this));

    WINRT_ASSERT(m_window);

 //   InitXaml(m_window, m_rootGrid, m_scale, m_xamlSource);
    m_native2 = m_xamlSource.as<IDesktopWindowXamlSourceNative2>();
    NewScale(m_currentDpi);
	RECT size;
	GetClientRect(m_window, &size);
	PostMessage(m_window, WM_SIZE, 0, MAKELPARAM(size.right - size.left, size.bottom - size.top));
}

DesktopWindow::~DesktopWindow()
{
}

void DesktopWindow::PreTranslateMessage(MSG* msg, BOOL * result)
{
    m_native2->PreTranslateMessage(msg, result);
}

void DesktopWindow::InitXaml(
    HWND wind,
    Windows::UI::Xaml::Controls::Grid & root,
    Windows::UI::Xaml::Media::ScaleTransform & dpiScale,
    DesktopWindowXamlSource & source) 
{
    // Create the desktop source
    DesktopWindowXamlSource desktopSource;
    auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();
    check_hresult(interop->AttachToWindow(wind));

    // stash the interop handle so we can resize it when the main hwnd is resized
    HWND h = nullptr;
    interop->get_WindowHandle(&h);
    m_interopWindowHandle = h;

    // setup a root grid that will be used to apply DPI scaling
    Windows::UI::Xaml::Media::ScaleTransform dpiScaleTransform;
    Windows::UI::Xaml::Controls::Grid dpiAdjustmentGrid;
    dpiAdjustmentGrid.RenderTransform(dpiScaleTransform);
    Windows::UI::Xaml::Media::SolidColorBrush background{ Windows::UI::Colors::White() };

    // Set the content of the rootgrid to the DPI scaling grid
    desktopSource.Content(dpiAdjustmentGrid);

    // Update the window size, DPI layout correction
    OnSize(h, dpiAdjustmentGrid, m_currentWidth, m_currentHeight);

    // set out params
    root = dpiAdjustmentGrid;
    dpiScale = dpiScaleTransform;
    source = desktopSource;
}

void DesktopWindow::OnSize(HWND interopHandle,
    winrt::Windows::UI::Xaml::Controls::Grid& rootGrid,
    UINT width,
    UINT height) 
{
    // update the interop window size
    SetWindowPos(interopHandle, 0, 0, 0, width, height, SWP_SHOWWINDOW);
	if (m_scale != nullptr)
	{
		width = (UINT)(width / m_scaleFactor);
		height = (UINT)(height / m_scaleFactor);
	}
	rootGrid.Width(width);
    rootGrid.Height(height);
}

LRESULT DesktopWindow::MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    switch (message)
    {
        case WM_CHAR:
            OutputDebugString(L"WM_CHAR\r\n");
        break;
    }
    return base_type::MessageHandler(message, wparam, lparam);
}

void DesktopWindow::NewScale(UINT dpi) 
{
	m_scaleFactor = (float)dpi / 96;
}

void DesktopWindow::DoResize(UINT width, UINT height) 
{
    m_currentWidth = width;
    m_currentHeight = height;
    if (nullptr != m_rootGrid) {
        OnSize(m_interopWindowHandle, m_rootGrid, m_currentWidth, m_currentHeight);
    }
}

void DesktopWindow::SetRootContent(Windows::UI::Xaml::UIElement content) 
{
    m_rootGrid.Children().Clear();
    m_rootGrid.Children().Append(content);
}

Windows::UI::Xaml::FrameworkElement DesktopWindow::GetRootElement() 
{
    return m_rootGrid;
}

Windows::UI::Xaml::UIElement DesktopWindow::GetRootElementAsUI()
{
    return m_rootGrid;
}

Windows::UI::Xaml::Controls::Grid DesktopWindow::GetRootElementGrid() 
{
    return m_rootGrid;
}

Windows::UI::Xaml::Controls::Grid DesktopWindow::GetDefaultContent()
{
    Windows::UI::Xaml::Media::AcrylicBrush acrylicBrush;
    acrylicBrush.BackgroundSource(Windows::UI::Xaml::Media::AcrylicBackgroundSource::HostBackdrop);
    acrylicBrush.TintOpacity(0.5);
    acrylicBrush.TintColor(Windows::UI::Colors::Red());

    Windows::UI::Xaml::Controls::Grid container;
    container.Background(acrylicBrush);
    return container;
}
