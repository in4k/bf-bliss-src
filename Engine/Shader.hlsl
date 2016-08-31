cbuffer _0 : register(b0)
{
	float _t : TIME;
	uint _sh : SHADER;
	uint _sy : HEIGHT;
	float _a : ASPET;
};

struct _2
{
	float3 o : POSITION;
	float2 tc : TEXCOORD0;
};

struct _3
{
	float4 o : SV_POSITION;
	float4 c : COLOR;
	sample float2 tc : TEXCOORD0;
	float3 n : TEXCOORD1;
};

struct _4
{
	float e[4] : SV_TessFactor;
	float i[2] : SV_InsideTessFactor;
};

struct _5
{
	float3 o : POSITION;
	float2 tc : TEXCOORD0;
	float4 c : COLOR;
	float4 _t : TRANSLATION;
	float4 _r : ROTATION;
	float4 _s : SCALE;
};

struct _6
{
	float4 _t : TRANSLATION;
	float4 _r : ROTATION;
	float4 _s : SCALE;
	float4 tc : TEXCOORD0;
};

struct __1
{
	float4 _d : _dS;
	float4 _c : _cS;
	float4x4 _v : _vS;
	float4x4 _p : _pS;
	float4x4 _lvp : _lvpS;
	float4 _f : _fS;
	float4 _r1 : _lS;
	float4 _r2 : _gS;
	float4 _r3 : _nS;
	float4 _l : lS;
};

cbuffer _1 : register(b1)
{
	__1 _c;
};

Texture2D shaderTexture: register(t0);
Texture2D terrainDiffuseTexture: register(t1);
Texture2D shadowTexture: register(t2);
Texture2DMS<float4, 4> backBuffer: register(t3);
SamplerState sampleType : register(s0);
SamplerState clampSampleType : register(s2);
SamplerComparisonState cmpSampler : register(s1);

static const float PI = 3.141592;

float4 qnorm(float4 q)
{
	return q / sqrt(dot(q, q));
}

float4 qmul(float4 a, float4 b)
{
	return float4(cross(a.xyz, b.xyz) + a.w*b.xyz + b.w*a.xyz, a.w*b.w - dot(a.xyz, b.xyz));
}

float4 qconj(float4 q)
{
	return float4(-q.xyz, q.w);
}

float4 quaternion(float3 v, float a)
{
	return float4(v * sin(a / 2), cos(a / 2));
}

float random(float2 a)
{
	return frac(sin(dot(a, float2(12.9898, 78.233))) * 43758.5453);
}

float3 dnoise2f(float2 p)
{
	float2 ij = floor(p);
		float2 uv = p - ij;

		float2 duv = 30 * uv *uv * (uv * (uv - 2) + 1);

		uv = uv*uv*uv*(uv*(uv * 6 - 15) + 10);

	float a = random(ij + float2(0, 0));
	float b = random(ij + float2(1, 0));
	float c = random(ij + float2(0, 1));
	float d = random(ij + float2(1, 1));

	float k1 = b - a;
	float k2 = c - a;
	float k3 = a - b - c + d;

	return float3(a + k1*uv.x + k2*uv.y + k3*uv.x*uv.y, duv * float2(k1 + k3*uv.y, k2 + k3*uv.x));
}

float3 fbm(float2 p, int octaves, float persistence, float gain)
{
	float f = 0.0;
	float w = 0.5;
	float2 d = float2(0, 0);
	for (int i = 0; i < octaves; i++)
	{
		float3 n = dnoise2f(p);
			d += n.yz;
		f += w * n.x / (1.0 + dot(d, d));
		w *= gain;
		p *= persistence;
	}
	return float3(f, d);
}

float4x4 lookTo(float3 Eye, float3 zaxis, float3 Up)
{
	float3 xaxis = normalize(cross(Up, zaxis));
		float3 yaxis = cross(zaxis, xaxis);

		float4x4 result = {
		xaxis, -dot(xaxis, Eye),
		yaxis, -dot(yaxis, Eye),
		zaxis, -dot(zaxis, Eye),
		0, 0, 0, 1
	};

	return transpose(result);
}

float island(float2 uvWorld)
{
	return 20.0 * max(0.0, 1.5 - 8.0 * pow(length(uvWorld - 0.5), 1.0));
}

float4 terrainHeight(float2 uvWorld)
{
	return shaderTexture.SampleLevel(sampleType, uvWorld, 0) * float4(island(uvWorld), 1.0, 1.0, 1.0);
}

