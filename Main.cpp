#include <Windows.h>
#include <WinBase.h>
#include <WinUser.h>
#include <d2d1.h>

#include <iostream>
#include <string>

#include "WindowClass.h"
#include "EventHandler.h"
#include "GraphicComponents.h"
#include "fctdef.h"

#pragma comment(lib, "d2d1")



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance , _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
	BaseWindow baseWindow = BaseWindow(hInstance, L"heheheha", L"Saluté");
	baseWindow.show(nCmdShow);

	Graphics::Image im({ 100.0f, 100.0f }, L"Images\\gif.gif");
	baseWindow.addComponent(std::make_unique<Graphics::Image>(std::move(im)));

	baseWindow.mainLoop();
	
	return 0x0;
}


/*
LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const wchar_t* CLASS_NAME = L"Hello_world";

ID2D1Factory* pFactory;
ID2D1HwndRenderTarget* pRenderTarget;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
	returnOnFail(CoInitialize(NULL));

	WNDCLASS wc{};

	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = winProc;

	RegisterClass(&wc);

	HWND hwnd = CreateWindow(
		CLASS_NAME,
		L"Hey",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd) return 0x1;

	ShowWindow(hwnd, nCmdShow);
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return 0x0;
}

IWICFormatConverter* pConverter = nullptr;

LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		IWICImagingFactory* pImageFactory	= nullptr;
		IWICBitmapDecoder* pDecoder			= nullptr;
		IWICBitmapFrameDecode* pFrame		= nullptr;

		returnOnFail(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory));
		returnOnFail(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), reinterpret_cast<void**>(&pImageFactory)));

		returnOnFail(pImageFactory->CreateDecoderFromFilename(L"Images\\test.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder));
		returnOnFail(pDecoder->GetFrame(0, &pFrame));
		returnOnFail(pImageFactory->CreateFormatConverter(&pConverter));
		returnOnFail(pConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.0f,
			WICBitmapPaletteTypeMedianCut
		));
		break;
	}

	case WM_PAINT:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)), &pRenderTarget);
		D2D1_RECT_F rc = { rect.left, rect.top, rect.right, rect.bottom };

		ID2D1Bitmap* pBitmap = nullptr;
		returnOnFail(pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, &pBitmap));
		pRenderTarget->BeginDraw();
		pRenderTarget->DrawBitmap(pBitmap, rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		pRenderTarget->EndDraw();
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		DestroyWindow(hwnd);
		break;
	}
	
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	return S_OK;
}
*/
