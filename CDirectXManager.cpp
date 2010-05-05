#include "stdafx.h"
#include "Global.h"
#include "DDBlivion.h"
#include "CDirectXManager.h"

typedef struct
{
    D3DXVECTOR3 Pos;
	D3DXVECTOR3 Norm;
	D3DXVECTOR4 Color;
	D3DXVECTOR2 TextUV;
	float weights[9];
} VertexLand;

LRESULT CALLBACK DxWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

CDirectXManager::CDirectXManager()
{
	g_driverType = D3D10_DRIVER_TYPE_NULL;
	pd3dDevice = NULL;
	g_pSwapChain = NULL;
	g_pRenderTargetView = NULL;
	pLandscapeEffect = NULL;
	pLandscapeTechnique = NULL;
	pLandscapeVertexLayout = NULL;
	g_pDepthStencil = NULL;
	g_pDepthStencilView = NULL;
	pLandscapeViewVariable = NULL;
	pLandscapeProjectionVariable = NULL;
}

CDirectXManager::~CDirectXManager()
{
}

void CDirectXManager::StartRender()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    pd3dDevice->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    pd3dDevice->ClearDepthStencilView( g_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );
}

void CDirectXManager::EndRender()
{
	HRESULT result;
    // Present the information rendered to the back buffer to the front buffer (the screen)
    result=g_pSwapChain->Present(0,0);
	switch(result)
	{
	case DXGI_ERROR_DEVICE_RESET:
		output_text(L"DXGI ERROR DEVICE RESET!\r\n");
		break;
	case DXGI_ERROR_DEVICE_REMOVED:
		output_text(L"DXGI ERROR DEVICE REMOVED!\r\n");
		break;
	case DXGI_STATUS_OCCLUDED:
		output_text(L"DXGI STATUS OCCLUDED!\r\n");
		break;
	}

	if(result!=S_OK) throw L"Error SwapChain->Present";
}

void CDirectXManager::DrawLandscapeQuadrant(ID3D10Buffer *VertexBuf,ID3D10Buffer *IndiceBuf,unsigned long numDraw,unsigned long sizeVert)
{
    UINT stride = sizeVert;
    UINT offset = 0;
    // Set the input layout
    pd3dDevice->IASetInputLayout(pLandscapeVertexLayout);
	//Set Vertex buf
    pd3dDevice->IASetVertexBuffers( 0, 1, &VertexBuf, &stride, &offset );
	//Set indice buffer
	pd3dDevice->IASetIndexBuffer(IndiceBuf,DXGI_FORMAT_R16_UINT,0);
    // Set primitive topology
    pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    pLandscapeViewVariable->SetMatrix( ( float* )&g_View );
    pLandscapeProjectionVariable->SetMatrix( ( float* )&g_Projection );

    D3D10_TECHNIQUE_DESC techDesc;
    pLandscapeTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pLandscapeTechnique->GetPassByIndex( p )->Apply( 0 );
        pd3dDevice->DrawIndexed(numDraw,0,0); // (((33*2)*(33-1))+(33-2))
    }
}

void CDirectXManager::DrawMeshStrip(ID3D10Buffer *VertexBuf,ID3D10Buffer *IndiceBuf,unsigned long numDraw,unsigned long sizeVert,float *position,float *rotation,float scale)
{
    UINT stride = sizeVert;
    UINT offset = 0;
    // Set the input layout
    pd3dDevice->IASetInputLayout(pSimpleMeshVertexLayout);
	//Set Vertex buf
    pd3dDevice->IASetVertexBuffers( 0, 1, &VertexBuf, &stride, &offset );
	//Set indice buffer
	pd3dDevice->IASetIndexBuffer(IndiceBuf,DXGI_FORMAT_R16_UINT,0);
    // Set primitive topology
    pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3DXMATRIX mWorld;
	D3DXMATRIX mScaling;
	D3DXMATRIX mRotation;
	D3DXMATRIX mTranslation;

	D3DXMatrixIdentity(&mScaling);
	D3DXMatrixIdentity(&mRotation);
	D3DXMatrixIdentity(&mTranslation);

	//D3DXMatrixRotationYawPitchRoll(&mRotation,(float)-((rotation[0]/180.0f)*D3DX_PI),(float)-((rotation[1]/180.0f)*D3DX_PI),(float)-((rotation[2]/180.0f)*D3DX_PI));
	D3DXMatrixRotationYawPitchRoll(&mRotation,-rotation[0],-rotation[1],-rotation[2]);
	D3DXMatrixTranslation(&mTranslation,position[0],position[1],position[2]);
	D3DXMatrixScaling(&mScaling,scale,scale,scale);

	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mRotation);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mScaling);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mTranslation);

	pSimpleMeshWorldVariable->SetMatrix((float *)&mWorld);
	pSimpleMeshViewVariable->SetMatrix((float*)&g_View);
    pSimpleMeshProjectionVariable->SetMatrix((float*)&g_Projection);

    D3D10_TECHNIQUE_DESC techDesc;
    pSimpleMeshTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pSimpleMeshTechnique->GetPassByIndex( p )->Apply( 0 );
        pd3dDevice->DrawIndexed(numDraw,0,0); // (((33*2)*(33-1))+(33-2))
    }
}

