#pragma once
#ifndef GRAPHIC_COMPONENTS_H
#define GRAPHIC_COMPONENTS_H

#include <memory>

#include <Windows.h>
#include <WinBase.h>
#include <WinUser.h>
#include <wincodec.h>
#include <minwindef.h> // BYTE
#include <vector>

#include <d2d1.h>

#include "fctdef.h"

#pragma comment (lib, "d2d1")

namespace Graphics
{
	/**
	 * @brief A B8G8R8A8 format pixel.
	 */
	struct Pixel
	{
		BYTE b = 0xFF;
		BYTE g = 0xFF;
		BYTE r = 0xFF;
		BYTE a = 0xFF;

		Pixel(BYTE red, BYTE green, BYTE blue, BYTE alpha)
		{
			r = red;
			g = green;
			b = blue;
			a = alpha;
		}
		Pixel(BYTE red, BYTE green, BYTE blue)
			:Pixel(red, green, blue, 0xFF)
		{}
		Pixel(BYTE color, BYTE alpha)
			:Pixel(color, color, color, alpha)
		{}
		Pixel(BYTE color)
			:Pixel(color, color, color, 0xFF)
		{}
	};


	/**
	 * @brief Structure for Direct2D render.
	 */
	struct D2D1RenderTools
	{
	public:
		ID2D1Factory* pFactory = nullptr;
		ID2D1HwndRenderTarget* pRenderTarget = nullptr;

		inline HRESULT CreateFactory()
		{
			D2D1_FACTORY_OPTIONS option(D2D1_DEBUG_LEVEL_WARNING); //TODO: remove
			return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &option, reinterpret_cast<void**>(&pFactory));
		}

		inline HRESULT CreateRenderTarget(_In_ HWND hwnd)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProperties = D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top));
			returnOnFail(pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), hwndProperties, &pRenderTarget));
			OutputDebugStringA("RT created !\n");
			return S_OK;
		}

		inline bool isFactoryValid() noexcept
		{
			return pFactory != nullptr;
		}

		inline bool isRenderTargetValid() noexcept
		{
			return pRenderTarget != nullptr;
		}

		inline void DestroyFactory()
		{
			if (isFactoryValid())
				pFactory->Release();
			pFactory = nullptr;
		}

		inline void DestroyRenderTarget()
		{
			if (isRenderTargetValid())
				pRenderTarget->Release();
			pRenderTarget = nullptr;
			OutputDebugStringA("RT destroyed !\n");
		}

		inline ~D2D1RenderTools()
		{
			DestroyRenderTarget();
			DestroyFactory();
		}

	};

	/**
	 * @brief Base class for window components.
	 */
	class Component
	{
	private:
		D2D1_POINT_2F m_position = { .0f, .0f };

	protected:
		void* m_window = nullptr;

		Component() = default;

	public:
		Component(_In_ D2D1_POINT_2F position) : m_position(position) {}

		/**
		 * @brief Initialize the component.
		 *
		 * @note Called when added to a window.
		 *
		 * @param[in] window The BaseWindow instance that called this method.
		 * @TODO: I don't like the void* : it is BaseWindow* !
		 *
		 * @retval bool
		 * @return True if the component should be added to the window, false otherwise.
		 */
		virtual bool initialize(void* window) noexcept { return m_window = window; }

		inline const D2D1_POINT_2F getPos() noexcept { return m_position; }
		inline void setPos(const D2D1_POINT_2F& pos) noexcept { m_position = pos; }
		inline void* getWindow() noexcept { return m_window; }
	};




	/**
	 * @brief Class for visual component.
	 */
	class DrawableComponent : public Component
	{
	protected:
		DrawableComponent() = default;

	public:
		/**
		 * @brief Method to draw the component.
		 * @note  renderTarget->BeginDraw() Must have been called.
		 *
		 * @param[in] hwnd		The window handle.
		 * @param[in] hdc		The device context handle.
		 *
		 * @retval LRESULT
		 * @return The draw result value.
		 */
		virtual void draw(_In_ HWND hwnd, _In_ ID2D1HwndRenderTarget* renderTarget) = 0;

		/**
		 * @brief Method called whenever the render target is destroyed.
		 * @note  Use to reset pixel dependent objects.
		*/
		virtual void reconstruct() noexcept = 0;
	};




	/**
	 * @brief Class for image component.
	 */
	class Image : public DrawableComponent
	{
	private:
		std::vector<Pixel> m_memory;
		ID2D1Bitmap* m_pBitmap = nullptr;

	public:
		/**
		 * @brief Constructor for an image.
		 *
		 * @param[in] pos			The image's position in client dependent pixel.
		 * @param[in] imageName		The image's path, relative or absolute.
		 */
		Image(_In_ const D2D1_POINT_2F& pos, _In_ const wchar_t* imageName);
		Image(_In_ const wchar_t* imageName);

		Image(Image&) noexcept;
		Image(Image&& other) noexcept;

		Image& operator=(Image&) noexcept;
		Image& operator=(Image&& other) noexcept;


		/**
		 * @brief Contructor of an image.
		 *
		 * @param[in] pConverter	Pointer to the image's converter.
		 * @param[in] nbFrame		The frame to load.
		 */
		Image(_In_ D2D1_POINT_2F pos, _In_ IWICFormatConverter* pConverter);

		void draw(_In_ HWND hwnd, _In_ ID2D1HwndRenderTarget* renderTarget) override;
		void reconstruct() noexcept override;
		inline D2D1_SIZE_F getSize();

		~Image();
	};




	/**
	 * @brief Class for animated image.
	 */
	class AnimatedImage : public DrawableComponent
	{
		/*
		private:
			std::vector<std::unique_ptr<Image>> m_images;
			unsigned int m_nbFrame = 0;
			unsigned int m_currentFrame = 0;

		public:
			AnimatedImage(_In_ const wchar_t* imageName);
			AnimatedImage(_In_ const D2D1_POINT_2F& pos, _In_ const wchar_t* imageName);

			AnimatedImage(AnimatedImage&& other) noexcept;
			AnimatedImage& operator=(AnimatedImage&& other) noexcept;

			AnimatedImage(AnimatedImage&) = delete;
			AnimatedImage& operator=(const AnimatedImage&) = delete;


			void draw(_In_ HWND hwnd, _In_ ID2D1HwndRenderTarget* renderTarget) override;
			void reconstruct() noexcept override;
			bool initialize(void* window) noexcept override;

			inline const unsigned int getNbFrame() const noexcept { return m_nbFrame; }
			inline const unsigned int getCurrentFrame() const noexcept { return m_currentFrame; }
			inline void setCurrentFrame(const unsigned int& frame) noexcept { m_currentFrame = frame; }
			~AnimatedImage()
			{}
		*/
	};

} // namespace Graphics

#endif // GRAPHIC_COMPONENTS_H