void cameraPosition(float t, float seed, out float3 position)
{
	position = (float3(random(float2(t, seed++)), random(float2(t, seed++)), random(float2(t, seed++))) * 2 - 1) * float3(64, 3, 64);
	position.y += 4 + island(position.xz / 512.0 + 0.5);
	position.y = smoothstep(3.0, 13.0, position.y) * 10.0 + 3.0;
}

void getCamera(int seed, out float3 position, out float3 direction, out float3 up)
{
	float a = seed;
	float3 v1, v2, v3, v4, right;
	cameraPosition(a, seed++, v1);
	cameraPosition(a, seed++, v2);
	cameraPosition(a, seed++, v3);
	cameraPosition(a, seed++, v4);
	position = lerp(v1, v2, frac(_t));
	direction = normalize(lerp(v3, v4, frac(_t)) - position);
	right = cross(float3(0, 1, 0), direction);
	up = cross(direction, right);
}

// Constant buffer vertex shader
__1 VSCB()
{
	__1 o;

	float dayTime = -cos(PI * 2 * _t / 19)*.5 + .5;

	o._l = float4(lerp(float3(-1.0, 0.2, 0.0), float3(0.1, -0.7, 0.0), dayTime), 0.0);
	o._f = float4(lerp(float3(0.3, 0.2, 0.0), float3(0.1, 0.5, 0.7), dayTime), dayTime);
	o._r1 = float4(lerp(float3(0.0, 0.0, 0.1), float3(0., 0.1, 0.1), dayTime), 1.0);
	o._r2 = float4(lerp(float3(0.2, 0.0, 0.3), float3(0.6, 0.5, 0.6), dayTime), 1.0);
	o._r3 = float4(lerp(float3(0.8, 0.6, 0.0), float3(0.7, 0.6, 0.5), dayTime), 1.0);

	int seeds[19] = { // good ones: 90, 91, 96
		55, 58, 2, 7, 111, // intro
		11, 90, 0, 17, // drums
		39, 83, 29, 91, // climax
		69, 55, 59, 51, // cooldown
		61, 55 
	};
	float3 position, direction, up;
	getCamera(seeds[floor(_t)], position, direction, up);
	o._v = lookTo(position, direction, up);

	float3 lposition = position + direction * 25;
		lposition.y = position.y;
	float3 ldir = normalize(o._l);
		lposition -= ldir * 25;

	o._d = float4(direction, 0);
	o._c = float4(position, 0);
	o._p = float4x4( // ortho matrix
		0.04, 0, 0, 0,
		0, 0.04, 0, 0,
		0, 0, 0.0005, 0,
		0, 0, -0.0005, 1
		);
	o._lvp = mul(lookTo(lposition, ldir, direction), o._p);
	o._p = float4x4( // perspective matrix
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 1,
		0, 0, -1, 0
		);

	return o;
}

// Cell vertex shader
void VSC(uint id : SV_VertexID, out _2 output)
{
	float x = id % 512;
	float y = id / 512;

	output.o = float3(-256 + x, 0, -256 + y);
	output.tc = (float2(x, y) + 0.5) / 512;
}

// Terrain vertex shader
void VSV(uint id : SV_VertexID, out _2 output)
{
	uint row = id % 2048;
	float y = id / 2048;
	uint vid = row % 4;
	float x = row / 4;

	output.o = float3(-256 + x, 0, -256 + y);

	if (vid == 0)
	{
		output.o += float3(-0.5f, 0, -0.5f);
		output.tc = float2(x, y) / 512;
	}
	else if (vid == 1)
	{
		output.o += float3(-0.5f, 0, 0.5f);
		output.tc = float2(x, y+1) / 512;
	}
	else if (vid == 2)
	{
		output.o += float3(0.5f, 0, 0.5f);
		output.tc = float2(x+1, y+1) / 512;
	}
	else // if (vid == 3
	{
		output.o += float3(0.5f, 0, -0.5f);
		output.tc = float2(x+1, y) / 512;
	}
}

// Terrain Vertex shader
void VS(_2 input, out _5 output)
{
	output.o = input.o;
	output.tc = input.tc;
	output.c =
		output._r =
		output._s = 0;
	output._t = float4(0, terrainHeight(input.tc).r, 0, 0);
}