void CDirectXManager::DrawMesh(ID3D10Buffer *VertexBuf,ID3D10Buffer *IndiceBuf,unsigned long numDraw,unsigned long sizeVert,float *position,float *rotation,float scale)
{
    UINT stride = sizeVert;
    UINT offset = 0;
    // Set the input layout
    pd3dDevice->IASetInputLayout(pSimpleMeshVertexLayout);
	//Set Vertex buf
    pd3dDevice->IASetVertexBuffers( 0, 1, &VertexBuf, &stride, &offset );
	//Set indice buffer
	pd3dDevice->IASetIndexBuffer(IndiceBuf,DXGI_FORMAT_R16_UINT,0);
    // Set primitive topology
    pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX mWorld;
	D3DXMATRIX mScaling;
	D3DXMATRIX mRotation;
	D3DXMATRIX mTranslation;

	D3DXMatrixIdentity(&mScaling);
	D3DXMatrixIdentity(&mRotation);
	D3DXMatrixIdentity(&mTranslation);

	//D3DXMatrixRotationYawPitchRoll(&mRotation,(float)-((rotation[0]/180.0f)*D3DX_PI),(float)-((rotation[1]/180.0f)*D3DX_PI),(float)-((rotation[2]/180.0f)*D3DX_PI));
	D3DXMatrixRotationYawPitchRoll(&mRotation,-rotation[0],-rotation[1],-rotation[2]);
	D3DXMatrixTranslation(&mTranslation,position[0],position[1],position[2]);
	D3DXMatrixScaling(&mScaling,scale,scale,scale);

	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mRotation);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mScaling);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mTranslation);

	pSimpleMeshWorldVariable->SetMatrix((float *)&mWorld);
	pSimpleMeshViewVariable->SetMatrix((float*)&g_View);
    pSimpleMeshProjectionVariable->SetMatrix((float*)&g_Projection);

    D3D10_TECHNIQUE_DESC techDesc;
    pSimpleMeshTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pSimpleMeshTechnique->GetPassByIndex( p )->Apply( 0 );
        pd3dDevice->DrawIndexed(numDraw,0,0); // (((33*2)*(33-1))+(33-2))
    }
}

bool CDirectXManager::IsVertexVisible(float *coords)
{
	D3DXVECTOR3 thevertex(coords[0],coords[1],coords[2]);
	D3DXVec3TransformCoord(&thevertex,&thevertex,&g_View);
	D3DXVec3TransformCoord(&thevertex,&thevertex,&g_Projection);

	if((thevertex.x>1.4f)||(thevertex.x<-1.4f)) return false;
	if((thevertex.y>1.4f)||(thevertex.y<-1.4f)) return false;
	if((thevertex.z<0.1f)) return false;

	return true;
}

void CDirectXManager::SetLandscapeTexture(ID3D10ShaderResourceView *daset,unsigned long num)
{
	if((landscape_texture[num]->SetResource(daset))!=S_OK)
	{
		output_text_value(L"Error setting Texture:",num);
	}
}

