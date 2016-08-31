#include <windows.h>
#include <vfw.h>

typedef struct
{
	PAVISTREAM         m_ps;
	PAVISTREAM	       m_psCompressed;
	AVISTREAMINFO      m_strhdr;
	AVICOMPRESSOPTIONS m_opts;
	PAVIFILE	       m_pfile;
	BITMAPINFOHEADER   m_alpbi;
} AVIWRITER;

static AVIWRITER avi;
static int frame;
static unsigned char* vbuffer;
static ID3D11Texture2D* vresolvedTexture;
static ID3D11Texture2D* vstagingTexture;
static D3D11_MAPPED_SUBRESOURCE vanalysisResource;
static D3D11_TEXTURE2D_DESC vstagingDesc;
extern ID3D11Texture2D* backBufferPtr;
extern ID3D11Device* device;
extern ID3D11DeviceContext* deviceContext;

void InitVideo()
{
	frame = 0;

	AVIFileInit();
	if (AVIFileOpen(&avi.m_pfile, "out.avi", OF_WRITE | OF_CREATE, NULL) != 0)
		return;

	memset(&avi.m_strhdr, 0, sizeof(avi.m_strhdr));
	avi.m_strhdr.fccType = streamtypeVIDEO;
	avi.m_strhdr.fccHandler = 0;
	avi.m_strhdr.dwScale = 1;
	avi.m_strhdr.dwRate = 60;
	avi.m_strhdr.dwSuggestedBufferSize = 3 * screenWidth * screenHeight;
	SetRect(&avi.m_strhdr.rcFrame, 0, 0, screenWidth, screenHeight);

	if (AVIFileCreateStream(avi.m_pfile, &avi.m_ps, &avi.m_strhdr) != 0)
		return;

	memset(&avi.m_opts, 0, sizeof(avi.m_opts));
	avi.m_opts.fccType = streamtypeVIDEO;
	//avi.m_opts.fccHandler = mmioFOURCC('d','i','v','x'); // divx
	avi.m_opts.fccHandler = mmioFOURCC('x','2','6','4'); // huffman uv
	//avi.m_opts.fccHandler = mmioFOURCC('D', 'I', 'B', ' '); // uncompressed frames
	avi.m_opts.dwKeyFrameEvery = 1;
	avi.m_opts.dwQuality = 100;
	avi.m_opts.dwBytesPerSecond = 0;
	avi.m_opts.dwFlags = AVICOMPRESSF_DATARATE | AVICOMPRESSF_KEYFRAMES;

	if (AVIMakeCompressedStream(&avi.m_psCompressed, avi.m_ps, &avi.m_opts, NULL) != AVIERR_OK)
		return;

	memset(&avi.m_alpbi, 0, sizeof(avi.m_alpbi));
	avi.m_alpbi.biSize = sizeof(avi.m_alpbi);
	avi.m_alpbi.biWidth = screenWidth;
	avi.m_alpbi.biHeight = screenHeight;
	avi.m_alpbi.biPlanes = 1;
	avi.m_alpbi.biBitCount = 24;
	avi.m_alpbi.biCompression = BI_RGB;
	if (AVIStreamSetFormat(avi.m_psCompressed, 0, &avi.m_alpbi, sizeof(avi.m_alpbi)) != 0)
		return;

	vbuffer = (unsigned char *)malloc(screenWidth*screenHeight * 3 + 1);
	if (!vbuffer)
		return;

	backBufferPtr->GetDesc(&vstagingDesc);
	vstagingDesc.Usage = D3D11_USAGE_DEFAULT;
	vstagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vstagingDesc.BindFlags = 0;
	vstagingDesc.SampleDesc.Count = 1;
	device->CreateTexture2D(&vstagingDesc, NULL, &vresolvedTexture);

	vstagingDesc.Usage = D3D11_USAGE_STAGING;
	vstagingDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	device->CreateTexture2D(&vstagingDesc, NULL, &vstagingTexture);
}

void VideoFrame()
{
	deviceContext->ResolveSubresource(vresolvedTexture, 0, backBufferPtr, 0, vstagingDesc.Format);
	deviceContext->CopyResource(vstagingTexture, vresolvedTexture);
	deviceContext->Map(vstagingTexture, 0, D3D11_MAP_READ, 0, &vanalysisResource);
	int* image = (int*)vanalysisResource.pData;
	for (int y = 0; y < screenHeight; ++y)
	for (int x = 0; x < screenWidth; ++x)
	{
		int pixel = image[screenWidth * y + x];
		int* bufPtr = (int*)&vbuffer[(screenWidth * (screenHeight - y - 1) + x) * 3];
		*bufPtr = ((pixel & 0x000000FF) << 16) | (pixel & 0x0000FF00) | ((pixel & 0x00FF0000) >> 16);
	}
	deviceContext->Unmap(stagingTexture, 0);
	if (AVIStreamWrite(avi.m_psCompressed, frame++, 1, vbuffer, 3 * screenWidth * screenHeight, AVIIF_KEYFRAME, NULL, NULL) != 0)
		throw;
}

void DoneVideo()
{
	free(vbuffer);

	AVIStreamClose(avi.m_ps);
	AVIStreamClose(avi.m_psCompressed);
	AVIFileClose(avi.m_pfile);
	AVIFileExit();
}

