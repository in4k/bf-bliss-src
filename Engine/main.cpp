#include "data.h"
#include "shaders.h"

#if _DEBUG
#include "profiler.h"
#include "analysis.h"
#include "timeline.h"
bool rendering = true;
bool doAnalysis = false;
bool wasPausePressed = false;
bool wasF10Pressed = false;
bool wasF9Pressed = false;
bool wasF8Pressed = false;
bool wasF7Pressed = false;
bool wasF6Pressed = false;
bool wasF5Pressed = false;
bool playMusic = true;
bool loopScene = false;
int currentScene = 0;
int sceneToLoop = 0;
int music_start_offset = 0;
#endif

#ifdef VIDEO
#include "video.h"
#endif

enum RenderMode
{
	Normal,
	Shadow,
	Water
};

#pragma code_seg("._ftol2_sse")
extern "C" __forceinline unsigned long _ftol2_sse(float Value)
{
	__asm fld Value
	__asm fistp dword ptr [eax]
}

#pragma code_seg(".InitD3D")
__forceinline void InitD3D()
{
	swapChainDesc.OutputWindow = hwnd;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D_DEVICE_FLAGS, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

	_asm
	{
		XOR			EDI, EDI
		MOV         ESI, dword ptr[device]
		MOV         EBX, [ESI]

		PUSH        offset swapChainDepthStencilBuffer
		PUSH        EDI
		PUSH        offset depthBufferDesc
		PUSH        ESI

		PUSH        offset shadowDepthStencilBuffer
		PUSH        EDI
		PUSH        offset shadowDepthBufferDesc
		PUSH        ESI

		PUSH        offset backBufferTexture
		PUSH        EDI
		PUSH        offset backBuffer1Desc
		PUSH        ESI

		PUSH        offset terrainDiffuseRenderTexture
		PUSH        EDI
		PUSH        offset terrainRenderTextureDesc
		PUSH        ESI

		PUSH        offset terrainRenderTexture
		PUSH        EDI
		PUSH        offset terrainRenderTextureDesc
		PUSH        ESI

		PUSH        offset floraRenderTexture
		PUSH        EDI
		PUSH        offset floraRenderTextureDesc
		PUSH        ESI

		PUSH        offset depthRenderTexture
		PUSH        EDI
		PUSH        offset depthRenderTextureDesc
		PUSH        ESI

		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]
		CALL		dword ptr[EBX + 0x14]

		PUSH        offset constantBuffers + 0
		PUSH        EDI
		PUSH        offset constantBuffer1Desc
		PUSH        ESI

		PUSH        offset constantBuffers + 4
		PUSH        EDI
		PUSH        offset constantBuffer2Desc
		PUSH        ESI

		PUSH        offset constantBufferTemp
		PUSH        EDI
		PUSH        offset constantBufferTempDesc
		PUSH        ESI

		PUSH        offset vertexBuffer
		PUSH        EDI
		PUSH        offset vertexBufferDesc
		PUSH        ESI

		PUSH        offset cellBuffer
		PUSH        EDI
		PUSH        offset cellBufferDesc
		PUSH        ESI

		PUSH        offset plantInstanceBuffer
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x0
		PUSH        ESI

		PUSH        offset soBuffers + 0
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x18
		PUSH        ESI

		PUSH        offset soBuffers + 4
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x30
		PUSH        ESI

		PUSH        offset multipliedPlantInstanceBuffer
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x48
		PUSH        ESI

		PUSH        offset multipliedFlowerInstanceBuffer
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x60
		PUSH        ESI

		PUSH        offset terrainBuffers + 0
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x78
		PUSH        ESI

		PUSH        offset terrainBuffers + 4
		PUSH        EDI
		PUSH        offset instanceBufferDescs + 0x78
		PUSH        ESI

		PUSH        offset solidRasterState
		PUSH        offset solidRasterDesc
		PUSH        ESI

		PUSH        offset solidFrontCullingRasterState
		PUSH        offset solidFrontCullingRasterDesc
		PUSH        ESI

		PUSH        offset solidNoCullingRasterState
		PUSH        offset solidNoCullingRasterDesc
		PUSH        ESI

		PUSH        offset swapChainDepthStencilView
		PUSH        offset depthStencilViewDesc
		PUSH        dword ptr[swapChainDepthStencilBuffer]
		PUSH        ESI

		PUSH        offset shadowDepthStencilView
		PUSH        offset depthStencilViewDesc
		PUSH        dword ptr[shadowDepthStencilBuffer]
		PUSH        ESI

		PUSH        offset depthStencilState
		PUSH        offset depthStencilDesc
		PUSH        ESI

		PUSH        offset noDepthStencilState
		PUSH        offset noDepthStencilDesc
		PUSH        ESI

		PUSH        offset alphaEnableBlendingState
		PUSH        offset blendStateDescription
		PUSH        ESI

		PUSH        offset samplerStates + 0
		PUSH        offset samplerDesc
		PUSH        ESI

		PUSH        offset samplerStates + 4
		PUSH        offset compSamplerDesc
		PUSH        ESI

		PUSH        offset samplerStates + 8
		PUSH        offset clampSamplerDesc
		PUSH        ESI

		PUSH        offset depthStencilShaderResourceView
		PUSH        offset depthStencilResourceViewDesc
		PUSH        dword ptr[swapChainDepthStencilBuffer]
		PUSH        ESI

		PUSH        offset terrainViews + 4
		PUSH        offset shaderResourceViewDesc
		PUSH        dword ptr[terrainDiffuseRenderTexture]
		PUSH        ESI

		PUSH        offset terrainViews + 8
		PUSH        offset depthResourceViewDesc
		PUSH        dword ptr[depthRenderTexture]
		PUSH        ESI

		PUSH        offset plantResources + 0
		PUSH        offset shaderResourceViewDesc
		PUSH        dword ptr[terrainRenderTexture]
		PUSH        ESI

		PUSH        offset plantResources + 4
		PUSH        offset shaderResourceViewDesc
		PUSH        dword ptr[floraRenderTexture]
		PUSH        ESI

		PUSH        offset postProcessResources + 12
		PUSH        offset backBufferShaderResourceViewDesc
		PUSH        dword ptr[backBufferTexture]
		PUSH        ESI

		PUSH        offset swapChainRenderTargetView
		PUSH        EDI
		PUSH        dword ptr[backBufferPtr]
		PUSH        ESI

		PUSH        offset terrainDiffuseRenderTargetView
		PUSH        offset renderTargetViewDesc
		PUSH        dword ptr[terrainDiffuseRenderTexture]
		PUSH        ESI

		PUSH        offset backBufferRenderTargetView
		PUSH        offset backBufferRenderTargetViewDesc
		PUSH        dword ptr[backBufferTexture]
		PUSH        ESI

		PUSH        offset heightMapRenderTargetView
		PUSH        offset renderTargetViewDesc
		PUSH        dword ptr[terrainRenderTexture]
		PUSH        ESI

		PUSH        offset floraTextureRenderTargetView
		PUSH        offset renderTargetViewDesc
		PUSH        dword ptr[floraRenderTexture]
		PUSH        ESI

		PUSH        offset depthTextureRenderTargetView
		PUSH        offset depthRenderTargetViewDesc
		PUSH        dword ptr[depthRenderTexture]
		PUSH        ESI

		CALL		dword ptr[EBX + 0x24]
		CALL		dword ptr[EBX + 0x24]
		CALL		dword ptr[EBX + 0x24]
		CALL		dword ptr[EBX + 0x24]
		CALL		dword ptr[EBX + 0x24]
		CALL		dword ptr[EBX + 0x24]

		CALL		dword ptr[EBX + 0x1C]
		CALL		dword ptr[EBX + 0x1C]
		CALL		dword ptr[EBX + 0x1C]
		CALL		dword ptr[EBX + 0x1C]
		CALL		dword ptr[EBX + 0x1C]
		CALL		dword ptr[EBX + 0x1C]

		CALL		dword ptr[EBX + 0x5C]
		CALL		dword ptr[EBX + 0x5C]
		CALL		dword ptr[EBX + 0x5C]

		CALL		dword ptr[EBX + 0x50]

		CALL		dword ptr[EBX + 0x54]
		CALL		dword ptr[EBX + 0x54]

		CALL		dword ptr[EBX + 0x28]
		CALL		dword ptr[EBX + 0x28]

		CALL		dword ptr[EBX + 0x58]
		CALL		dword ptr[EBX + 0x58]
		CALL		dword ptr[EBX + 0x58]

		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
		CALL		dword ptr[EBX + 0x0C]
	}

	/*device->CreateTexture2D(&depthBufferDesc, NULL, &swapChainDepthStencilBuffer);
	device->CreateTexture2D(&shadowDepthBufferDesc, NULL, &shadowDepthStencilBuffer);
	device->CreateTexture2D(&backBuffer1Desc, NULL, &backBufferTexture);
	device->CreateTexture2D(&terrainRenderTextureDesc, NULL, &terrainDiffuseRenderTexture);
	device->CreateTexture2D(&terrainRenderTextureDesc, NULL, &terrainRenderTexture);
	device->CreateTexture2D(&floraRenderTextureDesc, NULL, &floraRenderTexture);
	device->CreateTexture2D(&depthRenderTextureDesc, NULL, &depthRenderTexture);*/

	//device->CreateRenderTargetView(backBufferPtr, NULL, &swapChainRenderTargetView);
	//device->CreateRenderTargetView(terrainDiffuseRenderTexture, &renderTargetViewDesc, &terrainDiffuseRenderTargetView);
	//device->CreateRenderTargetView(backBufferTexture, &backBufferRenderTargetViewDesc, &backBufferRenderTargetView);
	//device->CreateRenderTargetView(terrainRenderTexture, &renderTargetViewDesc, &heightMapRenderTargetView);
	//device->CreateRenderTargetView(floraRenderTexture, &renderTargetViewDesc, &floraTextureRenderTargetView);
	//device->CreateRenderTargetView(depthRenderTexture, &depthRenderTargetViewDesc, &depthTextureRenderTargetView);

	//device->CreateShaderResourceView(terrainDiffuseRenderTexture, &shaderResourceViewDesc, &terrainViews[1]);
	//device->CreateShaderResourceView(backBufferTexture, &backBufferShaderResourceViewDesc, &postProcessResources[3]);
	//device->CreateShaderResourceView(terrainRenderTexture, &shaderResourceViewDesc, &plantResources[0]);
	//device->CreateShaderResourceView(floraRenderTexture, &shaderResourceViewDesc, &plantResources[1]);
	//device->CreateShaderResourceView(depthRenderTexture, &depthResourceViewDesc, &terrainViews[2]);
	//device->CreateShaderResourceView(swapChainDepthStencilBuffer, &depthStencilResourceViewDesc, &depthStencilShaderResourceView);

	//device->CreateSamplerState(&samplerDesc, &samplerStates[0]);
	//device->CreateSamplerState(&compSamplerDesc, &samplerStates[1]);
	//device->CreateSamplerState(&clampSamplerDesc, &samplerStates[2]);
	//device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState);

	//device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	//device->CreateDepthStencilState(&noDepthStencilDesc, &noDepthStencilState);

	//device->CreateDepthStencilView(swapChainDepthStencilBuffer, &depthStencilViewDesc, &swapChainDepthStencilView);
	//device->CreateDepthStencilView(shadowDepthStencilBuffer, &depthStencilViewDesc, &shadowDepthStencilView);

	//device->CreateRasterizerState(&solidRasterDesc, &solidRasterState);
	//device->CreateRasterizerState(&solidNoCullingRasterDesc, &solidNoCullingRasterState);
	//device->CreateRasterizerState(&solidFrontCullingRasterDesc, &solidFrontCullingRasterState);

	//device->CreateBuffer(&constantBuffer1Desc, NULL, &constantBuffers[0]);
	//device->CreateBuffer(&constantBuffer2Desc, NULL, &constantBuffers[1]);
	//device->CreateBuffer(&constantBufferTempDesc, NULL, &constantBufferTemp);
	//device->CreateBuffer(&vertexBufferDesc, NULL, &vertexBuffer);
	//device->CreateBuffer(&cellBufferDesc, NULL, &cellBuffer);
	//device->CreateBuffer(&instanceBufferDescs[0], NULL, &plantInstanceBuffer);
	//device->CreateBuffer(&instanceBufferDescs[1], NULL, &soBuffers[0]);
	//device->CreateBuffer(&instanceBufferDescs[2], NULL, &soBuffers[1]);
	//device->CreateBuffer(&instanceBufferDescs[3], NULL, &multipliedPlantInstanceBuffer);
	//device->CreateBuffer(&instanceBufferDescs[4], NULL, &multipliedFlowerInstanceBuffer);
	//device->CreateBuffer(&instanceBufferDescs[5], NULL, &terrainBuffers[0]);
	//device->CreateBuffer(&instanceBufferDescs[5], NULL, &terrainBuffers[1]);
