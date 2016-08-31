#include <Windows.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "4klang.h"
#include "Shader.h"

#define FULLSCREEN
#define MSAA

#ifdef MSAA
#define MSAA_SAMPLECOUNT 4
#define MSAA_DSV D3D11_DSV_DIMENSION_TEXTURE2DMS
#define MSAA_SRV D3D11_SRV_DIMENSION_TEXTURE2DMS
#define MSAA_RTV D3D11_RTV_DIMENSION_TEXTURE2DMS
#else
#define MSAA_SAMPLECOUNT 1
#define MSAA_DSV D3D11_DSV_DIMENSION_TEXTURE2D
#define MSAA_SRV D3D11_SRV_DIMENSION_TEXTURE2D
#define MSAA_RTV D3D11_RTV_DIMENSION_TEXTURE2D
#endif

#ifdef FULLSCREEN
#define WINDOWED FALSE
#define screenWidth 1024
#define screenHeight 768
#else
#define WINDOWED TRUE
#define screenWidth 1280
#define screenHeight 720
#endif

#define terrainTextureSize 4096
#define shadowMapSize 4096
#define floraTextureSize 512

#define PATCH_COUNT 512
#define CELL_COUNT (PATCH_COUNT * PATCH_COUNT)
#define TERRAIN_TRIANGLE_COUNT (CELL_COUNT * 64)
#define PLANT_COUNT 3000
#define LEAF_COUNT (PLANT_COUNT * 14)
#define FLOWER_COUNT (PLANT_COUNT * 8 * 4)
#define PLANT_QUAD_COUNT (4 * LEAF_COUNT)
#define FLOWER_QUAD_COUNT (4 * FLOWER_COUNT)
#define VERTEX_COUNT (4 * CELL_COUNT)

struct VertexType
{
	float px, py, pz, u, v;
};

struct PlantInstanceType
{
	float tx, ty, tz, tw, rx, ry, rz, rw, sx, sy, sz, sw, s, t, u, v;
};

struct ConstantBuffer1Type
{
	float time;
	int shader;
	int screenY;
	float aspect;
};

struct ConstantBuffer2Type
{
	float directionX;
	float directionY;
	float directionZ;
	float directionW;
	float cameraX;
	float cameraY;
	float cameraZ;
	float cameraW;
	float view11;
	float view12;
	float view13;
	float view14;
	float view21;
	float view22;
	float view23;
	float view24;
	float view31;
	float view32;
	float view33;
	float view34;
	float view41;
	float view42;
	float view43;
	float view44;
	float projection11;
	float projection12;
	float projection13;
	float projection14;
	float projection21;
	float projection22;
	float projection23;
	float projection24;
	float projection31;
	float projection32;
	float projection33;
	float projection34;
	float projection41;
	float projection42;
	float projection43;
	float projection44;
	float lightvp11;
	float lightvp12;
	float lightvp13;
	float lightvp14;
	float lightvp21;
	float lightvp22;
	float lightvp23;
	float lightvp24;
	float lightvp31;
	float lightvp32;
	float lightvp33;
	float lightvp34;
	float lightvp41;
	float lightvp42;
	float lightvp43;
	float lightvp44;
	float fog1;
	float fog2;
	float fog3;
	float fog4;
	float lift1;
	float lift2;
	float lift3;
	float lift4;
	float gamma1;
	float gamma2;
	float gamma3;
	float gamma4;
	float gain1;
	float gain2;
	float gain3;
	float gain4;
	float lightDir1;
	float lightDir2;
	float lightDir3;
	float lightDir4;
};

#pragma data_seg(".strides")
static UINT strides[] =
{
	sizeof(VertexType),
	sizeof(PlantInstanceType),
	sizeof(ConstantBuffer1Type),
	sizeof(ConstantBuffer2Type)
};

#pragma data_seg(".plantStrides")
static UINT plantStrides[] =
{
	sizeof(PlantInstanceType), sizeof(PlantInstanceType)
};

#pragma data_seg(".vertexStrides")
static UINT vertexStrides[] =
{
	sizeof(VertexType), sizeof(VertexType)
};

#pragma data_seg("d3dcompiler")
static char d3dcompiler[] = "d3dcompiler_47.dll";

