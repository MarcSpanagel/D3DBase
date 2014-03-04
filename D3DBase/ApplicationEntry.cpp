#include "StdAfx.h"
#include "ApplicationEntry.h"

using namespace std;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	ApplicationEntry app(hInstance);
	app.InitApp();
	app.MessagePump();

}
ApplicationEntry::ApplicationEntry(HINSTANCE hInstance) : D3DBase(hInstance)
{
	meshCount = 0;
	meshTextures = 0;
	fxSkyMapVar = NULL;
	smrv = NULL;
	SkyMapTechnique = NULL;
    NumVertices = 0;
	NumFaces = 0;

	fxSkyMapVar = 0;
	smrv = 0;
	SkyMapTechnique = 0;
	meshTextures = 0;
	D3DXMatrixIdentity(&Scale);
	D3DXMatrixIdentity(&Translation);
	D3DXMatrixIdentity(&Transformations);

}
ApplicationEntry::~ApplicationEntry(void)
{
}

bool ApplicationEntry::InitScene()
{
	HRESULT hr = 0;
	ID3D10Blob* compilationErrors = 0;	

	/////////////////////////new/////////////////////////////////////////////////
	LoadMesh(L"plane.dat");		//meshes[1]

	LoadMesh(L"sphere.dat");	//meshes[0]

	CreateSphere(10, 10);

	D3DX10_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	ID3D10Texture2D* SMTexture = 0;
	hr = D3DX10CreateTextureFromFile(d3dDevice, L"beach1024.dds", 
		&loadSMInfo, 0, (ID3D10Resource**)&SMTexture, 0);

	if(FAILED(hr))
	{
		MessageBox(0, L"Load cube texture - Failed",
			L"Error", MB_OK);
		return false;
	}

	D3D10_TEXTURE2D_DESC texDesc;
	SMTexture->GetDesc(&texDesc);

	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
    viewDesc.TextureCube.MipLevels = texDesc.MipLevels;
    viewDesc.TextureCube.MostDetailedMip = 0;


	hr = d3dDevice->CreateShaderResourceView(SMTexture, &viewDesc, &smrv);

	if(FAILED(hr))
	{
		MessageBox(0, L"Create Cube Texture RV - Failed",
			L"Error", MB_OK);
		return false;
	}
	SMTexture->Release();
	/////////////////////////new/////////////////////////////////////////////////

	light.dir = D3DXVECTOR3(0.25f, 0.5f, -1.0f);
	light.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);



	hr = D3DX10CreateEffectFromFile( L"vertex.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		d3dDevice, NULL, NULL, &FX, &compilationErrors, NULL );
	if(FAILED(hr))
	{
		MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
		compilationErrors->Release();
		return false;
	}

	Technique = FX->GetTechniqueByName( "Tech" );

	/////////////////////////new/////////////////////////////////////////////////
	SkyMapTechnique = FX->GetTechniqueByName("SkyMapTech");
	/////////////////////////new/////////////////////////////////////////////////

	fxWVPVar = FX->GetVariableByName("WVP")->AsMatrix();
	fxDiffuseMapVar = FX->GetVariableByName("DiffuseMap")->AsShaderResource();
	fxLightVar  = FX->GetVariableByName("light");
	fxSkyMapVar = FX->GetVariableByName("SkyMap")->AsShaderResource();
	
	D3D10_PASS_DESC PassDesc;
	Technique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	d3dDevice->CreateInputLayout( layout, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &VertexLayout );

	d3dDevice->IASetInputLayout( VertexLayout );  

	d3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	

	return true;
}