#ifdef _DEBUG
	device->CreateRasterizerState(&wiredRasterDesc, &wiredRasterState);
#endif
	
	terrainViews[0] = plantResources[0];
}

#pragma code_seg(".UpdateFrameConstantBuffer")
void UpdateFrameConstantBuffer()
{
	deviceContext->VSSetSamplers(0, 3, samplerStates);
	deviceContext->HSSetSamplers(0, 3, samplerStates);
	deviceContext->GSSetSamplers(0, 3, samplerStates);
	deviceContext->DSSetSamplers(0, 3, samplerStates);
	deviceContext->PSSetSamplers(0, 3, samplerStates);

	deviceContext->VSSetConstantBuffers(0, 2, constantBuffers);
	deviceContext->GSSetConstantBuffers(0, 2, constantBuffers);
	deviceContext->HSSetConstantBuffers(0, 2, constantBuffers);
	deviceContext->DSSetConstantBuffers(0, 2, constantBuffers);
	deviceContext->PSSetConstantBuffers(0, 2, constantBuffers);

	deviceContext->VSSetShader(constantBufferVertexShader, NULL, 0);
	deviceContext->GSSetShader(constantBufferGeometryShader, NULL, 0);
	deviceContext->SOSetTargets(1, &constantBufferTemp, (UINT*)zero);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->Draw(1, 0);
	deviceContext->CopyResource(constantBuffers[1], constantBufferTemp);
}

