#pragma once

#include "resource.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::Foundation::Numerics;

const int MAX_LOADSTRING = 100;

template <typename T>
struct DesktopWindowBase
{
    static T* GetThisFromHandle(HWND const window) noexcept
    {
        return reinterpret_cast<T *>(GetWindowLongPtr(window, GWLP_USERDATA));
    }

    static LRESULT __stdcall WndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
    {
        WINRT_ASSERT(window);

        if (WM_NCCREATE == message)
        {
            auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
            T* that = static_cast<T*>(cs->lpCreateParams);
            WINRT_ASSERT(that);
            WINRT_ASSERT(!that->m_window);
            that->m_window = window;
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));

            EnableNonClientDpiScaling(window);
            m_currentDpi = GetDpiForWindow(window);
        }
        else if (T* that = GetThisFromHandle(window))
        {
            return that->MessageHandler(message, wparam, lparam);
        }

        return DefWindowProc(window, message, wparam, lparam);
    }

    LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
    {
        switch (message) {
        case WM_DPICHANGED:
        {
            auto result = HandleDpiChange(m_window, wparam, lparam);
			RECT size;
			GetClientRect(m_window, &size);
			PostMessage(m_window, WM_SIZE, 0, MAKELPARAM(size.right - size.left, size.bottom - size.top));
			return result;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lparam;
			lpMMI->ptMinTrackSize.x = 300;
			lpMMI->ptMinTrackSize.y = 300;
			return 0;
		}

        case WM_SIZE:
        {
            UINT width = LOWORD(lparam);
            UINT height = HIWORD(lparam);

            if (T* that = GetThisFromHandle(m_window))
            {
				if (width < 300) 
					width = 300;
				if (height < 300) 
					height = 300;
				that->DoResize(width, height);
            }
        }
        }

        return DefWindowProc(m_window, message, wparam, lparam);
    }

    // DPI Change handler. on WM_DPICHANGE resize the window
    LRESULT HandleDpiChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
        HWND hWndStatic = GetWindow(hWnd, GW_CHILD);
        if (hWndStatic != nullptr)
        {
            UINT uDpi = HIWORD(wParam);

            // Resize the window
            auto lprcNewScale = reinterpret_cast<RECT*>(lParam);

            SetWindowPos(hWnd, nullptr, lprcNewScale->left, lprcNewScale->top,
                lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top,
                SWP_NOZORDER | SWP_NOACTIVATE);

            if (T* that = GetThisFromHandle(hWnd))
            {
                that->NewScale(uDpi);
            }
        }
        return 0;
    }

    void NewScale(UINT dpi) {

    }

    void DoResize(UINT width, UINT height) {

    }

	HWND GetMainWindow() const
	{
		return m_window;
	}

protected:

    using base_type = DesktopWindowBase<T>;
    HWND m_window = nullptr;
    inline static UINT m_currentDpi = 0;
};


struct DesktopWindow : DesktopWindowBase<DesktopWindow>
{
    DesktopWindow() noexcept;
    ~DesktopWindow();
    void InitXaml(
        HWND wind,
        Windows::UI::Xaml::Controls::Grid & root,
        Windows::UI::Xaml::Media::ScaleTransform & dpiScale,
        DesktopWindowXamlSource & source);
    void OnSize(HWND interopHandle,
        winrt::Windows::UI::Xaml::Controls::Grid& rootGrid,
        UINT width,
        UINT height);
    LRESULT MessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
    void NewScale(UINT dpi);   
    void ApplyCorrection(double scaleFactor);
    void DoResize(UINT width, UINT height);
    void SetRootContent(Windows::UI::Xaml::UIElement content);
    Windows::UI::Xaml::FrameworkElement GetRootElement();
    Windows::UI::Xaml::UIElement GetRootElementAsUI();
    Windows::UI::Xaml::Controls::Grid GetRootElementGrid();
    void PreTranslateMessage(MSG* msg, BOOL* result);

    Windows::UI::Xaml::Controls::Grid GetDefaultContent();

private:
    UINT m_currentWidth = 592;
    UINT m_currentHeight = 333;
    HWND m_interopWindowHandle = nullptr;
    Windows::UI::Xaml::Media::ScaleTransform m_scale{ nullptr };
    Windows::UI::Xaml::Controls::Grid m_rootGrid{ nullptr };
    DesktopWindowXamlSource m_xamlSource{ nullptr };
    Windows::UI::Xaml::Hosting::WindowsXamlManager m_manager{ nullptr };
    WCHAR m_title[MAX_LOADSTRING];
    winrt::impl::com_ref<IDesktopWindowXamlSourceNative2> m_native2;
    double m_scaleFactor = 1.0;
};