float SphereToScreenSpaceTessellation(float3 p0, float3 p1)
{
	float4 view0 = mul(float4((p0 + p1) / 2, 1), _c._v);
	float4 view1 = view0;
	view1.x++;
	float4 clip0 = mul(view0, _c._p);
	float4 clip1 = mul(view1, _c._p);

	float t = distance(clip0 / clip0.w, clip1 / clip1.w) / 8; // tessellatedTriWidth
	return clamp(t, 0, 64);
}

bool inFrustum(const float3 pt)
{
	float3 eyeToPt = pt - _c._c.rgb;
	float3 v = _c._d.rgb * dot(eyeToPt, _c._d.rgb) - eyeToPt;
	float4 p = mul(mul(float4(pt + normalize(v) * min(1.4, length(v)), 1.0), _c._v), _c._p);
	p /= p.w;
	return abs(p.x) <= 1.0 &&
		   abs(p.y) <= 1.0 &&
		   p.w >= 0 || length(eyeToPt) <= 1.4;
}

// Hull shader (terrain)
_4 CPCFT(InputPatch<_5, 4> ip)
{
	_4 o;
	float3 v0 = ip[0].o + ip[0]._t;
	float3 v1 = ip[1].o + ip[1]._t;
	float3 v2 = ip[2].o + ip[2]._t;
	float3 v3 = ip[3].o + ip[3]._t;

	if (distance(v0, _c._c.rgb) > 200 || !inFrustum(v0) && !inFrustum(v1) && !inFrustum(v2) && !inFrustum(v3))
	{
		o.i[0] = o.i[1] = o.e[0] = o.e[1] = o.e[2] = o.e[3] = -1;
	}
	else
	{
		o.e[0] = SphereToScreenSpaceTessellation(v0, v1);
		o.e[1] = SphereToScreenSpaceTessellation(v1, v2);
		o.e[2] = SphereToScreenSpaceTessellation(v2, v3);
		o.e[3] = SphereToScreenSpaceTessellation(v3, v0);
		o.i[1] = (o.e[0] + o.e[2]) / 2;
		o.i[0] = (o.e[1] + o.e[3]) / 2;
	}

	return o;
}

// Hull shader (plant)
_4 CPCFP(InputPatch<_5, 4> ip)
{
	_4 o;
	float3 v0 = ip[0].o + ip[0]._t;
	float3 v1 = ip[1].o + ip[1]._t;
	float3 v2 = ip[2].o + ip[2]._t;
	float3 v3 = ip[3].o + ip[3]._t;

	if (distance(v0, _c._c.rgb) > 200 || !inFrustum(v0) && !inFrustum(v1) && !inFrustum(v2) && !inFrustum(v3))
	{
		o.i[0] = o.i[1] = o.e[0] = o.e[1] = o.e[2] = o.e[3] = -1;
	}
	else
	{
		o.e[0] = o.e[2] = 1;
		o.e[1] = o.e[3] = 8;
		o.i[0] = 8;
		o.i[1] = 1;
	}

	return o;
}

// Hull shader (terrain)
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CPCFT")]
_5 HS(InputPatch<_5, 4> patch, uint pointId : SV_OutputControlPointID)
{
	return patch[pointId];
}

// Hull shader (plant)
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CPCFP")]
_5 HSP(InputPatch<_5, 4> patch, uint pointId : SV_OutputControlPointID)
{
	return patch[pointId];
}

// Terrain Domain shader
[domain("quad")]
void DS(_4 input, float2 UV : SV_DomainLocation, const OutputPatch<_5, 4> patch, out _2 output)
{
	output.tc = lerp(lerp(patch[0].tc, patch[1].tc, UV.x), lerp(patch[3].tc, patch[2].tc, UV.x), UV.y);
	output.o = lerp(lerp(patch[0].o, patch[1].o, UV.x), lerp(patch[3].o, patch[2].o, UV.x), UV.y);
	output.o.y += terrainHeight(output.tc).r;

	if (_sh == 2)
		output.o.y = 5 - output.o.y;
}

// Texture Vertex shader
void VST(uint vertexId : SV_VertexID, out _3 output)
{
	if (vertexId == 0)
		output.o = float4(-1.0, -1.0, 0.0, 1.0);
	else if (vertexId == 1)
		output.o = float4(-1.0, 1.0, 0.0, 1.0);
	else if (vertexId == 2)
		output.o = float4(1.0, -1.0, 0.0, 1.0);
	else
		output.o = float4(1.0, 1.0, 0.0, 1.0);

	output.tc = output.o.xy * 0.5 + 0.5;
	output.c = 0;
	output.n = _c._c;
}