#pragma code_seg(".BeginRender")
void BeginRender(RenderMode mode)
{
	deviceContext->Map(constantBuffers[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto constantBufferVertexData = (ConstantBuffer1Type*)mappedResource.pData;

	constantBufferVertexData->time = time;
	constantBufferVertexData->shader = mode;
	constantBufferVertexData->screenY = screenHeight;
	constantBufferVertexData->aspect = aspectRatio;

	deviceContext->Unmap(constantBuffers[0], 0);
}

#pragma code_seg(".InitBuffers")
__forceinline void InitBuffers()
{
#ifdef _DEBUG
	ProfileBlock profile("InitBuffers");
#endif
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	deviceContext->SOSetTargets(1, &cellBuffer, (UINT*)zero);
	deviceContext->VSSetShader(cellBufferVertexShader, NULL, 0);
	deviceContext->GSSetShader(cellBufferGeometryShader, NULL, 0);
	deviceContext->Draw(CELL_COUNT, 0);

	deviceContext->SOSetTargets(1, &vertexBuffer, (UINT*)zero);
	deviceContext->VSSetShader(vertexBufferVertexShader, NULL, 0);
	deviceContext->GSSetShader(vertexBufferGeometryShader, NULL, 0);
	deviceContext->Draw(VERTEX_COUNT, 0);
}

#pragma code_seg(".RenderPlantPositions")
__forceinline void RenderPlantPositions()
{
#ifdef _DEBUG
	ProfileBlock profile("RenderPlantPositions");
#endif
	deviceContext->SOSetTargets(2, soBuffers, (UINT*)zero);
	deviceContext->IASetInputLayout(instancedLayout);
	deviceContext->IASetVertexBuffers(0, 1, &plantInstanceBuffer, &strides[1], (UINT*)zero);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->VSSetShader(instanceVertexShader, NULL, 0);
	deviceContext->GSSetShaderResources(0, 1, &plantResources[0]);
	deviceContext->GSSetShader(rotationGeometryShader, NULL, 0);
	deviceContext->DrawAuto();

	deviceContext->GSSetShader(multiplyGeometryShader, NULL, 0);

	deviceContext->SOSetTargets(1, &multipliedPlantInstanceBuffer, (UINT*)zero);
	deviceContext->IASetVertexBuffers(0, 1, &soBuffers[0], &strides[1], (UINT*)zero);
	deviceContext->DrawAuto();

	deviceContext->SOSetTargets(1, &multipliedFlowerInstanceBuffer, (UINT*)zero);
	deviceContext->IASetVertexBuffers(0, 1, &soBuffers[1], &strides[1], (UINT*)zero);
	deviceContext->DrawAuto();

	deviceContext->SOSetTargets(0, (ID3D11Buffer* const*)zero, (UINT*)zero);
}

#pragma code_seg(".RenderPlants")
void RenderPlants(RenderMode mode)
{
#ifdef _DEBUG
	ProfileBlock profile("RenderPlants");
#endif

	deviceContext->IASetInputLayout(instancedLayout);
	deviceContext->IASetVertexBuffers(0, 1, &multipliedPlantInstanceBuffer, &strides[1], (UINT*)zero);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	deviceContext->VSSetShader(plantVertexShader, NULL, 0);
	deviceContext->HSSetShader(hullShaderPlant, NULL, 0);
	deviceContext->DSSetShader(mode == Normal ? plantDomainNormalShader : plantDomainShadowShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(mode == Normal ? plantPixelNormalShader : plantPixelShadowShader, NULL, 0);

	deviceContext->VSSetShaderResources(0, 2, plantResources);
	deviceContext->HSSetShaderResources(0, 2, plantResources);
	deviceContext->DSSetShaderResources(0, 2, plantResources);
	deviceContext->PSSetShaderResources(0, 2, plantResources);

	if (mode == Normal)
		deviceContext->OMSetBlendState(alphaEnableBlendingState, zero, 0xffffffff);

	deviceContext->DrawAuto();

	deviceContext->DSSetShader(mode == Normal ? flowerDomainNormalShader : flowerDomainShadowShader, NULL, 0);
	deviceContext->PSSetShader(mode == Normal ? flowerPixelNormalShader : plantPixelShadowShader, NULL, 0);
	deviceContext->IASetVertexBuffers(0, 1, &multipliedFlowerInstanceBuffer, &strides[1], (UINT*)zero);

	deviceContext->DrawAuto();
}

#pragma code_seg(".RenderTextures")
__forceinline void RenderTextures()
{
#ifdef _DEBUG
	ProfileBlock profile("RenderTextures");
#endif
	deviceContext->RSSetState(solidRasterState);

	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->VSSetShader(textureVertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->OMSetDepthStencilState(noDepthStencilState, 1);

	deviceContext->VSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);
	deviceContext->HSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);
	deviceContext->DSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);
	deviceContext->GSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);
	deviceContext->PSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);

	deviceContext->OMSetRenderTargets(1, &heightMapRenderTargetView, NULL);
	deviceContext->RSSetViewports(1, &terrainTextureViewport);
	deviceContext->PSSetShader(heightMapPixelShader, NULL, 0);
	deviceContext->SOSetTargets(0, (ID3D11Buffer* const*)zero, (UINT*)zero);
	deviceContext->Draw(4, 0);

	deviceContext->OMSetRenderTargets(1, &terrainDiffuseRenderTargetView, NULL);
	deviceContext->PSSetShaderResources(0, 1, &plantResources[0]);
	deviceContext->PSSetShader(terrainTexturePixelShader, NULL, 0);
	deviceContext->Draw(4, 0);

	deviceContext->OMSetRenderTargets(1, &floraTextureRenderTargetView, NULL);
	deviceContext->RSSetViewports(1, &floraTextureViewport);
	deviceContext->PSSetShader(plantTexturePixelShader, NULL, 0);
	deviceContext->Draw(4, 0);

	deviceContext->GenerateMips(terrainViews[1]);
	deviceContext->GenerateMips(plantResources[0]);
	deviceContext->GenerateMips(plantResources[1]);

	deviceContext->IASetInputLayout(layout);
	deviceContext->IASetVertexBuffers(0, 1, &cellBuffer, strides, (UINT*)zero);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->VSSetShader(terrainVertexShader, NULL, 0);
	deviceContext->GSSetShader(plantInstanceGeometryShader, NULL, 0);
	deviceContext->SOSetTargets(1, &plantInstanceBuffer, (UINT*)zero);
	deviceContext->VSSetShaderResources(0, 1, &plantResources[0]);
	deviceContext->GSSetShaderResources(0, 1, &plantResources[0]);
	deviceContext->Draw(CELL_COUNT, 0);
}

