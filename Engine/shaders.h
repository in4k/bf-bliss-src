#ifdef _DEBUG
#include <process.h>
#include <io.h>
#include <stdio.h>
HANDLE shaderCompileEvent;
static char** lastShader;
static __time64_t shaderChangedDate;
static char shaderDir[4096];
static char shaderFile[] = "E:\\dx11intro\\Engine\\Engine\\Shader.hlsl";

char* ReadFile(char* name, size_t* fileLen)
{
	FILE *file = 0;
	char *buffer = 0;

	//Open file
	fopen_s(&file, name, "rb");
	if (!file)
	{
		printf("Unable to open file %s\n", name);
		return 0;
	}

	//Get file length
	fseek(file, 0, SEEK_END);
	*fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer = (char *)malloc(*fileLen + 1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!\n");
		fclose(file);
		return 0;
	}

	//Read file contents into buffer
	fread(buffer, *fileLen, 1, file);
	fclose(file);

	return buffer;
}

void log(char* text)
{
	printf(text);
	OutputDebugStringA(text);
}

DWORD WINAPI filemon(void* args)
{
	while (true)
	{
		_finddata_t fdata;
		long hfile = _findfirst(shaderFile, &fdata);
		if (hfile != -1)
		{
			if (fdata.time_write != shaderChangedDate)
			{
				shaderChangedDate = fdata.time_write;
				::SetEvent(shaderCompileEvent);
			}
			_findclose(hfile);
		}
		::Sleep(100);
	}
	return 0;
}

void InitShader()
{
	_finddata_t fdata;
	long hfile = _findfirst(shaderFile, &fdata);
	if (hfile != -1)
	{
		shaderChangedDate = fdata.time_write;
		_findclose(hfile);
	}
}

#endif

#ifdef _DEBUG
#ifdef SHADERDEBUG
#define CompileShader(entryPoint, profile) CompileShaderD(shaderDir entryPoint ".cso", entryPoint, profile);
#else
#define CompileShader(entryPoint, profile) CompileShaderD(shaderFile, entryPoint, profile);
#endif

void CompileShaderD(char* fileName, char* entryPoint, char* profile)
{
#ifdef SHADERDEBUG
	shaderCode = ReadFile(fileName, &shaderSize);
#else
	size_t blobLength = 0;
	auto blob = ReadFile(fileName, &blobLength);
	d3dCompile(blob, blobLength, shaderFile, NULL, NULL, entryPoint, profile, 0, 0, &shaderBlob, &errorBlob);
	if (errorBlob != NULL)
	{
		auto error = errorBlob->GetBufferPointer();
		OutputDebugStringA((char*)error);
		if (shaderBlob == 0)
		{
			printf("Error compiling shader: ");
			printf((char*)error);
			printf("\n");
			ExitProcess(-1);
		}
	}
	if (shaderBlob != 0)
	{
		shaderSize = shaderBlob->GetBufferSize();
		shaderCode = shaderBlob->GetBufferPointer();
	}
#endif
}

#else
#define CompileShader(entryPoint, profile) CompileShaderR(entryPoint, profile);
#endif

#pragma code_seg(".CompileShader")
void CompileShaderR(char* entryPoint, char* profile)
{
#ifndef COMPRESS
	d3dCompile(Shader_hlsl, sizeof(Shader_hlsl), NULL, NULL, NULL, entryPoint, profile, 0, 0, &shaderBlob, &errorBlob);
	if (errorBlob != NULL)
	{
		auto error = errorBlob->GetBufferPointer();
		OutputDebugStringA((char*)error);
	}
#else
	d3dCompile(Shader_hlsl, sizeof(Shader_hlsl), NULL, NULL, NULL, entryPoint, profile, 0, 0, &shaderBlob, NULL);
#endif
	//if (shaderBlob != 0)
	{
		shaderSize = shaderBlob->GetBufferSize();
		shaderCode = shaderBlob->GetBufferPointer();
	}
};

#pragma data_seg(".vs_5_0")
static char vs_5_0[] = "vs_5_0";

#pragma data_seg(".gs_5_0")
static char gs_5_0[] = "gs_5_0";