// Terrain project vertex shader
void VSTP(_2 input, out _3 output)
{
	output.o = mul(mul(float4(input.o, 1), _c._v), _c._p);
	output.c = mul(float4(input.o, 1), _c._lvp);
	output.tc = input.tc;
	output.n = input.o;
}

float grassAmount(float4 t)
{
	return max(0, min(1, smoothstep(0.4, 0.7, t.w) * smoothstep(3.0, 3.5, t.x)));
}

float sandAmount(float4 t)
{
	return max(smoothstep(-0.4, -0.2, -t.w), smoothstep(-3.5, -3.0, -t.x));
}

float3 rayDir(float2 uv)
{
	float2 viewCoord = 2 * uv - 1;
	float3 right = normalize(cross(float3(0, 1, 0), _c._d));
	return normalize(_c._d + right * viewCoord.x * _a + normalize(cross(_c._d, right)) * viewCoord.y);
}

float3 fog(float3 rd)
{
	return pow(saturate(1 - rd.y), 4) * 0.2 + _c._f;
}

// Terrain Pixel shader
float4 PS(_3 input) : SV_TARGET
{
	float alpha = 1.0;
	float shadowFactor = 1.0f;
	float2 uv = input.tc;
		float4 t = terrainHeight(uv);

	float3 f = fog(rayDir(input.tc));
	if (t.x < 2.5)
	{
		if (_sh == 2)
			discard;
		else
			alpha = (0.5 - normalize(input.n - _c._c).y) * saturate(t.x/2.5);
		f *= 0.8;
	}

	float dist = distance(input.n, _c._c);
	float3 n = normalize(float3(t.y, -1, t.z));
		float3 detail3 = fbm(uv * 2000, 4, 1.5, 0.95);
		n = normalize(float3(detail3.y, -1, detail3.z)) * (1 - grassAmount(t)) + n;

	float shadowContrib = saturate(dist / 50.0);
	if (shadowContrib < 1)
	{
		float3 depth = input.c / input.c.w;
			float2 luv = depth.xy*.5 + .5;
			luv = float2(luv.x, 1 - luv.y);
		float sum = 0.0;
		for (float y = -1.5; y <= 1.5; y++)
		for (float x = -1.5; x <= 1.5; x++)
			sum += shadowTexture.SampleCmpLevelZero(cmpSampler, luv + float2(x, y) / 4096, depth.z);
		shadowFactor = lerp(sum / 16.0, 1, shadowContrib);
	}

		float tAmbient = 0.2 + 0.8 * (0.2 + 0.8 * _c._f.a) * (t.x / 10.0);
	float tDiffuse = max(0, dot(n, normalize(_c._l)));
	float3 color = terrainDiffuseTexture.Sample(sampleType, float2(uv.x, 1.0 - uv.y)).rgb;
		color *= lerp(1.0, 2 * fbm(uv * float2(10000, 20000), 4, 1.5, 0.95).x, grassAmount(t));

		float3 ambient = f * tAmbient;
		float3 diffuse = _c._f.a * float3(2.0, 2.0, 1.0) * tDiffuse * (0.2 + 0.8 * shadowFactor);
		return float4(lerp((diffuse + ambient) * color, f, max(0, min(1, dist / 200.0))), alpha);
}

// Terrain Texture Pixel shader
float4 PSTD(_3 input) : SV_TARGET
{
	float2 uv = input.tc;
	float4 t = terrainHeight(uv);
	float3 detail1 = fbm(uv * 2000, 4, 2.0, 0.5);
	float3 detail2 = dnoise2f(uv * 1000);
	float3 detail3 = fbm(uv * 1000, 8, 1.5, 0.95);
	float3 grass = lerp(lerp(float3(0.57, 0.51, 0.0), float3(0.16, 0.37, 0.1), detail2.x), float3(0.03, 0.21, 0.24), detail1.x);
	float3 gray = float3(0.56, 0.62, 0.67);
	float3 sand = lerp(lerp(float3(0.41, 0.37, 0.12), float3(0.57, 0.51, 0.32), detail2.x), gray, detail1.x);
	float3 stone = lerp(gray, float3(0.27, 0.22, 0.11), detail2.x);
	stone *= 0.5 * detail3.x;
	float3 ground = lerp(stone, 0.5 * sand, sandAmount(t));
		float3 color = lerp(ground * (0.5 + 0.5 * dnoise2f(uv * 900).x), 0.5 * grass, grassAmount(t));
		color *= 0.25 + (t.x / 10 - 0.25)*step(t.x, 2.5) + 0.75*sqrt(smoothstep(2.5, 3.0, t.x));
	return float4(color, 1);
}

