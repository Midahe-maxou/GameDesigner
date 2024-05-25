#pragma once
#ifndef WINDOWCLASS_H
#define WINDOWCLASS_H

#include "EventHandler.h"

#include <map>
#include <vector>
#include <stdexcept>
#include <memory>

#include <Windows.h>
#include <WinBase.h>
#include <WinUser.h>
#include <winerror.h>

#include <corecrt.h> // time_t

#include <d2d1.h>

#include "GraphicComponents.h"
#include "fctdef.h"

#pragma comment(lib, "d2d1")


#define CM_UPDATEFRAME 0x407 // Custom message: Must update the frame !

typedef unsigned long long ComponentId;
typedef int ZIndex;

typedef std::pair<std::unique_ptr<Graphics::Component>, ComponentId> IndexedComponent;
typedef std::map<ZIndex, std::vector<IndexedComponent>> ComponentMap; // Manages, for each ZIndex, a vector on component assiociated with their ID.

class BaseWindow
{
private:
	HWND m_hwnd = NULL;
	LPCWSTR m_classname = L"DefaultClassName";

	ComponentMap m_components;
	ComponentId m_lastIdUsed = 0xF; // First 15 IDs reserved.
	Graphics::D2D1RenderTools m_renderTools;

	std::time_t m_timeBetweenFrames = 16i64; // 60 Frames per seconds
	bool m_isRunning = false;

protected:
	BaseWindow() = default;


public:
	/**
	 * @brief Constructor for BaseWindow.
	 *
	 * @param[in] hInstance		The application instance.
	 * @param[in] className		Unique class name for the application.
	 * @param[in] windowName	The window's display name.
	 */
	BaseWindow(_In_ const HINSTANCE hInstance, _In_ const LPCWSTR className, _In_ const LPCWSTR windowName);

	virtual ~BaseWindow();

	BaseWindow(BaseWindow&) = delete;
	BaseWindow& operator=(const BaseWindow&) = delete;

	/**
	 * @brief Show the window
	 * 
	 * @param[in] nCmdShow How to show the window. Refer to WinUser.h (393 - 407).
	 */
	void show(_In_ const int nCmdShow);

	/**
	 * @brief Add a component to this window.
	 * 
	 * @param[in] Component		The component to add.
	 * @param[in] zIndex		The verticality index of the component.
	 */
	ComponentId addComponent(_In_ std::unique_ptr<Graphics::Component>&& component, _In_opt_ int zIndex = 0);

	/**
	 * @brief Remove the component binded to the componentId.
	 * 
	 * @param[in] componentId The component's id to remove.
	 * 
	 * @retval std::unique_ptr<Component>& component
	 * @return The component unique_ptr that has been remove, or nullptr if the component was not in the window's component list.
	 */
	std::unique_ptr<Graphics::Component>&& removeComponent(_In_ ComponentId componentId) noexcept;

	/**
	 * @brief Modify the z-index of the component binded to componentId.
	 * 
	 * @param[in] componentId	The component's id.
	 * @param[in] zIndex		The new z-index for the component.
	 * 
	 * @retval bool
	 * @return True if the component is on the new z-index, false if it was not in the window's component list.
	 */
	bool setComponentZIndex(_In_ ComponentId componentId, _In_ ZIndex zIndex) noexcept;

	/**
	 * @brief Return the window handle.
	 *
	 * @retval HWND
	 * @return The window handle.
	 */
	const HWND getHwnd() { return m_hwnd; }

	/**
	 * @brief Get the component's pointer binded to the id.
	 * 
	 * @param[in] componentId The id of the component to get.
	 * 
	 * @retval Component*
	 * @return The component's pointer binded to the id, or nullptr if the component is not in the window's component list.
	*/
	Graphics::Component* getComponent(_In_ ComponentId componentId) const noexcept;

	/**
	 * @brief Return the list of all the component this window have.
	 * 
	 * @retval std::vector<Component>
	 * @return All the components this window have.
	 */
	const ComponentMap& getComponentList() { return m_components; }

	/**
	 * @brief Call the reconstruct method for all drawable components.
	 */
	inline void reconstructDrawableComponents() noexcept;

	/**
	 * @brief Return the time to wait between frames.
	 * 
	 * @retval std::time_t
	 * @return Time between frames.
	 */
	const inline std::time_t getTimeBetweenFrames() noexcept;

	/**
	 * @brief Set the time to wait between frames.
	 * 
	 * @param[in] time Value to set the time between fram to.
	 */
	inline void setTimeBetweenFrames(std::time_t time) noexcept;

	/**
	 * @brief Return the fps of the window.
	 *
	 * @retval unsigned int
	 * @return Fps of the window.
	 */
	const inline unsigned int getFps() noexcept;

	/**
	 * @brief Set the fps.
	 *
	 * @param[in] fps Must be non-zero.
	 *
	 * @throw std::invalid_argument When fps is zero.
	 */
	inline void setFps(_In_ unsigned int fps);

	/**
	 * @brief Returns the window's render tools.
	 * 
	 * @retval D2D1RenderTools
	 * @return Render tools of the window.
	 */
	inline Graphics::D2D1RenderTools& getRenderTools() noexcept { return m_renderTools; }

	/**
	 * @brief Loop until the window is destroy.
	 */
	void mainLoop();

	/**
	 * @brief Stop the main loop.
	 */
	inline void stop() noexcept;
};

#endif // WINDOWCLASS_H