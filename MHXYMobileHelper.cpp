// MHXYMobileHelper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

bool use_mask;
cv::Mat img;
cv::Mat templ;
cv::Mat mask;
cv::Mat result;
int match_method;
int max_Trackbar = 5;

void CaptureScreen(HWND hWnd);
void MatchingMethod();
void Monitor();

int main()
{
	std::cout << "Hello World!\n";
	Monitor();
}

void CaptureScreen(HWND hWnd)
{
	HDC hdc = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	int32_t ScrWidth = 0, ScrHeight = 0;
	RECT rect = { 0 };
	if (hWnd == NULL)
	{
		ScrWidth = GetDeviceCaps(hdc, HORZRES) * 1.5;
		ScrHeight = GetDeviceCaps(hdc, VERTRES) * 1.5;
	}
	else
	{
		GetWindowRect(hWnd, &rect);
		ScrWidth = rect.right - rect.left;
		ScrHeight = rect.bottom - rect.top;
	}
	HDC hmdc = CreateCompatibleDC(hdc);

	HBITMAP hBmpScreen = CreateCompatibleBitmap(hdc, ScrWidth, ScrHeight);
	HBITMAP holdbmp = (HBITMAP)SelectObject(hmdc, hBmpScreen);

	BITMAP bm;
	GetObject(hBmpScreen, sizeof(bm), &bm);

	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = bm.bmPlanes;
	bi.biBitCount = bm.bmBitsPixel;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = bm.bmHeight * bm.bmWidthBytes;
	// 图片的像素数据
	char* buf = new char[bi.biSizeImage];
	BitBlt(hmdc, 0, 0, ScrWidth, ScrHeight, hdc, rect.left, rect.top, SRCCOPY);
	GetDIBits(hmdc, hBmpScreen, 0L, (DWORD)ScrHeight, buf, (LPBITMAPINFO)&bi, (DWORD)DIB_RGB_COLORS);

	BITMAPFILEHEADER bfh = { 0 };
	bfh.bfType = ((WORD)('M' << 8) | 'B');
	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	HANDLE hFile = CreateFile(L"ScreenShot.png", GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD dwWrite;
	WriteFile(hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
	WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	WriteFile(hFile, buf, bi.biSizeImage, &dwWrite, NULL);
	CloseHandle(hFile);
	hBmpScreen = (HBITMAP)SelectObject(hmdc, holdbmp);
}

void MatchingMethod()
{
	cv::Mat img_display;
	img.copyTo(img_display);
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	bool method_accepts_mask = (cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED);
	if (use_mask && method_accepts_mask)
	{
		std::cout << 11111 << std::endl;
		matchTemplate(img, templ, result, match_method, mask);
	}
	else
	{
		std::cout << 22222 << std::endl;
		matchTemplate(img, templ, result, match_method);
	}
	normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	cv::Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	std::cout << minVal << std::endl;
	std::cout << maxVal << std::endl;
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	std::cout << "move to x: " << matchLoc.x / 1.5 << " y: " << matchLoc.y / 1.5 << std::endl;
	SetCursorPos(matchLoc.x / 1.5, matchLoc.y / 1.5);
	return;
}

void Monitor()
{
	while (true) {
		std::cin >> match_method;
		std::cout << match_method << std::endl;
		CaptureScreen(NULL);
		img = cv::imread("ScreenShot.png", cv::IMREAD_COLOR);
		templ = cv::imread("集火.png", cv::IMREAD_COLOR);
		MatchingMethod();
		Sleep(500);
	}
}