// HeightMap Pixel shader
float4 PSH(_3 input) : SV_TARGET
{
	float2 n = fbm(input.tc * 20, 3, 2.0, 0.5).yz;
	return float4(fbm(input.tc * 20, 10, 2.0, 0.5).x, n, normalize(float3(n.x, 0.5, n.y)).y);
}

bool intersectPlane(float3 n, float3 p0, float3 ro, float3 rd, out float t)
{
	float denom = dot(n, rd);
	if (denom > 0)
	{
		t = dot(p0 - ro, n) / denom;
		return t >= 0;
	}
	return false;
}

// Cloud Pixel shader
float4 PSC(_3 input) : SV_TARGET
{
	float t = 0;

	float3 ro = _c._c;
	float3 rd = rayDir(input.tc);
	float a = 0;
	float intensity = 2.0;
	if (intersectPlane(float3(0, -1, 0), float3(0, 0, 0), ro, rd, t))
	{
		ro += rd * t;
		rd.y *= -1;
		a = -0.4;
		intensity = 1.2;
	}

	if (intersectPlane(float3(0, 1, 0), float3(0, 200, 0), ro, rd, t))
	{
		ro += rd * t;
		a +=
			pow(abs(fbm(_t * 0.1 - ro.xz / 80, 6, 2.0, 0.5)), 0.1)
			* smoothstep(0.1, 0.9, pow(abs(fbm(-_t * 0.1 - ro.zx / 400, 6, 1.7, 0.49)), 0.6))
			* (1 - pow(min(1, max(0, abs(t) / 100000.0)), 0.1));
	}

	float3 f = fog(rd);
	return float4(lerp(f, lerp((1 - rd.y*.5)*f, intensity, a), pow(rd.y, 0.4)) + smoothstep(0.995, 0.998, dot(rd, -normalize(_c._l))) * float3(2.0, 2.0, 1.0), 1);
}

// Post Processing Pixel shader
float4 PSDD(_3 input, uint nSampleIndex : SV_SAMPLEINDEX) : SV_TARGET
{
	uint x, y, s;
	backBuffer.GetDimensions(x, y, s);

	float2 texCoord = float2(input.tc.x, 1 - input.tc.y);
	float3 finalColor = backBuffer.Load(texCoord * float2(x - 1, y - 1), nSampleIndex).rgb;

	// Sun
	finalColor += pow(0.00001 + _c._f.a, 0.1) * smoothstep(0.0, 1.0, pow(saturate(dot(rayDir(input.tc), -normalize(_c._l))), 1.0 / (0.1 + 1 - _c._f.a))) * float3(2.0, 2.0, 1.0) * 0.2;

	// Color grading
	finalColor = pow(abs(finalColor * (_c._r3 - _c._r1) + _c._r1), 2 * (1 - _c._r2));

	// Vignette
	finalColor *= 1.4 - pow(1.2 * distance(texCoord, float2(0.5, 0.5)), 2);

	// Film grain
	finalColor *= 1 + 0.07 * dnoise2f((texCoord + sin(_t * 333)) * 333).x;

	// Flackern
	finalColor *= 1 + 0.05 * pow(dnoise2f(float2(1, _t * 222)).x, 8);

	// Fade
	finalColor *= min(1, _c._f.a * 33) * min(1, abs(sin(_t * PI) * 9));
	return float4(finalColor, 1);
}

float4 shadeFlora(_3 input, float3 color1, float3 color2, float2 tex)
{
	float dist = distance(input.n, _c._c);

	float3 color = lerp(color1, color2, tex.r);
		color = lerp(color.rgb, color.grb, input.c.x * 0.5) * pow(abs(tex.g), input.c.y);

	float2 uv = (input.n.xz / 256) * 0.5 + 0.5;
		float4 t = terrainHeight(float2(uv.x, uv.y));
		float3 n = normalize(float3(t.y, -1, t.z));
		float tDiffuse = max(0, dot(n, normalize(_c._l)));
	float f = fog(rayDir(input.tc));
	float tAmbient = 0.2 + 0.8 * (0.2 + 0.8 * _c._f.a) * (t.x / 10.0);
	float3 ambient = f * tAmbient;
		float3 diffuse = _c._f.a * float3(1.5, 1.5, 0.6) * tDiffuse;

		return float4(lerp(color * (diffuse + ambient), _c._f.rgb, max(0, min(1, dist / 200.0))), 1.0 - min(1, dist / 100.0));
}

