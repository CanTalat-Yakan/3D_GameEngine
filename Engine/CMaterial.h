#pragma once
#include "CLighting.h"
#include "CDirect.h"
#include "CCamera.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3dx11.lib")

using namespace DirectX;

class CMaterial
{
public:
	int Init(LPCWSTR _shaderName, LPCWSTR _textureName);
	int Init(LPCWSTR _shaderName, LPCWSTR _textureName, LPCWSTR _normalTextureName);
	void Render(XMMATRIX _worldMatrix);
	void Release();


private:
	CDirect* p_d3d;
	CCamera* p_camera;
	CLighting* p_lighting;


	int createVertexShader(LPCWSTR _shaderName);
	int createPixelShader(LPCWSTR _shaderName);
	int createInputLayout(ID3DBlob* _pBlob);
	int createMatrixBuffer();
	int createLightingBuffer();
	int createTextureAndSampler(LPCWSTR _textureName, ID3D11ShaderResourceView** _texture_SRV);

	void setMatrixBuffer(XMMATRIX _worldMatrix);
	void SetLightingBuffer();

	// shader
	ID3D11VertexShader* p_vertexShader = nullptr;
	ID3D11PixelShader* p_pixelShader = nullptr;

	// input layout
	ID3D11InputLayout* p_inputLayout = nullptr;

	// texture
	ID3D11ShaderResourceView* p_texture_SRV = nullptr;
	ID3D11ShaderResourceView* p_normalTexture_SRV = nullptr;
	ID3D11SamplerState* p_texture_SS = nullptr;

	// constant buffer
	ID3D11Buffer* p_cbMatrix = nullptr;
	struct cbMatrix
	{
		XMMATRIX WVP;
		XMMATRIX World;
		XMFLOAT3 WCP;
	};

	ID3D11Buffer* p_cbLighting = nullptr;
	struct cbLighting
	{
		CDirectionalLight dirLight;
		CPointLight pointLight;
	};
};