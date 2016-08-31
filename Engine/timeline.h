#pragma once		
static int* tpixels;
static HBITMAP thbmp;
static HDC thdcMem;
static HDC thdc;

static char* sceneNames[] =
{
	"Wind", // 0
	"Intro", // 1
	"Intro+Sax", // 2
	"Intro", // 3
	"Intro+Sax", // 4
	"Drums", // 5
	"Drums+Sax", // 6
	"Drums", // 7
	"Drums+Sax", // 8
	"Climax", // 9
	"Climax", // 10
	"Climax", // 11
	"Climax", // 12
	"Cool down", // 13
	"Cool down", // 14
	"Cool down", // 15
	"Cool down", // 16
	"Mute instr.", // 17
	"Mute all", // 18
};

static int sceneColors[] =
{
	0x007F0000, // 0
	0x007F0000, // 1
	0x007F0000, // 2
	0x007F0000, // 3
	0x007F0000, // 4
	0x007F7F00, // 5
	0x007F7F00, // 6
	0x007F7F00, // 7
	0x007F7F00, // 8
	0x00007F00, // 9
	0x00007F00, // 10
	0x00007F00, // 11
	0x00007F00, // 12
	0x00007F7F, // 13
	0x00007F7F, // 14
	0x00007F7F, // 15
	0x00007F7F, // 16
	0x0000007F, // 17
	0x0000007F, // 18
};

void InitTimeline(HWND hwnd)
{
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = 20 * 50;
	bmi.bmiHeader.biHeight = -128; // Order pixels from top to bottom
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbReserved = 0;

	thdc = ::GetDC(hwnd);
	thdcMem = CreateCompatibleDC(thdc);
	thbmp = CreateDIBSection(thdc, &bmi, DIB_RGB_COLORS, (void**)&tpixels, NULL, 0);
}

void updateTimeline(HWND hwnd,  bool playMusic, bool loopScene, float time)
{
	SelectObject(thdcMem, thbmp);

	memset(tpixels, 0, 20 * 50 * 128 * 4);

	int x = 0;
	RECT rect = { 0 };
	char titleStr[255];
	int seconds = (time * (MAX_SAMPLES / 38)) / SAMPLE_RATE;
	sprintf_s(titleStr, "[F5] toggle playback   [F6] toggle scene loop   [F7] Previous scene   [F8] Next scene   Current scene: %d   Time: %02d:%02d", (int)time, seconds / 60, seconds % 60);
	DrawText(thdcMem, titleStr, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
	for (int i = 0; i < 19; ++i)
	{
		x = 50 * i;
		for (int y = 64; y < 128; ++y)
		{
			for (int dx = x; dx < x + 50; ++dx)
				tpixels[20 * 50 * y + dx] = sceneColors[i] | 0xFF000000;
			tpixels[20 * 50 * y + x] = (2 * sceneColors[i]) | 0xFF000000;
		}
	}

	for (int x = 0; x < (_4klang_current_tick / (float)MAX_TICKS) * 50 * 20 - 25; ++x)
	{
		tpixels[20 * 50 * 23 + x] = 0xFFFFFFFF;
	}

	x = 50 * time;
	for (int y = 16; y < 128 && x < 20 * 50; ++y)
		tpixels[20 * 50 * y + x] = 0xFFFFFFFF;

	for (int i = 0; i < 19; ++i)
	{
		rect.left = 50 * i;
		rect.top = 64 + ((i + 3) % 4) * 16;
		DrawText(thdcMem, sceneNames[i], -1, &rect, DT_SINGLELINE | DT_NOCLIP);
	}

	seconds = 0;
	rect.top = 32;
	while (seconds < MAX_SAMPLES / SAMPLE_RATE)
	{
		x = seconds * SAMPLE_RATE / (MAX_SAMPLES / (38 * 50));
		if (x > 20 * 50)
			break;

		rect.left = x;
		for (int y = 24; y < ((seconds % 5) > 0 ? 28 : 32); ++y)
		{
			tpixels[20 * 50 * y + x] = 0xFFFFFFFF;
		}

		if (seconds % 10 == 0)
		{
			char timeStr[6];
			sprintf_s(timeStr, "%02d:%02d", seconds / 60, seconds % 60);
			DrawText(thdcMem, timeStr, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
		}

		++seconds;
	}

	BitBlt(thdc, 0, 0, 20 * 50, 128, thdcMem, 0, 0, SRCCOPY);
}