// Plant Pixel shader (normal)
float4 PSTN(_3 input) : SV_TARGET
{
	return shadeFlora(input, float3(0.67, 0.71, 0.14), float3(0.06, 0.37, 0.17), terrainDiffuseTexture.Sample(sampleType, input.tc).rg);
}

// Plant Pixel shader (shadow)
float4 PSTS(_3 input) : SV_TARGET
{
	return float4(input.c.z / input.c.w, 0, 0, 0);
}

// Flower Pixel shader (normal)
float4 FSTN(_3 input) : SV_TARGET
{
	return shadeFlora(input, float3(1, 0, 1), 1, terrainDiffuseTexture.Sample(sampleType, input.tc).ba);
}

// Plant Texture Pixel shader
float4 PSP(_3 input) : SV_TARGET
{
	float y = input.tc.y;
	float x = input.tc.x;

	// Leaf
	float2 uv = input.tc*float2(0.5, 0.25) + float2(0.4, 0);
		float3 noise = shaderTexture.Sample(sampleType, uv).rgb;
		float l = (1 + max(0, sin(pow(2 - 2 * y, 2)))*max(0, sin(x * 5 * PI))*max(step(0.75, x), step(x, 0.25)))
		* (1 - 0.2 * min(1, 1.25*cos((-x + .5) * 5 * PI)*min(step(0.4, x), step(x, 0.6))))
		* max(sin((1 - y)*PI), step(0.5, 1 - y))
		* pow(abs(noise.r), 0.25);

	// Flower
	float c = y + 0.3*sin(x * 8 + 4) - 0.2;
	return float4(y, l, smoothstep(c - 0.1, c + 0.1, 0.7 + 0.1*noise.x), (noise.x*.4 + .6) * (1 - min(1, pow(abs(y), 4))));
}

// Plant Vertex shader
void VSP(_6 instance, uint id : SV_VertexID, out _5 output)
{
	uint vertexId = id % 4;
	if (vertexId == 0)
		output.o = float3(0, -1.0, 1.0);
	else if (vertexId == 1)
		output.o = float3(0, 1.0, 1.0);
	else if (vertexId == 2)
		output.o = float3(0, 1.0, -1.0);
	else
		output.o = float3(0, -1.0, -1.0);

	output.tc = output.o.yz * 0.5 + 0.5;
	output.c = float4(instance.tc.zw, 0, 0);
	output._t = instance._t;
	output._r = instance._r;
	output._s = instance._s;
}

void plantDomain(inout _3 output, int shadow, const OutputPatch<_5, 4> patch, float2 UV)
{
	float2 texCoords = lerp(lerp(patch[0].tc, patch[1].tc, UV.x), lerp(patch[3].tc, patch[2].tc, UV.x), UV.y);
		float x = texCoords.x;
	float y = texCoords.y;
	float t = _t * 8;
	float size = (2 * sin(pow((1 - x) + 0.825, 3)*0.15*PI) - 0.5)*0.1;
	float3 pos = float3(x, -pow(x, 4) + 0.07*((max(sin(y * 2 * PI), sin(y * 2 * PI + PI)) + sin(y*PI)) / 1.765), (2 * y - 1) * size);
		pos = qmul(qmul(patch[0]._r, float4(pos, 0)), qconj(patch[0]._r));
	pos *= (0.5 + patch[0]._s.x) * 0.7 * (0.7 + 0.3 * patch[0].c.x);
	pos += dnoise2f(patch[0]._t.xz + pos.xz * 0.2 + t) * pow(abs(x), 8) * (0.2 + 0.1 * sin(t));
	pos.xyz += patch[0]._t.xyz;
	output.n = pos;
	output.tc = texCoords.yx;
	float4 l = mul(float4(pos, 1.0f), _c._lvp);

	if (shadow == 0)
	{
		output.o = mul(mul(float4(pos, 1.0f), _c._v), _c._p);
		output.c = patch[0].c;
	}
	else
	{
		output.o = l;
		output.c = output.o;
	}
}