void CDirectXManager::SetSimpleMeshTexture(ID3D10ShaderResourceView *daset)
{
	if((simplemesh_texture->SetResource(daset))!=S_OK)
	{
		output_text(L"Error setting SimpleMesh Texture.\r\n");
	}
}

void CDirectXManager::UpdateCamera()
{
	long mouseX,mouseY;
	float angleX,angleY;
	float playerpos[3];
	float roll=0.0f;

	pInputManager->Get_MouseXY(mouseX,mouseY);

	angleX=pGlobal->GetYaw();
	angleY=pGlobal->GetPitch();

	angleX-=(float)(D3DX_PI/500.0f)*mouseX;
	angleY-=(float)(D3DX_PI/500.0f)*mouseY;

	if(angleY>(float)((D3DX_PI/2)-0.1f)) angleY=(float)((D3DX_PI/2)-0.1f);
	if(angleY<(float)(-((D3DX_PI/2)-0.1f))) angleY=(float)(-((D3DX_PI/2)-0.1f));

	pGlobal->SetYaw(angleX);
	pGlobal->SetPitch(angleY);
	
	//Camera position in FP view
	pGlobal->GetPlayerPosition(playerpos);

	//Init the values to default
	Up=D3DXVECTOR3(0.0f,0.0f,1.0f);
	Look=D3DXVECTOR3(0.0f,1.0f,0.0f);
	Right=D3DXVECTOR3(1.0f,0.0f,0.0f);
	Position=D3DXVECTOR3(playerpos[0],playerpos[1],playerpos[2]);

	D3DXMatrixRotationAxis(&yawMatrix, &Up, angleX);
	D3DXVec3TransformCoord(&Look, &Look, &yawMatrix);
	D3DXVec3TransformCoord(&Right, &Right, &yawMatrix); 

	D3DXMatrixRotationAxis(&pitchMatrix, &Right, angleY); 
	D3DXVec3TransformCoord(&Look, &Look, &pitchMatrix);
	D3DXVec3TransformCoord(&Up, &Up, &pitchMatrix);

	D3DXMatrixIdentity(&g_View);

	g_View._11 = Right.x; g_View._12 = Up.x; g_View._13 = Look.x;
	g_View._21 = Right.y; g_View._22 = Up.y; g_View._23 = Look.y;
	g_View._31 = Right.z; g_View._32 = Up.z; g_View._33 = Look.z;

	g_View._41 = - D3DXVec3Dot( &Position, &Right );
	g_View._42 = - D3DXVec3Dot( &Position, &Up );
	g_View._43 = - D3DXVec3Dot( &Position, &Look );
}