#pragma bss_seg(".bss")
static pD3DCompile d3dCompile;
static float time;
static HWND hwnd;
static IDXGISwapChain* swapChain;
static ID3D11Device* device;
static ID3D11DeviceContext* deviceContext;
static ID3D11Texture2D* swapChainDepthStencilBuffer;
static ID3D11Texture2D* shadowDepthStencilBuffer;
static ID3D11Texture2D* backBufferPtr;
static ID3D11Texture2D* terrainRenderTexture;
static ID3D11Texture2D* terrainDiffuseRenderTexture;
static ID3D11Texture2D* backBufferTexture;
static ID3D11Texture2D* floraRenderTexture;
static ID3D11Texture2D* depthRenderTexture;
static ID3D11RenderTargetView* swapChainRenderTargetView;
static ID3D11RenderTargetView* terrainDiffuseRenderTargetView;
static ID3D11RenderTargetView* backBufferRenderTargetView;
static ID3D11RenderTargetView* heightMapRenderTargetView;
static ID3D11RenderTargetView* terrainWaterRenderTargetView;
static ID3D11RenderTargetView* floraTextureRenderTargetView;
static ID3D11RenderTargetView* depthTextureRenderTargetView;
static ID3D11DepthStencilState* depthStencilState;
static ID3D11DepthStencilState* noDepthStencilState;
static ID3D11DepthStencilView* swapChainDepthStencilView;
static ID3D11DepthStencilView* shadowDepthStencilView;
static ID3D11RasterizerState* solidRasterState;
static ID3D11RasterizerState* solidNoCullingRasterState;
static ID3D11RasterizerState* solidFrontCullingRasterState;
static ID3D11RasterizerState* wiredRasterState;
static ID3DBlob* shaderBlob;
static ID3DBlob* errorBlob;
static ID3D11VertexShader* instancedVertexShader;
static ID3D11VertexShader* terrainVertexShader;
static ID3D11VertexShader* constantBufferVertexShader;
static ID3D11VertexShader* cellBufferVertexShader;
static ID3D11VertexShader* vertexBufferVertexShader;
static ID3D11HullShader* hullShader;
static ID3D11HullShader* hullShaderPlant;
static ID3D11DomainShader* terrainDomainShader;
static ID3D11PixelShader* terrainPixelShader;
static ID3D11VertexShader* textureVertexShader;
static ID3D11VertexShader* plantVertexShader;
static ID3D11VertexShader* instanceVertexShader;
static ID3D11VertexShader* terrainProjectVertexShader;
static ID3D11DomainShader* plantDomainNormalShader;
static ID3D11DomainShader* plantDomainShadowShader;
static ID3D11DomainShader* flowerDomainNormalShader;
static ID3D11DomainShader* flowerDomainShadowShader;
static ID3D11PixelShader* heightMapPixelShader;
static ID3D11PixelShader* plantTexturePixelShader;
static ID3D11PixelShader* plantPixelNormalShader;
static ID3D11PixelShader* flowerPixelNormalShader;
static ID3D11PixelShader* plantPixelShadowShader;
static ID3D11PixelShader* postProcessPixelShader;
static ID3D11PixelShader* terrainTexturePixelShader;
static ID3D11GeometryShader* constantBufferGeometryShader;
static ID3D11GeometryShader* plantInstanceGeometryShader;
static ID3D11GeometryShader* multiplyGeometryShader;
static ID3D11GeometryShader* rotationGeometryShader;
static ID3D11GeometryShader* quadGeometryShader;
static ID3D11GeometryShader* terrainGeometryShader;
static ID3D11GeometryShader* cellBufferGeometryShader;
static ID3D11GeometryShader* vertexBufferGeometryShader;
static ID3D11PixelShader* cloudPixelShader;
static ID3D11Buffer* constantBufferTemp;
static ID3D11Buffer* cellBuffer;
static ID3D11Buffer* vertexBuffer;
static ID3D11Buffer* plantInstanceBuffer;
static ID3D11Buffer* multipliedPlantInstanceBuffer;
static ID3D11Buffer* multipliedFlowerInstanceBuffer;
static ID3D11InputLayout* layout;
static ID3D11InputLayout* instancedLayout;
static D3D11_MAPPED_SUBRESOURCE mappedResource;
static ID3D11ShaderResourceView* terrainWaterShaderResourceView;
static ID3D11ShaderResourceView* depthStencilShaderResourceView;
static ID3D11BlendState* alphaEnableBlendingState;
static void* shaderCode;
static size_t shaderSize;
static ID3D11SamplerState* samplerStates[3];// = { samplerState, compSamplerState, clampSamplerState };
static ID3D11Buffer* constantBuffers[2];// = { constantBuffer1, constantBuffer2, tweakBuffer };
static ID3D11Buffer* soBuffers[2];// = { rotatedPlantInstanceBuffer, flowerInstanceBuffer };
static ID3D11ShaderResourceView* plantResources[2];// = { terrainShaderResourceView, floraShaderResourceView };
static ID3D11ShaderResourceView* postProcessResources[4];// = { 0, 0, 0, backBufferShaderResourceView1 };
static ID3D11Buffer* terrainBuffers[2];// = { underwaterVertexBuffer, landVertexBuffer };
static ID3D11ShaderResourceView* terrainViews[3];// = { plantResources[0], terrainDiffuseShaderResourceView, depthShaderResourceView }; // !!!

