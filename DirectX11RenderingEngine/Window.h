#pragma once
#include "Win.h"
#include "ExceptionClass.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <optional>

class Window
{
public:
	class Exception : public ExceptionClass
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	// singleton manages registraction/cleanup of window class
	class WindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "DirectX11 Rendering Engine";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitile(const std::string& title);
	static std::optional<int> ProcessMessages();
private:
	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard keyBoard;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
};


// error exception helper macro
#define WND_EXCEPT(hr) (Window::Exception(__LINE__, __FILE__, hr))
#define WND_LAST_EXCEPT() (Window::Exception(__LINE__, __FILE__, GetLastError()))