void CDirectXManager::Prepare_DX()
{
    D3D10_PASS_DESC PassDesc;
	UINT numElements;
	HRESULT hr = S_OK;

	LPD3D10BLOB blob = NULL;
	D3D10CreateBlob(512, &blob);

    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
    hr = D3DX10CreateEffectFromFile(L"LandscapeShader.fx",NULL,NULL,"fx_4_0",dwShaderFlags,0,pd3dDevice,NULL,NULL,&pLandscapeEffect,&blob,NULL);
    if(FAILED(hr))
    {
		MessageBoxA(NULL,(char *)blob->GetBufferPointer(),"Error FX compilation",MB_OK);
        throw L"Failed to compile LandscapeShader.fx file...";
    }

    hr = D3DX10CreateEffectFromFile(L"SimpleMeshShader.fx",NULL,NULL,"fx_4_0",dwShaderFlags,0,pd3dDevice,NULL,NULL,&pSimpleMeshEffect,&blob,NULL);
    if(FAILED(hr))
    {
		MessageBoxA(NULL,(char *)blob->GetBufferPointer(),"Error FX compilation",MB_OK);
        throw L"Failed to compile SimpleMeshShader.fx file...";
    }

	//Process Simple Mesh
	pSimpleMeshTechnique=pSimpleMeshEffect->GetTechniqueByName("RenderSMesh");
	pSimpleMeshWorldVariable=pSimpleMeshEffect->GetVariableByName("World")->AsMatrix();
	pSimpleMeshViewVariable=pSimpleMeshEffect->GetVariableByName("View")->AsMatrix();
	pSimpleMeshProjectionVariable=pSimpleMeshEffect->GetVariableByName("Projection")->AsMatrix();

	simplemesh_texture=pSimpleMeshEffect->GetVariableByName("datext")->AsShaderResource();

    D3D10_INPUT_ELEMENT_DESC simplemesh_layout[] =
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    numElements = sizeof( simplemesh_layout ) / sizeof( simplemesh_layout[0] );

    pSimpleMeshTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = pd3dDevice->CreateInputLayout( simplemesh_layout, numElements, PassDesc.pIAInputSignature,
                                          PassDesc.IAInputSignatureSize, &pSimpleMeshVertexLayout );
    if(FAILED(hr))
	{
		output_text(L"Failed to create Input Layout!\r\n");
		switch(hr)
		{
		case D3DERR_INVALIDCALL:
			output_text(L"Invalid Call...");
			break;
		case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			output_text(L"Too many unique state objects!\r\n");
			break;
		case E_INVALIDARG:
			output_text(L"Invalid arg!\r\n");
			break;
		}
		throw L"Failed to create input layout...";
	}

    //Process Landscape
    pLandscapeTechnique=pLandscapeEffect->GetTechniqueByName( "RenderLand" );
    pLandscapeViewVariable=pLandscapeEffect->GetVariableByName( "View" )->AsMatrix();
    pLandscapeProjectionVariable = pLandscapeEffect->GetVariableByName( "Projection" )->AsMatrix();

	landscape_texture[0]=pLandscapeEffect->GetVariableByName("datext0")->AsShaderResource();
	landscape_texture[1]=pLandscapeEffect->GetVariableByName("datext1")->AsShaderResource();
	landscape_texture[2]=pLandscapeEffect->GetVariableByName("datext2")->AsShaderResource();
	landscape_texture[3]=pLandscapeEffect->GetVariableByName("datext3")->AsShaderResource();
	landscape_texture[4]=pLandscapeEffect->GetVariableByName("datext4")->AsShaderResource();
	landscape_texture[5]=pLandscapeEffect->GetVariableByName("datext5")->AsShaderResource();
	landscape_texture[6]=pLandscapeEffect->GetVariableByName("datext6")->AsShaderResource();
	landscape_texture[7]=pLandscapeEffect->GetVariableByName("datext7")->AsShaderResource();
	landscape_texture[8]=pLandscapeEffect->GetVariableByName("datext8")->AsShaderResource();

    D3D10_INPUT_ELEMENT_DESC landscape_layout[] =
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    numElements = sizeof( landscape_layout ) / sizeof( landscape_layout[0] );

    pLandscapeTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = pd3dDevice->CreateInputLayout( landscape_layout, numElements, PassDesc.pIAInputSignature,
                                          PassDesc.IAInputSignatureSize, &pLandscapeVertexLayout );
    if(FAILED(hr))
	{
		output_text(L"Failed to create Input Layout!\r\n");
		switch(hr)
		{
		case D3DERR_INVALIDCALL:
			output_text(L"Invalid Call...");
			break;
		case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			output_text(L"Too many unique state objects!\r\n");
			break;
		case E_INVALIDARG:
			output_text(L"Invalid arg!\r\n");
			break;
		}
		throw L"Failed to create input layout...";
	}

    // Initialize the projection matrix
	D3DXMatrixPerspectiveFovLH( &g_Projection, ( float )D3DX_PI * 0.25f, 640.0f / 480.0f, 0.1f, 1000000.0f );
}

ID3D10Buffer *CDirectXManager::CreateVertexBuffer(unsigned char *dabuf,unsigned long numVert,unsigned long sizeVert)
{
    D3D10_BUFFER_DESC bd;
	ID3D10Buffer *pVertexBuf;
	HRESULT hr;

	memset(&bd,0,sizeof(D3D10_BUFFER_DESC));

    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeVert * numVert;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = dabuf;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pVertexBuf );
    if(FAILED(hr))
	{
		output_text(L"Failed CreateBuffer!\r\n");
		return NULL;
	}

	return pVertexBuf;
}

