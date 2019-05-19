#include "Window.h"
#include <sstream>
#include "resource.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(NULL))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// Window stuff
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	// calculate window size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if ((AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)) == 0)
	{
		throw(WND_LAST_EXCEPT());
	}
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		NULL, NULL, WindowClass::GetInstance(), this
	);

	// check window handle
	if (hWnd == NULL)
	{
		throw WND_LAST_EXCEPT();
	}
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitile(const std::string & title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	// while queue has messages, remove and dispatch them
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// check for quit message 
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		// Translate and dispatch message
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics & Window::Gfx()
{
	return *pGfx;
}

LRESULT CALLBACK Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from createWindow() to store window 
	// class pointer
	if (msg == WM_NCCREATE)
	{
		//extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WINAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (no-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
		// forward mesage to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);	
}

LRESULT CALLBACK Window::HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window class 
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		keyBoard.ClearState();
		break;

		/* ------------Keyboard messages-------------*/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || keyBoard.AutorepeatIsEnable())
		{
			keyBoard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyBoard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		keyBoard.OnChar(static_cast<unsigned char>(wParam));
		break;
		/* ------------Keyboard messages-------------*/

		/* ------------Mouse messages-------------*/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// int client region -> log move and log enter + capture mouse
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.isInWindow)
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client ->log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}

		mouse.OnMouseMove(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);

		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
		/* ------------Mouse messages-------------*/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// --------------------Window Exception Stuff-----------------------
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	ExceptionClass(line, file),
	hr(hr)
{

}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, NULL
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	// free windows buffer
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}