#pragma data_seg(".mmtime")
static MMTIME mmtime =
{
	TIME_SAMPLES
};

#pragma data_seg(".aspectRatio")
static float aspectRatio = float(screenWidth) / screenHeight;

#pragma bss_seg(".audio_bss")
static SAMPLE_TYPE soundBuffer[MAX_SAMPLES * 2];
static HWAVEOUT hWaveOut;

#pragma data_seg(".wavehdr")
WAVEHDR WaveHDR =
{
	(LPSTR)soundBuffer,
	MAX_SAMPLES*sizeof(SAMPLE_TYPE)* 2,
	0, 0, WHDR_PREPARED, 0, 0, 0
};

#pragma data_seg(".wavefmt")
WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
	2, // channels
	SAMPLE_RATE, // samples per sec
	SAMPLE_RATE*sizeof(SAMPLE_TYPE)* 2, // bytes per sec
	sizeof(SAMPLE_TYPE)* 2, // block alignment;
	sizeof(SAMPLE_TYPE)* 8, // bits per sample
	0 // extension not needed
};

static D3D11_BLEND_DESC blendStateDescription =
{
	0,
	FALSE,
	{
		{
			TRUE,
			D3D11_BLEND_SRC_ALPHA,//D3D11_BLEND_ONE,
			D3D11_BLEND_INV_SRC_ALPHA,
			D3D11_BLEND_OP_ADD,
			D3D11_BLEND_ONE,
			D3D11_BLEND_ZERO,
			D3D11_BLEND_OP_ADD,
			0x0F
		}
	}
};

#pragma data_seg(".polygonLayout")
static D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
{
	{
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	}
};

#pragma data_seg(".instancedLayoutDesc")
static D3D11_INPUT_ELEMENT_DESC instancedLayoutDesc[] =
{
	{
		"TRANSLATION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"ROTATION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"SCALE",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	},
	{
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	}
};

#pragma data_seg(".constantBuffer1Desc")
static D3D11_BUFFER_DESC constantBuffer1Desc =
{
	sizeof(ConstantBuffer1Type),
	D3D11_USAGE_DYNAMIC,
	D3D11_BIND_CONSTANT_BUFFER,
	D3D11_CPU_ACCESS_WRITE,
	0,
	0
};

#pragma data_seg(".constantBuffer2Desc")
static D3D11_BUFFER_DESC constantBuffer2Desc =
{
	sizeof(ConstantBuffer2Type),
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_CONSTANT_BUFFER,
	0,
	0,
	0
};

#pragma data_seg(".constantBufferTempDesc")
static D3D11_BUFFER_DESC constantBufferTempDesc =
{
	sizeof(ConstantBuffer2Type),
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_STREAM_OUTPUT,
	0,
	0,
	0
};

#pragma data_seg(".vertexBufferDesc")
static D3D11_BUFFER_DESC vertexBufferDesc =
{
	sizeof(VertexType) * VERTEX_COUNT,
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
	0,
	0,
	0
};

#pragma data_seg(".cellBufferDesc")
static D3D11_BUFFER_DESC cellBufferDesc =
{
	sizeof(VertexType)* CELL_COUNT,
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
	0,
	0,
	0
};