#pragma data_seg(".ds_5_0")
static char ds_5_0[] = "ds_5_0";

#pragma data_seg(".hs_5_0")
static char hs_5_0[] = "hs_5_0";

#pragma data_seg(".ps_5_0")
static char ps_5_0[] = "ps_5_0";

#pragma code_seg(".CreateShaders")
void CreateShaders()
{
	HINSTANCE hl = LoadLibrary(d3dcompiler);
	if (!hl)
	{
		d3dcompiler[13] = '3';
		hl = LoadLibrary(d3dcompiler);
	}
	d3dCompile = (pD3DCompile)GetProcAddress(hl, "D3DCompile");

	CompileShader("GSQ", gs_5_0);
	device->CreateGeometryShader(shaderCode, shaderSize, NULL, &quadGeometryShader);

	CompileShader("HS", hs_5_0);
	device->CreateHullShader(shaderCode, shaderSize, NULL, &hullShader);

	CompileShader("HSP", hs_5_0);
	device->CreateHullShader(shaderCode, shaderSize, NULL, &hullShaderPlant);

	CompileShader("VSC", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &cellBufferVertexShader);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, cellBufferSODeclarations, 2, strides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &cellBufferGeometryShader);

	CompileShader("VSV", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &vertexBufferVertexShader);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, cellBufferSODeclarations, 2, strides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &vertexBufferGeometryShader);

	CompileShader("VSCB", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &constantBufferVertexShader);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, constantBufferSODeclarations, 19, &strides[3], 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &constantBufferGeometryShader);

	CompileShader("GSP", gs_5_0);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, plantSODeclarations, 4, plantStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &plantInstanceGeometryShader);

	CompileShader("GSM", gs_5_0);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, plantSODeclarations, 4, plantStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &multiplyGeometryShader);

	CompileShader("GSR", gs_5_0);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, plantSODeclarations, 8, plantStrides, 2, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &rotationGeometryShader);

	CompileShader("GST", gs_5_0);
	device->CreateGeometryShaderWithStreamOutput(shaderCode, shaderSize, terrainSODeclarations, 4, vertexStrides, 2, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &terrainGeometryShader);

	CompileShader("VS", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &terrainVertexShader);
	device->CreateInputLayout(polygonLayout, 2, shaderCode, shaderSize, &layout);

	CompileShader("VSP", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &plantVertexShader);
	device->CreateInputLayout(instancedLayoutDesc, 4, shaderCode, shaderSize, &instancedLayout);

	CompileShader("VSM", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &instanceVertexShader);

	CompileShader("VSTP", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &terrainProjectVertexShader);

	CompileShader("VST", vs_5_0);
	device->CreateVertexShader(shaderCode, shaderSize, NULL, &textureVertexShader);

	CompileShader("DS", ds_5_0);
	device->CreateDomainShader(shaderCode, shaderSize, NULL, &terrainDomainShader);

	CompileShader("DSPN", ds_5_0);
	device->CreateDomainShader(shaderCode, shaderSize, NULL, &plantDomainNormalShader);

	CompileShader("DSPS", ds_5_0);
	device->CreateDomainShader(shaderCode, shaderSize, NULL, &plantDomainShadowShader);

	CompileShader("DSFN", ds_5_0);
	device->CreateDomainShader(shaderCode, shaderSize, NULL, &flowerDomainNormalShader);

	CompileShader("DSFS", ds_5_0);
	device->CreateDomainShader(shaderCode, shaderSize, NULL, &flowerDomainShadowShader);

	CompileShader("PS", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &terrainPixelShader);

	CompileShader("PSH", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &heightMapPixelShader);

	CompileShader("PSTN", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &plantPixelNormalShader);

	CompileShader("FSTN", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &flowerPixelNormalShader);

	CompileShader("PSTS", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &plantPixelShadowShader);

	CompileShader("PSDD", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &postProcessPixelShader);

	CompileShader("PSP", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &plantTexturePixelShader);

	CompileShader("PSC", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &cloudPixelShader);

	CompileShader("PSTD", ps_5_0);
	device->CreatePixelShader(shaderCode, shaderSize, NULL, &terrainTexturePixelShader);
}