#pragma code_seg(".Postprocess")
__forceinline void Postprocess()
{
#ifdef _DEBUG
	ProfileBlock profile("Postprocess");
#endif
	BeginRender(Normal);
	deviceContext->RSSetState(solidRasterState);

	deviceContext->OMSetDepthStencilState(noDepthStencilState, 1);
	deviceContext->OMSetRenderTargets(1, &swapChainRenderTargetView, NULL);
	deviceContext->RSSetViewports(1, &swapChainViewport);

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->VSSetShader(textureVertexShader, NULL, 0);
	deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(postProcessPixelShader, NULL, 0);
	deviceContext->PSSetShaderResources(0, 4, postProcessResources);

	deviceContext->Draw(4, 0);

	deviceContext->PSSetShaderResources(0, 4, (ID3D11ShaderResourceView* const*)zero);
}

#pragma code_seg(".RenderCloud")
__forceinline void RenderCloud()
{
#ifdef _DEBUG
	ProfileBlock profile("RenderCloud");
#endif
	BeginRender(Normal);
	deviceContext->RSSetState(solidRasterState);

	deviceContext->OMSetDepthStencilState(noDepthStencilState, 1);

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->VSSetShader(textureVertexShader, NULL, 0);
	deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(cloudPixelShader, NULL, 0);
	deviceContext->Draw(4, 0);
}

