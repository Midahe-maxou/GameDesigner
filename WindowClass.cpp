#include "WindowClass.h"

#include <memory>
#include <chrono>
#include <map>

#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include <winerror.h>

#include <d2d1.h>

#include "WindowClass.h"
#include "EventHandler.h"
#include "GraphicComponents.h"
#include "fctdef.h"

#pragma comment(lib, "d2d1")


typedef std::chrono::steady_clock ticker;

ticker::time_point lastUpdateTime = ticker::now();


LRESULT onDestroy(_In_ HWND hwnd, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_opt_ void* context)
{
	
	BaseWindow* window = reinterpret_cast<BaseWindow*>(context);
	window->stop();
	PostQuitMessage(0);
	return S_OK;
}

LRESULT onPaint(_In_ HWND hwnd, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_opt_ void* context)
{
	BaseWindow* window = reinterpret_cast<BaseWindow*>(context);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	
	Graphics::D2D1RenderTools& rt = window->getRenderTools();
	if(!rt.isRenderTargetValid()) rt.CreateRenderTarget(hwnd);

	rt.pRenderTarget->BeginDraw();
	
	rt.pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Azure));

	// Call draw method on all drawable components.
	const ComponentMap& componentMap = window->getComponentList();
	for (auto&[zIndex, components] : componentMap)
	{
		for (auto&[component, id] : components) {
			Graphics::DrawableComponent* drawComp = dynamic_cast<Graphics::DrawableComponent*>(component.get());
			if (drawComp) drawComp->draw(hwnd, rt.pRenderTarget);
		}
	}
	
	HRESULT hr = rt.pRenderTarget->EndDraw();

	// Call reconstruct method on all drawable components if the render target gets destroyed.
	//TODO: maybe update componentMap.
	if (hr == D2DERR_RECREATE_TARGET)
	{
		rt.DestroyRenderTarget();
		window->reconstructDrawableComponents();
	}
	return hr;
}

LRESULT onResize(_In_ HWND hwnd, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_opt_ void* context)
{
	BaseWindow* window = reinterpret_cast<BaseWindow*>(context);
	Graphics::D2D1RenderTools& rt = window->getRenderTools();
	rt.DestroyRenderTarget();
	rt.CreateRenderTarget(hwnd);
	window->reconstructDrawableComponents();
	return S_OK;
}


BaseWindow::BaseWindow(_In_ const HINSTANCE hInstance, _In_ const LPCWSTR className, _In_ const LPCWSTR windowName)
	:m_classname(className)
{
	WNDCLASS wc{};
	wc.hInstance = hInstance;
	wc.lpszClassName = m_classname;
	wc.lpfnWndProc = EventHandler::handleMessage;

	RegisterClass(&wc);

	m_hwnd = CreateWindowEx(
		0,
		m_classname,				// Class name
		windowName,					// Window name
		WS_OVERLAPPEDWINDOW,		// Style

		// Location and size
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,		// Parent
		NULL,		// Menu
		hInstance,	// Instance handle
		NULL		// Additionals parameters
	);


	throwOnFail(m_renderTools.CreateFactory());
	throwOnFail(m_renderTools.CreateRenderTarget(m_hwnd));
}

void BaseWindow::show(_In_ const int nCmdShow = SW_SHOW)
{
	EventHandler::registerEvent(WM_DESTROY, onDestroy, this, PRIORITY_HIGHEST);
	EventHandler::registerEvent(CM_UPDATEFRAME, onPaint, this);
	EventHandler::registerEvent(WM_EXITSIZEMOVE, onResize, this);
	ShowWindow(m_hwnd, nCmdShow);
}

Graphics::Component* BaseWindow::getComponent(_In_ ComponentId componentId) const noexcept
{
	for (const auto& [zIndex, components] : m_components) // ZIndex zIndex, std::vector<std::pair<std::unique_ptr<Component>, ComponentId>> components.
	{
		for (auto it = components.begin(); it < components.end(); it++)
		{
			if (it->second == componentId)
			{
				return it->first.get();
			}
		}
	}
	return nullptr;
}

ComponentId BaseWindow::addComponent(_In_ std::unique_ptr<Graphics::Component>&& component, _In_opt_ int zIndex)
{
	if (component->initialize(this))
	{
		m_lastIdUsed++;
		m_components[zIndex].push_back({std::move(component), m_lastIdUsed });
	}

	return m_lastIdUsed;
}

inline void BaseWindow::reconstructDrawableComponents() noexcept
{
	for (auto& [zIndex, components] : m_components)
	{
		for (auto& [component, id] : components) {
			Graphics::DrawableComponent* drawComp = dynamic_cast<Graphics::DrawableComponent*>(component.get());
			if (drawComp) drawComp->reconstruct();
		}
	}
}

std::unique_ptr<Graphics::Component>&& BaseWindow::removeComponent(_In_ ComponentId componentId) noexcept
{
	for (auto& [zIndex, components] : m_components) // ZIndex zIndex, std::vector<std::pair<std::unique_ptr<Component>, ComponentId>> components.
	{
		for (auto it = components.begin(); it < components.end(); it++)
		{
			if (it->second == componentId)
			{
				components.erase(it); //TODO: delete unique_ptr ? Might be a bug here.
				return std::move(it->first);
			}
		}
	}
	return nullptr;
}

bool BaseWindow::setComponentZIndex(_In_ ComponentId componentId, _In_ ZIndex zIndex) noexcept
{
	std::unique_ptr<Graphics::Component> comp = removeComponent(componentId);
	if (comp == nullptr) return false;
	addComponent(std::move(comp), zIndex);
	return true;
}


//TODO: Review this ASAP!
LRESULT updateFrame(BaseWindow* window, HWND hwnd)
{
	ticker::time_point currentTime = ticker::now();
	std::chrono::milliseconds period = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime);
	if (period.count() < window->getTimeBetweenFrames())
		return S_OK;
	lastUpdateTime = currentTime;
	RECT rect;
	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, false);
	PostMessage(hwnd, CM_UPDATEFRAME, NULL, NULL);

	return S_OK;
}

const inline std::time_t BaseWindow::getTimeBetweenFrames() noexcept
{
	return m_timeBetweenFrames;
}

inline void BaseWindow::setTimeBetweenFrames(std::time_t time) noexcept
{
	m_timeBetweenFrames = time;
}

const inline unsigned int BaseWindow::getFps() noexcept
{
	return static_cast<unsigned int>(1000 / m_timeBetweenFrames);
}

inline void BaseWindow::setFps(_In_ unsigned int fps)
{
	if (fps == 0) throw std::invalid_argument("Fps cannot be zero.");

	m_timeBetweenFrames = static_cast<time_t>(1000.0f / static_cast<float>(fps));
}


void BaseWindow::mainLoop()
{
	m_isRunning = true;
	MSG msg;

	while (m_isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, true))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		updateFrame(this, m_hwnd);
	}
}

inline void BaseWindow::stop() noexcept
{
	m_isRunning = false;
}


BaseWindow::~BaseWindow()
{
	m_renderTools.~D2D1RenderTools();
}
