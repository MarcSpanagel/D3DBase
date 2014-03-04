#pragma once
#include "StdAfx.h"
#include "D3DBase.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <istream>


struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v, float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u,v), normal(nx,ny,nz){}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR3 normal;
};

D3D10_INPUT_ELEMENT_DESC layout[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D10_INPUT_PER_VERTEX_DATA, 0}
};

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	D3DXVECTOR3 dir;
	float pad;
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
};

class ApplicationEntry : public D3DBase
{
public:
	ApplicationEntry(HINSTANCE hInstance);
	~ApplicationEntry(void);
	void ApplicationEntry::InitApp();
	bool InitScene();
	bool LoadMesh(std::wstring filename);
	void CreateSphere(int LatLines, int LongLines);
	virtual void Render();

	std::vector<ID3DX10Mesh*> meshes;
	int meshCount;
	int meshTextures;
	std::vector<ID3D10ShaderResourceView*> TextureResourceViews;
	std::vector<UINT> meshSubsets;
protected:
	ID3D10EffectShaderResourceVariable* fxSkyMapVar;
	ID3D10ShaderResourceView* smrv;
	ID3D10EffectTechnique* SkyMapTechnique;
	Light light;
	int NumVertices;
	int NumFaces;

	D3DXMATRIX Scale;
	D3DXMATRIX Translation;
	D3DXMATRIX Transformations;
};