#pragma data_seg(".instanceBufferDescs")
static D3D11_BUFFER_DESC instanceBufferDescs[] =
{
	{
		sizeof(PlantInstanceType)* CELL_COUNT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	},
	{
		sizeof(PlantInstanceType)* LEAF_COUNT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	},
	{
		sizeof(PlantInstanceType)* FLOWER_COUNT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	},
	{
		sizeof(PlantInstanceType)* PLANT_QUAD_COUNT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	},
	{
		sizeof(PlantInstanceType)* FLOWER_QUAD_COUNT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	},
	{
		sizeof(VertexType)* 1500000,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT,
		0,
		0,
		0
	}
};

#pragma data_seg(".cellBufferSODeclarations")
static D3D11_SO_DECLARATION_ENTRY cellBufferSODeclarations[] =
{
	{ 0, "POSITION", 0, 0, 3, 0 },
	{ 0, "TEXCOORD", 0, 0, 2, 0 },
};

#pragma data_seg(".constantBufferSODeclarations")
static D3D11_SO_DECLARATION_ENTRY constantBufferSODeclarations[] =
{
	{ 0, "_dS", 0, 0, 4, 0 },
	{ 0, "_cS", 0, 0, 4, 0 },
	{ 0, "_vS", 0, 0, 4, 0 },
	{ 0, "_vS", 1, 0, 4, 0 },
	{ 0, "_vS", 2, 0, 4, 0 },
	{ 0, "_vS", 3, 0, 4, 0 },
	{ 0, "_pS", 0, 0, 4, 0 },
	{ 0, "_pS", 1, 0, 4, 0 },
	{ 0, "_pS", 2, 0, 4, 0 },
	{ 0, "_pS", 3, 0, 4, 0 },
	{ 0, "_lvpS", 0, 0, 4, 0 },
	{ 0, "_lvpS", 1, 0, 4, 0 },
	{ 0, "_lvpS", 2, 0, 4, 0 },
	{ 0, "_lvpS", 3, 0, 4, 0 },
	{ 0, "_fS", 0, 0, 4, 0 },
	{ 0, "_lS", 0, 0, 4, 0 },
	{ 0, "_gS", 0, 0, 4, 0 },
	{ 0, "_nS", 0, 0, 4, 0 },
	{ 0, "lS", 0, 0, 4, 0 },
};

#pragma data_seg(".plantSODeclarations")
static D3D11_SO_DECLARATION_ENTRY plantSODeclarations[] =
{
	{
		0,
		"TRANSLATION",
		0,
		0,
		4,
		0
	},
	{
		0,
		"ROTATION",
		0,
		0,
		4,
		0
	},
	{
		0,
		"SCALE",
		0,
		0,
		4,
		0
	},
	{
		0,
		"TEXCOORD",
		0,
		0,
		4,
		0
	},
	{
		1,
		"TRANSLATION",
		0,
		0,
		4,
		1
	},
	{
		1,
		"ROTATION",
		0,
		0,
		4,
		1
	},
	{
		1,
		"SCALE",
		0,
		0,
		4,
		1
	},
	{
		1,
		"TEXCOORD",
		0,
		0,
		4,
		1
	}
};

#pragma data_seg(".terrainSODeclarations")
static D3D11_SO_DECLARATION_ENTRY terrainSODeclarations[] =
{
	{
		0,
		"POSITION",
		0,
		0,
		3,
		0
	},
	{
		0,
		"TEXCOORD",
		0,
		0,
		2,
		0
	},
	{
		1,
		"POSITION",
		0,
		0,
		3,
		1
	},
	{
		1,
		"TEXCOORD",
		0,
		0,
		2,
		1
	}
};

#pragma data_seg(".swapChainDesc")
static DXGI_SWAP_CHAIN_DESC swapChainDesc =
{
	{
		screenWidth,
		screenHeight,
		{
			0, 0
		},
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		DXGI_MODE_SCALING_UNSPECIFIED
	},
	{
		MSAA_SAMPLECOUNT, 0
	},
	DXGI_USAGE_RENDER_TARGET_OUTPUT,
	1,
	0,
	WINDOWED,
	DXGI_SWAP_EFFECT_DISCARD,
	0
};

#pragma data_seg(".samplerDesc")
static D3D11_SAMPLER_DESC samplerDesc =
{
	D3D11_FILTER_ANISOTROPIC,
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_WRAP,
	0.0f,
	1,
	D3D11_COMPARISON_ALWAYS,
	{
		0, 0, 0, 0
	},
	0,
	D3D11_FLOAT32_MAX
};