// Plant Domain shader (normal)
[domain("quad")]
void DSPN(_4 input, float2 UV : SV_DomainLocation, const OutputPatch<_5, 4> patch, out _3 output)
{
	plantDomain(output, 0, patch, UV);
}

// Plant Domain shader (shadow)
[domain("quad")]
void DSPS(_4 input, float2 UV : SV_DomainLocation, const OutputPatch<_5, 4> patch, out _3 output)
{
	plantDomain(output, 1, patch, UV);
}

void flowerDomain(inout _3 output, int shadow, const OutputPatch<_5, 4> patch, float2 UV)
{
	float2 texCoords = lerp(lerp(patch[0].tc, patch[1].tc, UV.x), lerp(patch[3].tc, patch[2].tc, UV.x), UV.y);
		float x = texCoords.x;
	float y = texCoords.y;
	float size = 0.2*lerp(lerp(8 * x, 0, pow(x, 2)), 0, x);
	float3 pos = float3(x, -log(pow(x, 0.85) + 1), (2 * y - 1) * size);
		pos = qmul(qmul(patch[0]._r, float4(pos, 0)), qconj(patch[0]._r));
	pos *= (0.5 + patch[0]._s.x) * 0.3 * (0.7 + 0.3 * patch[0].c.x);
	pos.xyz += patch[0]._t.xyz;
	pos += dnoise2f(pos.xz + _t) * pow(abs(x), 2) * (0.05 + 0.02 * sin(_t));
	output.n = pos;
	output.tc = texCoords.yx;
	float4 l = mul(float4(pos, 1.0f), _c._lvp);

	if (shadow == 0)
	{
		output.o = mul(mul(float4(pos, 1.0f), _c._v), _c._p);
		output.c = patch[0].c;
	}
	else
	{
		output.o = l;
		output.c = output.o;
	}
}

// Flower Domain shader (normal)
[domain("quad")]
void DSFN(_4 input, float2 UV : SV_DomainLocation, const OutputPatch<_5, 4> patch, out _3 output)
{
	flowerDomain(output, 0, patch, UV);
}

// Flower Domain shader (shadow)
[domain("quad")]
void DSFS(_4 input, float2 UV : SV_DomainLocation, const OutputPatch<_5, 4> patch, out _3 output)
{
	flowerDomain(output, 1, patch, UV);
}

_2 reflect(_2 i)
{
	i.o.y = 5 - i.o.y;
	return i;
}

[maxvertexcount(6)]
// Terrain geometry shader
void GST(triangle _2 input[3], inout PointStream<_2> water, inout PointStream<_2> land)
{
	land.Append(input[0]);
	land.Append(input[1]);
	land.Append(input[2]);
	land.RestartStrip();

	if (input[0].o.y >= 2.5 || input[1].o.y >= 2.5 || input[2].o.y >= 2.5)
	{
		water.Append(reflect(input[0]));
		water.Append(reflect(input[1]));
		water.Append(reflect(input[2]));
		water.RestartStrip();
	}
}

[maxvertexcount(1)]
// Plant layout geometry shader
void GSP(point _2 input[1], inout PointStream<_6> output)
{
	float2 uv = input[0].tc;
		float2 offset = float2(dnoise2f((uv - (1 / 1024)) * 999).x, dnoise2f((uv.yx - (1 / 1024)) * 999).x);
		uv += offset / 512;
	float4 t = terrainHeight(uv);

	if (grassAmount(t) < 0.5)
		return;

	float3 v = input[0].o;
		v.xzy += float3(offset, t.r);

	_6 result;

	result._t = float4(v, 1);
	result._r =
		result._s = 0;
	result.tc = float4(uv, 0, 0);
	output.Append(result);
	output.RestartStrip();
}

// Multiplication Vertex shader
_6 VSM(_6 input)
{
	return input;
}