#pragma code_seg(".RenderTerrain")
__forceinline void RenderTerrain()
{
	{
#ifdef _DEBUG
		ProfileBlock profile("RenderTerrain positions");
#endif
		deviceContext->IASetInputLayout(layout);
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, strides, (UINT*)zero);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		deviceContext->VSSetShader(terrainVertexShader, NULL, 0);
		deviceContext->HSSetShader(hullShader, NULL, 0);
		deviceContext->DSSetShader(terrainDomainShader, NULL, 0);
		deviceContext->GSSetShader(terrainGeometryShader, NULL, 0);
		deviceContext->PSSetShader(NULL, NULL, 0);

		deviceContext->SOSetTargets(2, terrainBuffers, (UINT*)zero);

		deviceContext->VSSetShaderResources(0, 3, terrainViews);
		deviceContext->HSSetShaderResources(0, 3, terrainViews);
		deviceContext->DSSetShaderResources(0, 3, terrainViews);
		deviceContext->GSSetShaderResources(0, 3, terrainViews);

		deviceContext->Draw(VERTEX_COUNT, 0);
	}

	deviceContext->SOSetTargets(0, (ID3D11Buffer* const*)zero, (UINT*)zero);
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	{
#ifdef _DEBUG
		ProfileBlock profile("RenderTerrain water");
#endif
		BeginRender(Water);

		deviceContext->VSSetShader(terrainProjectVertexShader, NULL, 0);
		deviceContext->HSSetShader(NULL, NULL, 0);
		deviceContext->DSSetShader(NULL, NULL, 0);
		deviceContext->GSSetShader(NULL, NULL, 0);
		deviceContext->PSSetShader(terrainPixelShader, NULL, 0);
		deviceContext->PSSetShaderResources(0, 3, terrainViews);
		deviceContext->RSSetState(solidFrontCullingRasterState);
		deviceContext->IASetInputLayout(layout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, &terrainBuffers[0], strides, (UINT*)zero);
		deviceContext->DrawAuto();
	}

	{
#ifdef _DEBUG
		ProfileBlock profile("RenderTerrain normal");
#endif
		BeginRender(Normal);

		deviceContext->OMSetBlendState(alphaEnableBlendingState, zero, 0xffffffff);
		deviceContext->RSSetState(solidRasterState);
		deviceContext->IASetVertexBuffers(0, 1, &terrainBuffers[1], strides, (UINT*)zero);
		deviceContext->DrawAuto();
		deviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);

		deviceContext->VSSetShaderResources(0, 3, (ID3D11ShaderResourceView* const *)zero);
		deviceContext->HSSetShaderResources(0, 3, (ID3D11ShaderResourceView* const *)zero);
		deviceContext->DSSetShaderResources(0, 3, (ID3D11ShaderResourceView* const *)zero);
		deviceContext->GSSetShaderResources(0, 3, (ID3D11ShaderResourceView* const *)zero);
		deviceContext->PSSetShaderResources(0, 3, (ID3D11ShaderResourceView* const *)zero);
	}
}