#pragma data_seg(".clampSamplerDesc")
static D3D11_SAMPLER_DESC clampSamplerDesc =
{
	D3D11_FILTER_ANISOTROPIC,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	0.0f,
	1,
	D3D11_COMPARISON_ALWAYS,
	{
		0, 0, 0, 0
	},
	0,
	D3D11_FLOAT32_MAX
};

#pragma bss_seg(".zero")
static float zero[4] = { 0, 0, 0, 0 };

#pragma data_seg(".compSamplerDesc")
static D3D11_SAMPLER_DESC compSamplerDesc =
{
	D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	0.0f,
	1,
	D3D11_COMPARISON_LESS_EQUAL,
	{
		0, 0, 0, 0
	},
	0,
	D3D11_FLOAT32_MAX
};

#pragma data_seg(".featureLevel")
static D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };

#pragma data_seg(".depthBufferDesc")
static D3D11_TEXTURE2D_DESC depthBufferDesc =
{
	screenWidth,
	screenHeight,
	1,
	1,
	DXGI_FORMAT_R24G8_TYPELESS,
	{
		MSAA_SAMPLECOUNT, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
	0,
	0
};

#pragma data_seg(".shadowDepthBufferDesc")
static D3D11_TEXTURE2D_DESC shadowDepthBufferDesc =
{
	shadowMapSize,
	shadowMapSize,
	1,
	1,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	{
		1, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_DEPTH_STENCIL,
	0,
	0
};

#pragma data_seg(".terrainRenderTextureDesc")
static D3D11_TEXTURE2D_DESC terrainRenderTextureDesc =
{
	terrainTextureSize,
	terrainTextureSize,
	1,
	1,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	{
		1, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	0,
	D3D11_RESOURCE_MISC_GENERATE_MIPS
};

#pragma data_seg(".backBuffer1Desc")
static D3D11_TEXTURE2D_DESC backBuffer1Desc =
{
	screenWidth,
	screenHeight,
	1,
	1,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	{
		MSAA_SAMPLECOUNT, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	0,
	0
};

#pragma data_seg(".backBuffer2Desc")
static D3D11_TEXTURE2D_DESC backBuffer2Desc =
{
	screenWidth,
	screenHeight,
	1,
	1,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	{
		MSAA_SAMPLECOUNT, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	0,
	0
};

#pragma data_seg(".floraRenderTextureDesc")
static D3D11_TEXTURE2D_DESC floraRenderTextureDesc =
{
	floraTextureSize,
	floraTextureSize,
	1,
	1,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	{
		1, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	0,
	D3D11_RESOURCE_MISC_GENERATE_MIPS
};

#pragma data_seg(".depthRenderTextureDesc")
static D3D11_TEXTURE2D_DESC depthRenderTextureDesc =
{
	shadowMapSize,
	shadowMapSize,
	1,
	1,
	DXGI_FORMAT_R32_FLOAT,
	{
		1, 0
	},
	D3D11_USAGE_DEFAULT,
	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	0,
	0
};

#pragma data_seg(".backBufferRenderTargetViewDesc")
static D3D11_RENDER_TARGET_VIEW_DESC backBufferRenderTargetViewDesc =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	MSAA_RTV,
	{
		0
	}
};

#pragma data_seg(".renderTargetViewDesc")
static D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	D3D11_RTV_DIMENSION_TEXTURE2D,
	{
		0
	}
};

#pragma data_seg(".depthRenderTargetViewDesc")
static D3D11_RENDER_TARGET_VIEW_DESC depthRenderTargetViewDesc =
{
	DXGI_FORMAT_R32_FLOAT,
	D3D11_RTV_DIMENSION_TEXTURE2D,
	{
		0
	}
};

#pragma data_seg(".backBufferShaderResourceViewDesc")
static D3D11_SHADER_RESOURCE_VIEW_DESC backBufferShaderResourceViewDesc =
{
	DXGI_FORMAT_R8G8B8A8_UNORM,
	MSAA_SRV,
	{
		0, 1
	}
};

#pragma data_seg(".shaderResourceViewDesc")
static D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	{
		0, 1
	}
};

#pragma data_seg(".depthResourceViewDesc")
static D3D11_SHADER_RESOURCE_VIEW_DESC depthResourceViewDesc =
{
	DXGI_FORMAT_R32_FLOAT,
	D3D11_SRV_DIMENSION_TEXTURE2D,
	{
		0, 1
	}
};

#pragma data_seg(".depthStencilResourceViewDesc")
static D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilResourceViewDesc =
{
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
	MSAA_SRV,
	{
		0, 1
	}
};

#pragma data_seg(".depthStencilDesc")
static D3D11_DEPTH_STENCIL_DESC depthStencilDesc =
{
	true,
	D3D11_DEPTH_WRITE_MASK_ALL,
	D3D11_COMPARISON_LESS,
	false,
	0,
	0
};

#pragma data_seg(".noDepthStencilDesc")
static D3D11_DEPTH_STENCIL_DESC noDepthStencilDesc =
{
	false,
	D3D11_DEPTH_WRITE_MASK_ZERO,
	D3D11_COMPARISON_ALWAYS,
	false,
	0,
	0
};

#pragma data_seg(".depthStencilViewDesc")
static D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc =
{
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	MSAA_DSV,
	0,
	{
		0
	}
};

#pragma data_seg(".rasterDesc")
static D3D11_RASTERIZER_DESC solidRasterDesc =
{
	D3D11_FILL_SOLID,
	D3D11_CULL_BACK,
	FALSE,
	0,
	0.0f,
	0.0f,
	TRUE,
	FALSE,
	TRUE,
	TRUE
};

#pragma data_seg(".solidNoCullingRasterDesc")
static D3D11_RASTERIZER_DESC solidNoCullingRasterDesc =
{
	D3D11_FILL_SOLID,
	D3D11_CULL_NONE,
	FALSE,
	0,
	0.0f,
	0.0f,
	TRUE,
	FALSE,
	TRUE,
	TRUE
};

#pragma data_seg(".solidFrontCullingRasterDesc")
static D3D11_RASTERIZER_DESC solidFrontCullingRasterDesc =
{
	D3D11_FILL_SOLID,
	D3D11_CULL_FRONT,
	FALSE,
	0,
	0.0f,
	0.0f,
	TRUE,
	FALSE,
	TRUE,
	TRUE
};

#ifdef _DEBUG
#pragma data_seg(".rasterDesc")
static D3D11_RASTERIZER_DESC wiredRasterDesc =
{
	D3D11_FILL_WIREFRAME,
	D3D11_CULL_NONE,
	FALSE,
	0,
	0.0f,
	0.0f,
	TRUE,
	FALSE,
	TRUE,
	TRUE
};
#endif

#pragma data_seg(".swapChainViewport")
static D3D11_VIEWPORT swapChainViewport =
{
	0.0f,
	0.0f,
	screenWidth,
	screenHeight,
	0.0f,
	1.0f,
};

#pragma data_seg(".terrainTextureViewport")
static D3D11_VIEWPORT terrainTextureViewport =
{
	0.0f,
	0.0f,
	terrainTextureSize,
	terrainTextureSize,
	0.0f,
	1.0f,
};

#pragma data_seg(".floraTextureViewport")
static D3D11_VIEWPORT floraTextureViewport =
{
	0.0f,
	0.0f,
	floraTextureSize,
	floraTextureSize,
	0.0f,
	1.0f,
};

#pragma data_seg(".depthTextureViewport")
static D3D11_VIEWPORT depthTextureViewport =
{
	0.0f,
	0.0f,
	shadowMapSize,
	shadowMapSize,
	0.0f,
	1.0f,
};

#pragma data_seg(".depthClearColor")
static float depthClearColor[] = { 1.0f, 0.0f, 0.0f, 0.0f };

#pragma data_seg(".windowClass")
static char windowClass[] = "static";

#pragma data_seg(".dmScreenSettings")
static DEVMODE dmScreenSettings = {
	"", 0, 0, sizeof(dmScreenSettings), 0, DM_PELSWIDTH | DM_PELSHEIGHT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0, screenWidth, screenHeight
};

#ifndef COMPRESS
#define D3D_DEVICE_FLAGS D3D11_CREATE_DEVICE_DEBUG 
#else
#define D3D_DEVICE_FLAGS 0
#endif