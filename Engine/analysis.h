#pragma bss_seg(".analysisbss")
static ID3D11Texture2D* resolvedTexture;
static ID3D11Texture2D* stagingTexture;
static D3D11_MAPPED_SUBRESOURCE analysisResource;
static int* pixels;
static HBITMAP hbmp;
static HDC hdcMem;
static HDC hdc;
static D3D11_TEXTURE2D_DESC stagingDesc;
static unsigned int hR[256];
static unsigned int hG[256];
static unsigned int hB[256];
static unsigned int waveForm[screenWidth * 256];
static unsigned int paradeR[screenWidth * 256];
static unsigned int paradeG[screenWidth * 256];
static unsigned int paradeB[screenWidth * 256];
static unsigned int vectorScope[256 * 256];

#pragma data_seg(".analysisdata")
extern ID3D11Texture2D* backBufferPtr;
extern ID3D11Device* device;
extern ID3D11DeviceContext* deviceContext; 

float fast_log2(float val)
{
	int * const    exp_ptr = reinterpret_cast <int *> (&val);
	int            x = *exp_ptr;
	const int      log_2 = ((x >> 23) & 255) - 128;
	x &= ~(255 << 23);
	x += 127 << 23;
	*exp_ptr = x;

	return (val + log_2);
}

void InitAnalysis(HWND hwnd)
{
	backBufferPtr->GetDesc(&stagingDesc);
	stagingDesc.Usage = D3D11_USAGE_DEFAULT;
	stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	stagingDesc.BindFlags = 0;
	stagingDesc.SampleDesc.Count = 1;
	device->CreateTexture2D(&stagingDesc, NULL, &resolvedTexture);

	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	device->CreateTexture2D(&stagingDesc, NULL, &stagingTexture);

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = 1024;
	bmi.bmiHeader.biHeight = -512; // Order pixels from top to bottom
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

	hdc = ::GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdc);
	hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
}