#pragma code_seg(".RenderShadow")
__forceinline void RenderShadow()
{
#ifdef _DEBUG
	ProfileBlock profile("RenderShadow");
#endif
	deviceContext->ClearDepthStencilView(shadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearRenderTargetView(depthTextureRenderTargetView, depthClearColor);
	deviceContext->OMSetRenderTargets(1, &depthTextureRenderTargetView, shadowDepthStencilView);
	deviceContext->RSSetViewports(1, &depthTextureViewport);

	BeginRender(Shadow);
	RenderPlantPositions();
	deviceContext->RSSetState(solidRasterState);
	deviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	RenderPlants(Shadow);
}

#if _DEBUG
void SeekToScene(int scene)
{
	if (scene < 0)
		scene = 0;
	
	music_start_offset = scene * (long)(MAX_SAMPLES / 38.0f);

	WaveHDR.lpData = (LPSTR)(float*)(soundBuffer + music_start_offset * 2);
	WaveHDR.dwBufferLength = (MAX_SAMPLES - (DWORD)music_start_offset) * 2;

	//send buffer to waveOut
	waveOutReset(hWaveOut);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));

	//put on pause
	if (!playMusic)
		waveOutPause(hWaveOut);
}
#endif

#pragma code_seg(".Frame")
__forceinline void Frame()
{
#if _DEBUG
	bool isPausePressed = GetAsyncKeyState(VK_PAUSE);
	bool isF10Pressed = GetAsyncKeyState(VK_F10);
	bool isF9Pressed = GetAsyncKeyState(VK_F9);
	bool isF8Pressed = GetAsyncKeyState(VK_F8);
	bool isF7Pressed = GetAsyncKeyState(VK_F7);
	bool isF6Pressed = GetAsyncKeyState(VK_F6);
	bool isF5Pressed = GetAsyncKeyState(VK_F5);
	if (isPausePressed && !wasPausePressed)
		rendering = !rendering;
	if (isF10Pressed && !wasF10Pressed)
		Profiler::GlobalProfiler.EnableProfiling = true;
	if (isF9Pressed && !wasF9Pressed)
		doAnalysis = !doAnalysis;
	if (isF5Pressed && !wasF5Pressed)
	{
		playMusic = !playMusic;
		if (playMusic)
			waveOutRestart(hWaveOut);
		else
			waveOutPause(hWaveOut);
	}
	if (isF6Pressed && !wasF6Pressed)
	{
		sceneToLoop = currentScene;
		loopScene = !loopScene;
	}
	if (isF7Pressed && !wasF7Pressed)
		SeekToScene(currentScene - 1);
	if (isF8Pressed && !wasF8Pressed)
		SeekToScene(currentScene + 1);

	wasPausePressed = isPausePressed;
	wasF10Pressed = isF10Pressed;
	wasF9Pressed = isF9Pressed;
	wasF8Pressed = isF8Pressed;
	wasF7Pressed = isF7Pressed;
	wasF6Pressed = isF6Pressed;
	wasF5Pressed = isF5Pressed;

	if (!rendering)
	{
		swapChain->Present(1, 0);
		return;
	}

	ProfileBlock profile("Frame");
#endif

#ifdef VIDEO
	time += 1.0f / 60 / (MAX_SAMPLES / 38.0f / SAMPLE_RATE);
#else
	waveOutGetPosition(hWaveOut, &mmtime, sizeof(MMTIME));
	time = mmtime.u.sample / (MAX_SAMPLES / 38.0f);
#ifdef _DEBUG
	time += music_start_offset / (MAX_SAMPLES / 38.0f);

	currentScene = (int)time;
	if (loopScene && currentScene != sceneToLoop)
	{
		SeekToScene(sceneToLoop);
		currentScene = sceneToLoop;
		time = music_start_offset / (MAX_SAMPLES / 38.0f);
	}
#endif
#endif

	UpdateFrameConstantBuffer();
	RenderShadow();

	deviceContext->ClearDepthStencilView(swapChainDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->OMSetRenderTargets(1, &backBufferRenderTargetView, swapChainDepthStencilView);
	deviceContext->RSSetViewports(1, &swapChainViewport);
	
	//DisplayTexture(floraShaderResourceView);
	RenderCloud();
	RenderTerrain();

	deviceContext->RSSetState(solidNoCullingRasterState);
	RenderPlants(Normal);

	Postprocess();
	swapChain->Present(1, 0);
}

#ifdef VIDEO
void WriteWav()
{
	fprintf(stdout, "calculating sound. please wait ...\n");

	// fill the sound buffer
	_4klang_render(soundBuffer);

	fprintf(stdout, "writing wav file ...\n");

	// init wave header
	char WaveHeader[44] =
	{
		'R', 'I', 'F', 'F',
		0, 0, 0, 0,				// filled below
		'W', 'A', 'V', 'E',
		'f', 'm', 't', ' ',
		16, 0, 0, 0,
		1, 0,
		2, 0,
		0x44, 0xac, 0, 0,
		0x10, 0xB1, 0x02, 0,
		4, 0,
		16, 0,
		'd', 'a', 't', 'a',
		0, 0, 0, 0				// filled below
	};
	*((DWORD*)(&WaveHeader[4])) = MAX_SAMPLES * 2 * 2 + 36;	// size of the rest of the file in bytes
	*((DWORD*)(&WaveHeader[40])) = MAX_SAMPLES * 2 * 2;		// size of raw sample data to come

	// write wave file
	FILE *file;
	fopen_s(&file, "out.wav", "wb");
	if (file)
	{
		fwrite(WaveHeader, 1, 44, file);
		for (int i = 0; i < MAX_SAMPLES * 2; i++)
		{
			// convert and clip each sample
#ifdef FLOAT_32BIT				
			int iin = (int)(soundBuffer[i] * 32767);
#else
			int iin = (int)(soundBuffer[i]);
#endif			
			if (iin > 32767) iin = 32767;
			if (iin < -32767) iin = -32767;
			short iout = iin;
			fwrite(&iout, 2, 1, file);
		}
		fclose(file);
	}
}
#endif

#pragma code_seg(".initsnd")
__forceinline void InitSound()
{
#ifndef VIDEO
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, soundBuffer, 0, 0);

	::Sleep(8192);// give the 4klang render call some time

	waveOutOpen(&hWaveOut, -1, &WaveFMT, 0, 0, 0);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, 0x20);
	waveOutWrite(hWaveOut, &WaveHDR, 0x20);