[maxvertexcount(28)]
// Rotation geometry shader
void GSR(point _6 input[1], inout PointStream<_6> output, inout PointStream<_6> flower)
{
	_6 result = input[0];
	float3 v = input[0]._t;

		float3 v0 = v + float3(0.5, 0, 0.5);
		float3 v1 = v + float3(-0.5, 0, 0.5);
		float3 v2 = v + float3(0.5, 0, -0.5);
		float3 v3 = v + float3(-0.5, 0, -0.5);
	if (distance(v, _c._c) > 100 || !inFrustum(v0) && !inFrustum(v1) && !inFrustum(v2) && !inFrustum(v3))
		return;

	float3 cellNoise = dnoise2f(result._t.xz);
		float2 offset = cellNoise.yz * 0.5;
		float2 uv = input[0].tc.xy;
		float4 t = terrainHeight(uv);
		float3 n = normalize(float3(t.y, 3.0, t.z));
		float3 forward = float3(0, 0, 1);
		float3 up = float3(0, 1, 0);

		float i;
	float leafsUpper = 7.0;
	float leafsLower = 5.0;
	float flowerCount = 4.0;

	float4 q;
	float3 noise;
	result._s = float4(cellNoise, 1);
	for (i = 0; i < 1; i += 1 / leafsUpper)
	{
		noise = dnoise2f(result._t.xz * i * leafsUpper);
		q = quaternion(forward, 1 + 0.5 * noise.x);
		q = qnorm(qmul(quaternion(up, offset.x + i * (1 + 0.2 * noise.x) * 2 * PI), q));
		q = qnorm(qmul(float4(cross(n, up), dot(n, up)), q));
		result._r = q;
		result.tc.z = noise.y;
		result.tc.w = 1;
		output.Append(result);
		output.RestartStrip();
	}

	for (i = 0; i < 1; i += 1 / leafsLower)
	{
		noise = dnoise2f(result._t.xz * i * leafsLower + 1);
		q = quaternion(forward, 0.5 + 0.5 * noise.x);
		q = qnorm(qmul(quaternion(up, offset.y + i * (1 + 0.2 * noise.x) * 2 * PI), q));
		q = qnorm(qmul(float4(float3(-1, 1, 1)*cross(n, up), dot(n, up)), q));
		result._r = q;
		result.tc.z = noise.z;
		result.tc.w = 2;
		output.Append(result);
		output.RestartStrip();
	}

	if (cellNoise.x < 0.25)
	{
		float flowerCount = 1 + floor(cellNoise.x * 4 * 7);

		for (i = 0; i < 1; i += 1 / flowerCount)
		{
			float2 offset = float2(dnoise2f((uv + i) * 283).x, dnoise2f((uv.yx + i) * 185).x)* 0.5;
				uv += offset / 512;
			float4 t = terrainHeight(uv);
				result._t.y = t.x + 0.1;
			result._t.xz += offset;
			result._s = float4(dnoise2f(result._t.xz), 1);

			for (int j = 0; j < 4; ++j)
			{
				q = quaternion(forward, 1 + 0.5 * noise.x);
				q = qnorm(qmul(quaternion(up, (offset.y + j / 4.0) * 2 * PI), q));
				q = qnorm(qmul(float4(cross(n, up), dot(n, up)), q));
				result._r = q;
				result.tc.z = offset.x;
				result.tc.w = 1;
				flower.Append(result);
				flower.RestartStrip();
			}
		}
	}
}

[maxvertexcount(4)]
// Multiplication geometry shader
void GSM(point _6 input[1], inout PointStream<_6> output)
{
	output.Append(input[0]);
	output.RestartStrip();

	output.Append(input[0]);
	output.RestartStrip();

	output.Append(input[0]);
	output.RestartStrip();

	output.Append(input[0]);
	output.RestartStrip();
}

[maxvertexcount(6)]
// Quad geometry shader
void GSQ(point _6 input[1], inout TriangleStream<_3> output)
{
	_3 result;
	float size = 1 / 1024.0;
	float4 location = float4(input[0]._t.xz / 256 + 2 * size, 1, 1); // -1..1

	result.c = 0;
	result.n = 0;

	result.o = location + float4(-size, size, 0, 0);
	result.tc = float2(-1, 1);
	output.Append(result);
	result.o = location + float4(size, -size, 0, 0);
	result.tc = float2(1, -1);
	output.Append(result);
	result.o = location + float4(-size, -size, 0, 0);
	result.tc = float2(-1, -1);
	output.Append(result);
	output.RestartStrip();

	result.o = location + float4(-size, size, 0, 0);
	result.tc = float2(-1, 1);
	output.Append(result);
	result.o = location + float4(size, size, 0, 0);
	result.tc = float2(1, 1);
	output.Append(result);
	result.o = location + float4(size, -size, 0, 0);
	result.tc = float2(1, -1);
	output.Append(result);
	output.RestartStrip();
}