void Analyze()
{
	SelectObject(hdcMem, hbmp);
	deviceContext->ResolveSubresource(resolvedTexture, 0, backBufferPtr, 0, stagingDesc.Format);
	deviceContext->CopyResource(stagingTexture, resolvedTexture);
	deviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &analysisResource);
	int* image = (int*)analysisResource.pData;
	memset(pixels, 0, 1024 * 512 * 4);

	memset(hR, 0, 256 * 4);
	memset(hG, 0, 256 * 4);
	memset(hB, 0, 256 * 4);
	memset(waveForm, 0, screenWidth * 256 * 4);
	memset(paradeR, 0, screenWidth * 256 * 4);
	memset(paradeG, 0, screenWidth * 256 * 4);
	memset(paradeB, 0, screenWidth * 256 * 4);
	memset(vectorScope, 0, 256 * 256 * 4);

	// Gather data
	for (int y = 0; y < screenHeight; ++y)
	{
		for (int x = 0; x < screenWidth; ++x)
		{
			unsigned char* colors = (unsigned char*)&image[y * (analysisResource.RowPitch / 4) + x]; // BGRA
			float luminance = 0.2126f * colors[2] + 0.7152f * colors[1] + 0.0722f * colors[0];

			float umax = 0.436f;
			float vmax = 0.615f;
			float ly = luminance / 255.0f;
			float fu = 0.492f * (colors[0] / 255.0f - ly);
			float fv = 0.877f * (colors[2] / 255.0f - ly);
			int u = min(255, max(0, 128 * (1 + fu / umax)));
			int v = min(255, max(0, 128 * (1 + fv / vmax)));

			int l = (int)(luminance);
			waveForm[l * screenWidth + x]++;
			vectorScope[v * 256 + u]++;
			paradeR[colors[2] * screenWidth + x]++;
			paradeG[colors[1] * screenWidth + x]++;
			paradeB[colors[0] * screenWidth + x]++;
			hR[colors[2]]++;
			hG[colors[1]]++;
			hB[colors[0]]++;
		}
	}

	deviceContext->Unmap(stagingTexture, 0);

	// Determine max values
	float maxHistogramValue = 0;
	float maxVectorscopeValue = 0;
	float maxWaveForm = 0;
	float maxParadeR = 0;
	float maxParadeG = 0;
	float maxParadeB = 0;
	for (int i = 0; i < 256; ++i)
	{
		if (hR[i] > maxHistogramValue)
			maxHistogramValue = hR[i];
		if (hG[i] > maxHistogramValue)
			maxHistogramValue = hG[i];
		if (hB[i] > maxHistogramValue)
			maxHistogramValue = hB[i];

		for (int j = 0; j < screenWidth; ++j)
		{
			if (waveForm[i * screenWidth + j] > maxWaveForm)
				maxWaveForm = waveForm[i * screenWidth + j];
			if (paradeR[i * screenWidth + j] > maxParadeR)
				maxParadeR = paradeR[i * screenWidth + j];
			if (paradeG[i * screenWidth + j] > maxParadeG)
				maxParadeG = paradeG[i * screenWidth + j];
			if (paradeB[i * screenWidth + j] > maxParadeB)
				maxParadeB = paradeB[i * screenWidth + j];
		}

		for (int j = 0; j < 256; ++j)
		{
			if (vectorScope[i * 256 + j] > maxVectorscopeValue)
				maxVectorscopeValue = vectorScope[i * 256 + j];
		}
	}

	maxHistogramValue = fast_log2(1 + maxHistogramValue);
	maxVectorscopeValue = fast_log2(1 + maxVectorscopeValue);
	maxWaveForm = fast_log2(1 + maxWaveForm);
	maxParadeR = fast_log2(1 + maxParadeR);
	maxParadeG = fast_log2(1 + maxParadeG);
	maxParadeB = fast_log2(1 + maxParadeB);
	
	// Draw histogram
	int oldYR = 255;
	int oldYG = 255;
	int oldYB = 255;
	for (int i = 0; i < 768; ++i)
	{
		int x = i * 256 / 768;
		int yR = 255 - min(255, max(0, (int)(fast_log2(1 + hR[x]) * 256.0f / maxHistogramValue)));
		int yG = 255 - min(255, max(0, (int)(fast_log2(1 + hG[x]) * 256.0f / maxHistogramValue)));
		int yB = 255 - min(255, max(0, (int)(fast_log2(1 + hB[x]) * 256.0f / maxHistogramValue)));
		if (oldYR < yR)
		{
			for (int y = oldYR; y <= yR; ++y)
				pixels[y * 1024 + i] |= 0x000000FF;
		}
		else
		{
			for (int y = yR; y <= oldYR; ++y)
				pixels[y * 1024 + i] |= 0x000000FF;
		}

		if (oldYG < yG)
		{
			for (int y = oldYG; y <= yG; ++y)
				pixels[y * 1024 + i] |= 0x0000FF00;
		}
		else
		{
			for (int y = yG; y <= oldYG; ++y)
				pixels[y * 1024 + i] |= 0x0000FF00;
		}

		if (oldYB < yB)
		{
			for (int y = oldYB; y <= yB; ++y)
				pixels[y * 1024 + i] |= 0x00FF0000;
		}
		else
		{
			for (int y = yB; y <= oldYB; ++y)
				pixels[y * 1024 + i] |= 0x00FF0000;
		}

		for (int y = yR; y <= 255; ++y)
			pixels[y * 1024 + i] |= 0x00000080;

		for (int y = yG; y <= 255; ++y)
			pixels[y * 1024 + i] |= 0x00008000;

		for (int y = yB; y <= 255; ++y)
			pixels[y * 1024 + i] |= 0x00800000;

		oldYR = yR;
		oldYG = yG;
		oldYB = yB;
	}

	for (int y = 0; y < 256; ++y)
	{
		// Draw Waveform and RGB parade
		for (int wx = 0; wx < screenWidth; ++wx)
		{
			int x = (int)(wx * 255.0f / screenWidth);

			int luminance = (int)(fast_log2(1 + waveForm[y * screenWidth + wx]) * 256.0f / maxWaveForm);
			int r = (int)(fast_log2(1 + paradeR[y * screenWidth + wx]) * 256.0f / maxParadeR);
			int g = (int)(fast_log2(1 + paradeG[y * screenWidth + wx]) * 256.0f / maxParadeG);
			int b = (int)(fast_log2(1 + paradeB[y * screenWidth + wx]) * 256.0f / maxParadeB);

			luminance	= max(luminance,	pixels[(511 - y) * 1024 + x] & 0x000000FF);
			r			= max(r,			pixels[(511 - y) * 1024 + x + 768]);
			g			= max(g,			pixels[(511 - y) * 1024 + x + 512] >> 8);
			b			= max(b,			pixels[(511 - y) * 1024 + x + 256] >> 16);

			int pixel = luminance | (luminance << 8) | (luminance << 16);
			pixels[(511 - y) * 1024 + x] = pixel;
			pixels[(511 - y) * 1024 + x + 768] = r;
			pixels[(511 - y) * 1024 + x + 512] = g << 8;
			pixels[(511 - y) * 1024 + x + 256] = b << 16;
		}

		// Draw Vectorscope
		for (int x = 0; x < 256; ++x)
		{
			float luminance = 0.1f + 0.9f * (1.0f - max(0, min(1, fast_log2(vectorScope[y * 256 + x] + 1) / maxVectorscopeValue)));
			float fx = (x - 128) / 128.0f;
			float fy = (y - 128) / 128.0f;
			if (fx*fx + fy*fy > 1)
				luminance = 0.0f;

			float umax = 0.436f;
			float vmax = 0.615f;
			float ly = 0.5;
			float u = ((x / 128.0f) - 1.0f) * umax;
			float v = ((y / 128.0f) - 1.0f) * vmax;
			int r = luminance * 255.0f * min(1, max(0, (ly + v / 0.877f)));
			int b = luminance * 255.0f * min(1, max(0, (ly + u / 0.492f)));
			int g = luminance * 255.0f * min(1, max(0, (ly - u * 0.395f - v * 0.581f)));

			int pixel = r | (g << 8) | (b << 16);
			pixels[(255 - y) * 1024 + x + 768] = pixel;
		}
	}

	RECT rect = { 0 };

	rect.left = 0;
	rect.top = 0;
	DrawText(hdcMem, "Histogram", -1, &rect, DT_SINGLELINE | DT_NOCLIP);

	rect.left = 768;
	rect.top = 0;
	DrawText(hdcMem, "Vectorscope", -1, &rect, DT_SINGLELINE | DT_NOCLIP);

	rect.left = 0;
	rect.top = 256;
	DrawText(hdcMem, "Waveform", -1, &rect, DT_SINGLELINE | DT_NOCLIP);

	rect.left = 256;
	rect.top = 256;
	DrawText(hdcMem, "RGB Parade", -1, &rect, DT_SINGLELINE | DT_NOCLIP);

	BitBlt(hdc, 0, 0, 1024, 512, hdcMem, 0, 0, SRCCOPY);
}