#endif
}

#pragma code_seg(".main")
void main()
{
#ifdef _DEBUG
	/*char exePath[4096];
	char* fileName;
	::GetModuleFileName(NULL, exePath, 4096);
	::GetFullPathName(exePath, 4096, shaderFile, &fileName);
	memcpy(fileName, "Shader.hlsl", 12);
	::GetFullPathName(exePath, 4096, shaderDir, &fileName);
	*fileName = 0;*/

#ifdef FULLSCREEN
	hwnd = CreateWindowExA(0, windowClass, 0, WS_VISIBLE, 0, 0, screenWidth, screenHeight, 0, 0, 0, 0);
	ShowCursor(false);
#else
	WNDCLASSEX wndclass;
	wndclass.cbSize        = sizeof (wndclass) ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = (WNDPROC)DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "BluFlame intro window class";
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowExA(0,
		wndclass.lpszClassName,
		"Intro",
		WS_EX_APPWINDOW,
		GetSystemMetrics(SM_CXSCREEN) - screenWidth - 16,
		0,
		screenWidth + 16,
		screenHeight + 40,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
		);
	ShowWindow(hwnd, SW_NORMAL);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	auto analysisHwnd = CreateWindowExA(0,
		wndclass.lpszClassName,
		"Image analysis",
		WS_EX_APPWINDOW,
		0,
		0,
		1024 + 16,
		512 + 40,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
		);
	SetWindowPos(analysisHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(analysisHwnd, SW_HIDE);

	auto timelineHwnd = CreateWindowExA(0,
		wndclass.lpszClassName,
		"Timeline",
		WS_EX_APPWINDOW,
		GetSystemMetrics(SM_CXSCREEN) - (20 * 50) - 16,
		GetSystemMetrics(SM_CYSCREEN) - 128 - 40 - 30,
		(20 * 50) + 16,
		128 + 40,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
		);

	ShowWindow(timelineHwnd, SW_NORMAL);
#endif
#else
	hwnd = CreateWindowExA(0, windowClass, 0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, screenWidth + 16, screenHeight + 40, 0, 0, 0, 0);
	ShowCursor(false);
#endif

#ifdef FULLSCREEN
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
#endif

	InitD3D();

#ifndef _DEBUG
	CreateShaders();
#else
#ifndef SHADERDEBUG
	InitShader();
	shaderCompileEvent = ::CreateEvent(NULL, FALSE, FALSE, TEXT("WriteEvent"));
	SetThreadPriority((HANDLE)CreateThread(0, 0, &filemon, 0, 0, 0), THREAD_PRIORITY_BELOW_NORMAL);
#endif
	CreateShaders();
#endif

	InitBuffers();
	BeginRender(Normal);
	UpdateFrameConstantBuffer();
	RenderTextures();

#ifdef _DEBUG
	Profiler::GlobalProfiler.Initialize(device, deviceContext);
#ifndef FULLSCREEN
	InitAnalysis(analysisHwnd);
	InitTimeline(timelineHwnd);
	ShowWindow(analysisHwnd, SW_NORMAL);
	ShowWindow(timelineHwnd, SW_NORMAL);
#endif
#endif

#ifdef VIDEO
	WriteWav();
	InitVideo();
#else
	InitSound();
#endif

#ifndef _DEBUG
	while (!GetAsyncKeyState(VK_ESCAPE) && time < 19)
#else
	while (!GetAsyncKeyState(VK_ESCAPE))
#endif
	{
#ifndef COMPRESS
		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#endif

#ifdef _DEBUG
		Profiler::GlobalProfiler.StartFrame();
#endif

		Frame();

#ifdef VIDEO
		VideoFrame();
#endif

#ifdef _DEBUG
		Profiler::GlobalProfiler.EndFrame();

#ifndef FULLSCREEN
		if (doAnalysis)
		{
			ShowWindow(analysisHwnd, SW_NORMAL);
			deviceContext->Flush();
			Analyze();
		}
		else
		{
			ShowWindow(analysisHwnd, SW_HIDE);
		}

		updateTimeline(timelineHwnd, playMusic, loopScene, time);
#endif

#ifndef SHADERDEBUG
		if (::WaitForSingleObject(shaderCompileEvent, 0) == WAIT_OBJECT_0)
		{
			::Sleep(50);
			CreateShaders();
			::Sleep(50);
			BeginRender(Normal);
			RenderTextures();
		}
#endif
#endif
	}

#ifdef VIDEO
	DoneVideo();
#else
	ExitProcess(0);
#endif
}