bool ApplicationEntry::LoadMesh(wstring filename)
{
	HRESULT hr = 0;

	ID3DX10Mesh* tempMesh; 

	UINT tempMeshSubsets;

	wifstream fileIn (filename.c_str());
	wstring skipString;

	UINT meshVertices  = 0;
	UINT meshTriangles = 0;

	if (fileIn)
	{
		fileIn >> skipString; // #Subsets
		fileIn >> tempMeshSubsets;
		fileIn >> skipString; // #Vertices
		fileIn >> meshVertices;
		fileIn >> skipString; // #Faces (Triangles)
		fileIn >> meshTriangles;

		meshSubsets.push_back(tempMeshSubsets);

		hr = D3DX10CreateMesh(d3dDevice,
			layout, 
			3, 
			layout[0].SemanticName, 
			meshVertices, 
			meshTriangles, 
			D3DX10_MESH_32_BIT, 
			&tempMesh);

		if(FAILED(hr))
		{
			MessageBox(0, L"Mesh Creation - Failed",
				L"Error", MB_OK);
			return false;
		}

		fileIn >> skipString;	//#Subset_info
		for(UINT i = 0; i < tempMeshSubsets; ++i)
		{
			std::wstring diffuseMapFilename;

			fileIn >> diffuseMapFilename;

			ID3D10ShaderResourceView* DiffuseMapResourceView;

			D3DX10CreateShaderResourceViewFromFile(d3dDevice,
				diffuseMapFilename.c_str(), 0, 0, &DiffuseMapResourceView, 0 );

			TextureResourceViews.push_back(DiffuseMapResourceView);

			meshTextures++;
		}

		Vertex* verts = new Vertex[meshVertices];
		fileIn >> skipString;	//#Vertex_info
		for(UINT i = 0; i < meshVertices; ++i)
		{
			fileIn >> skipString;	//Vertex Position
			fileIn >> verts[i].pos.x;
			fileIn >> verts[i].pos.y;
			fileIn >> verts[i].pos.z;

			fileIn >> skipString;	//Vertex Normal
			fileIn >> verts[i].normal.x;
			fileIn >> verts[i].normal.y;
			fileIn >> verts[i].normal.z;

			fileIn >> skipString;	//Vertex Texture Coordinates
			fileIn >> verts[i].texCoord.x;
			fileIn >> verts[i].texCoord.y;
		}
		tempMesh->SetVertexData(0, verts);

		delete[] verts;

		DWORD* indices = new DWORD[meshTriangles*3];
		UINT* attributeIndex = new UINT[meshTriangles];
		fileIn >> skipString;	//#Face_Index
		for(UINT i = 0; i < meshTriangles; ++i)
		{
			fileIn >> indices[i*3+0];
			fileIn >> indices[i*3+1];
			fileIn >> indices[i*3+2];
			fileIn >> attributeIndex[i];	//Current Subset
		}
		tempMesh->SetIndexData(indices, meshTriangles*3);
		tempMesh->SetAttributeData(attributeIndex);

		delete[] indices;
		delete[] attributeIndex;

		tempMesh->GenerateAdjacencyAndPointReps(0.001f);
		tempMesh->Optimize(D3DX10_MESHOPT_ATTR_SORT|D3DX10_MESHOPT_VERTEX_CACHE,0,0);
		tempMesh->CommitToDevice();

		meshCount++;
		meshes.push_back(tempMesh);
	}
	else
	{
		MessageBox(0, L"Load Mesh File - Failed",
			L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationEntry::CreateSphere(int LatLines, int LongLines)
{
	NumVertices = LatLines * LongLines;
	NumFaces  = (LatLines-1)*(LongLines-1)*2;

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Vertex> vertices(NumVertices);

	D3DXVECTOR3 currVertPos = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	for(DWORD i = 0; i < LatLines; ++i)
	{
		sphereYaw = i * (3.14/LatLines);
		for(DWORD j = 0; j < LongLines; ++j)
		{
			spherePitch = j * (3.14/LongLines);
			D3DXMatrixRotationYawPitchRoll( &RotationMatrix, sphereYaw, spherePitch, 0 );
			D3DXVec3TransformCoord( &currVertPos, &DefaultForward, &RotationMatrix );	
			D3DXVec3Normalize( &currVertPos, &currVertPos );
			vertices[i*LongLines+j].pos = currVertPos;
		}
	}

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof( Vertex ) * NumVertices;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vertices[0];
	d3dDevice->CreateBuffer( &bd, &InitData, &VertexBuffer );

	std::vector<DWORD> indices(NumFaces * 3);

	int k = 0;
	for(DWORD i = 0; i < LatLines-1; ++i)
	{
		for(DWORD j = 0; j < LongLines-1; ++j)
		{
			indices[k]   = i*LongLines+j;
			indices[k+1] = i*LongLines+j+1;
			indices[k+2] = (i+1)*LongLines+j;

			indices[k+3] = (i+1)*LongLines+j;
			indices[k+4] = i*LongLines+j+1;
			indices[k+5] = (i+1)*LongLines+j+1;

			k += 6; // next quad
		}
	}

	D3D10_BUFFER_DESC ibd;
	ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * NumFaces * 3;
	ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	d3dDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer);

	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers( 0, 1, &VertexBuffer, &stride, &offset );
	d3dDevice->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

}

void ApplicationEntry::Render()
{
	//Draw Scene Here
	D3DXCOLOR bgColor( 0.0f, 0.0f, 0.0f, 1.0f);
	d3dDevice->ClearRenderTargetView( RenderTargetView, bgColor );
	d3dDevice->ClearDepthStencilView(DepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	D3DXMatrixPerspectiveFovLH(&Projection, 0.4f*3.14f, Width/Height, 1.0f, 100.0f);

	fxLightVar->SetRawValue(&light, 0, sizeof(Light));
	/////////////////////////new/////////////////////////////////////////////////
	D3D10_TECHNIQUE_DESC skymaptechDesc;
	SkyMapTechnique->GetDesc( &skymaptechDesc );
	/////////////////////////new/////////////////////////////////////////////////
	
	D3D10_TECHNIQUE_DESC techDesc;
	Technique->GetDesc( &techDesc );
	
	D3DXMatrixScaling( &Scale, 1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation( &Translation, 0.0f, -3.0f, 0.0f );

	Transformations = Scale * Translation;

	WVP = World * Transformations * View * Projection;
	fxWVPVar->SetMatrix((float*)&WVP);
	
	//draw plane
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		for(UINT subsetID = 0; subsetID < meshSubsets[0]; ++subsetID)
		{
			fxDiffuseMapVar->SetResource(TextureResourceViews[subsetID]);	
			Technique->GetPassByIndex( p )->Apply( 0 );
			meshes[0]->DrawSubset(subsetID);
		}
	}
	
	D3DXMatrixScaling( &Scale, 2.0f, 2.0f, 2.0f );
	D3DXMatrixTranslation( &Translation, 0.0f, 1.0f, 5.0f );

	Transformations = Scale * Translation;

	WVP = World * Transformations * View * Projection;
	fxWVPVar->SetMatrix((float*)&WVP);
	
	//draw sphere
	
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		for(UINT subsetID = 0; subsetID < meshSubsets[1]; ++subsetID)
		{
			fxDiffuseMapVar->SetResource(TextureResourceViews[subsetID + meshSubsets[0]]);	
			Technique->GetPassByIndex( p )->Apply( 0 );
			meshes[1]->DrawSubset(subsetID);
		}
	}
	
	D3DXMatrixScaling( &Scale, 10.0f, 10.0f,10.0f );
	D3DXMatrixTranslation( &Translation, Position.x, Position.y, Position.z );

	Transformations = Scale * Translation;

	WVP = World * Transformations * View * Projection;
	fxWVPVar->SetMatrix((float*)&WVP);

	/////////////////////////new/////////////////////////////////////////////////
	//draw skymap based off loaded mesh sphere
	fxSkyMapVar->SetResource(smrv);	
	for( UINT p = 0; p < skymaptechDesc.Passes; ++p )
	{
		SkyMapTechnique->GetPassByIndex( p )->Apply( 0 );	
		d3dDevice->DrawIndexed(NumFaces * 3, 0, 0);
	}
	/////////////////////////new/////////////////////////////////////////////////

	SwapChain->Present( 0, 0 );
}
void ApplicationEntry::InitApp()
{
	D3DBase:Init();
}



