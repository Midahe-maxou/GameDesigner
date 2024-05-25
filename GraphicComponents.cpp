#include "GraphicComponents.h"

#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include <combaseapi.h>
#include <wincodec.h>
#include <minwindef.h> // BYTE
#include <d2d1.h>

#include "fctdef.h"
#include "EventHandler.h"
#include "WindowClass.h"

#pragma comment(lib, "d2d1")


namespace Graphics
{


	/********************/
	/*		 Image		*/
	/********************/


	/**** Constructors ****/

	Image::Image(_In_ const D2D1_POINT_2F& pos, _In_ const wchar_t* imageName)
	{
		__super::setPos(pos);

		const unsigned int width = 100;
		const unsigned int height = 100;

		const int SIZE = width * height;
		m_memory.reserve(SIZE);

		for (int i = 0; i < SIZE; i++)
		{
			Pixel pixel(0x00, 0x12, 0xFF, 0x00);
			m_memory.push_back(pixel);
		}
	}


	Image::Image(_In_ const wchar_t* imageName)
		:Image({ 0.0f, 0.0f }, imageName)
	{}


	Image::Image(_In_ D2D1_POINT_2F pos, _In_ IWICFormatConverter* pConverter)
	{
		__super::setPos(pos);
	}

	Image::Image(Image& other) noexcept
	{
		this->setPos(other.getPos());
		m_memory = other.m_memory;
		m_pBitmap = other.m_pBitmap;
	}

	Image::Image(Image&& other) noexcept
	{
		this->setPos(other.getPos());
		m_memory = std::move(other.m_memory);
		m_pBitmap = other.m_pBitmap;
	}

	/**** Operators ****/

	Image& Image::operator=(Image& other) noexcept
	{
		this->setPos(other.getPos());
		m_memory = other.m_memory;
		m_pBitmap = other.m_pBitmap;
		return *this;
	}

	Image& Image::operator=(Image&& other) noexcept
	{
		if (this != &other)
		{
			this->setPos(other.getPos());
			m_memory = std::move(other.m_memory);
			m_pBitmap = other.m_pBitmap;
		}
		return *this;
	}

	/**** Methods ****/

	void Image::draw(_In_ HWND hwnd, _In_ ID2D1HwndRenderTarget* renderTarget)
	{
		if (m_pBitmap == nullptr)
		{
			D2D1_BITMAP_PROPERTIES properties = D2D1::BitmapProperties();
			properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
			properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
			throwOnFail(renderTarget->CreateBitmap({ 400, 400 }, reinterpret_cast<void*>(m_memory.data()), 10 * 4, properties, &m_pBitmap));
		}
		D2D1_SIZE_F size = getSize();
		D2D1_POINT_2F pos = getPos();

		D2D1_RECT_F rect({ pos.x, pos.y, pos.x + size.width, pos.y + size.height });

		renderTarget->DrawBitmap(m_pBitmap, rect);
	}

	void Image::reconstruct() noexcept
	{
		m_pBitmap = nullptr;
	}

	D2D1_SIZE_F Image::getSize()
	{
		if (m_pBitmap == nullptr) return { 0.0f, 0.0f };
		return m_pBitmap->GetSize();
	}

	Image::~Image()
	{
	}



	/****************************/
	/*		AnimatedImage		*/
	/****************************/


	/**** Constructors ****/

	/*
	AnimatedImage::AnimatedImage(_In_ const D2D1_POINT_2F& pos, _In_ const wchar_t* imageName)
	{
		__super::setPos(pos);
	}

	AnimatedImage::AnimatedImage(_In_ const wchar_t* imageName)
		:AnimatedImage({ 0.0f, 0.0f }, imageName)
	{
	}

	AnimatedImage::AnimatedImage(AnimatedImage&& other) noexcept
	{
		m_currentFrame = other.m_currentFrame;
		m_images = std::move(other.m_images);
		m_window = other.m_window;
		m_nbFrame = other.m_nbFrame;

		for (std::unique_ptr<Image>& im : other.m_images)
		{
			im = nullptr;
		}
	}
	*/

	/**** Operators ****/

	/*
	AnimatedImage& AnimatedImage::operator=(AnimatedImage&& other) noexcept
	{
		if (this != &other)
		{
			m_currentFrame = other.m_currentFrame;
			m_images = std::move(other.m_images);
			m_window = other.m_window;
			m_nbFrame = other.m_nbFrame;

			for (std::unique_ptr<Image>& im : other.m_images)
			{
				im = nullptr;
			}
		}
		return *this;
	}
	*/

	/**** Methods ****/

	/*
	void AnimatedImage::draw(_In_ HWND hwnd, _In_ ID2D1HwndRenderTarget* renderTarget)
	{
		std::unique_ptr<Image>& im = m_images[m_currentFrame];
		im->draw(hwnd, renderTarget);
		//m_currentFrame = (m_currentFrame + 1) % (m_nbFrame);
	}

	void AnimatedImage::reconstruct() noexcept
	{
		for (std::unique_ptr<Image>& im : m_images)
			im->reconstruct();
	}

	//TODO: remove
	LRESULT onKeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam, void* context)
	{
		AnimatedImage* image = reinterpret_cast<AnimatedImage*>(context);
		unsigned int frame = image->getCurrentFrame();
		unsigned int maxFrame = image->getNbFrame();
		image->setCurrentFrame((frame + 1) % maxFrame);
		return S_OK;
	}

	bool AnimatedImage::initialize(void* window) noexcept
	{
		__super::initialize(window);
		EventHandler::registerEvent(WM_KEYDOWN, onKeyDown, this);
		return true;
	}
	*/

} // namespace Graphics