ID3D10Buffer *CDirectXManager::CreateIndiceBuffer(unsigned char *dabuf,unsigned long numInd,unsigned long sizeofInd)
{
    D3D10_BUFFER_DESC bd;
	ID3D10Buffer *pIndiceBuf;
	HRESULT hr;

	memset(&bd,0,sizeof(D3D10_BUFFER_DESC));

    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeofInd * numInd;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = dabuf;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pIndiceBuf );
    if(FAILED(hr))
	{
		output_text(L"Failed CreateIndiceBuffer!\r\n");
		return NULL;
	}

	return pIndiceBuf;
}

ID3D10ShaderResourceView *CDirectXManager::CreateTexture(WCHAR *filename)
{
	ID3D10ShaderResourceView *daTexture;
	HRESULT result;

	result=D3DX10CreateShaderResourceViewFromFile(pd3dDevice,filename,NULL,NULL,&daTexture,NULL);
	if(FAILED(result))
	{
		output_text(L"Failed CreateTexture:");
		output_text(filename);
		output_text(L"\r\n");
		return NULL;
	}
	return daTexture;
}

void CDirectXManager::CreateRenderWindow(HWND parent_window)
{
    WNDCLASSEX wcex;
	memset(&wcex,0,sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = DxWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = g_hInst;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = L"DX_Wnd_Class";
    if(!RegisterClassEx(&wcex)) throw L"Failed to create DX wnd class...";

    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_directx_hwnd = CreateWindow( L"DX_Wnd_Class", L"DDBlivion v0.001a", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, parent_window, NULL,
                           g_hInst, NULL);
	if(!g_directx_hwnd) throw L"Failed to create DX window...";

    ShowWindow( g_directx_hwnd, SW_SHOW );
}

void CDirectXManager::InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_directx_hwnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    UINT createDeviceFlags = 0;

    D3D10_DRIVER_TYPE driverTypes[] =
    {
        D3D10_DRIVER_TYPE_HARDWARE,
        D3D10_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_directx_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for(UINT driverTypeIndex=0;driverTypeIndex < numDriverTypes;driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D10CreateDeviceAndSwapChain(NULL,g_driverType,NULL,createDeviceFlags,D3D10_SDK_VERSION,&sd,&g_pSwapChain,&pd3dDevice);
        if(SUCCEEDED(hr)) break;
    }
    if(FAILED(hr)) throw L"Failed to initialize DX device...";

    ID3D10RasterizerState * g_pRasterState;

    D3D10_RASTERIZER_DESC rasterizerState;
    rasterizerState.FillMode = D3D10_FILL_SOLID;
    rasterizerState.CullMode = D3D10_CULL_BACK;
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.DepthBias = false;
    rasterizerState.DepthBiasClamp = 0;
    rasterizerState.SlopeScaledDepthBias = 0;
    rasterizerState.DepthClipEnable = false;
    rasterizerState.ScissorEnable = false;
    rasterizerState.MultisampleEnable = false;
    rasterizerState.AntialiasedLineEnable = false;
    pd3dDevice->CreateRasterizerState( &rasterizerState, &g_pRasterState );
	pd3dDevice->RSSetState(g_pRasterState);


    // Create a back buffer to render to
    ID3D10Texture2D* pBackBuffer;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
    if(FAILED(hr)) throw L"Failed to Get DX BackBuffer...";

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer,NULL,&g_pRenderTargetView );
    pBackBuffer->Release();
    if(FAILED(hr)) throw L"Failed to create Render Target View...";

    // Create depth stencil texture
    D3D10_TEXTURE2D_DESC descDepth;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = pd3dDevice->CreateTexture2D(&descDepth,NULL,&g_pDepthStencil);
    if(FAILED(hr)) throw L"Failed to create depth stencil texture...";

    // Create the depth stencil view
    D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if(FAILED(hr)) throw L"Failed to create depth stencil view...";

    pd3dDevice->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D10_VIEWPORT vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pd3dDevice->RSSetViewports( 1, &vp );
}

void CDirectXManager::CleanupDevice()
{
    if(pd3dDevice) pd3dDevice->ClearState();
    if(g_pRenderTargetView) g_pRenderTargetView->Release();
    if(g_pSwapChain) g_pSwapChain->Release();
    if(pd3dDevice) pd3dDevice->Release();
}
