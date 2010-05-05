#ifndef DXMANAGE_HEADER
#define DXMANAGE_HEADER

class CDirectXManager
{
public:
	CDirectXManager();
	~CDirectXManager();

	void CreateRenderWindow(HWND);
	void InitDevice();
	void Prepare_DX();

	void CleanupDevice();

	void StartRender();
	void EndRender();

	void DrawLandscapeQuadrant(ID3D10Buffer *,ID3D10Buffer *,unsigned long,unsigned long);
	void DrawMeshStrip(ID3D10Buffer *,ID3D10Buffer *,unsigned long,unsigned long,float *,float *,float);
	void DrawMesh(ID3D10Buffer *,ID3D10Buffer *,unsigned long,unsigned long,float *,float *,float);

	void UpdateCamera();
	bool IsVertexVisible(float *);

	ID3D10Buffer *CreateVertexBuffer(unsigned char *,unsigned long,unsigned long);
	ID3D10Buffer *CreateIndiceBuffer(unsigned char *,unsigned long,unsigned long);
	ID3D10ShaderResourceView *CreateTexture(WCHAR *);

	void SetLandscapeTexture(ID3D10ShaderResourceView *,unsigned long);
	void SetSimpleMeshTexture(ID3D10ShaderResourceView *);

protected:
	D3D10_DRIVER_TYPE       g_driverType;
	ID3D10Device*           pd3dDevice;
	IDXGISwapChain*         g_pSwapChain;
	ID3D10RenderTargetView* g_pRenderTargetView;
	
	ID3D10Effect *pLandscapeEffect;
	ID3D10EffectTechnique*  pLandscapeTechnique;
	ID3D10InputLayout*      pLandscapeVertexLayout;
	ID3D10EffectMatrixVariable* pLandscapeViewVariable;
	ID3D10EffectMatrixVariable* pLandscapeProjectionVariable;
	ID3D10EffectShaderResourceVariable *landscape_texture[9];


	ID3D10Effect *pSimpleMeshEffect;
	ID3D10EffectTechnique*  pSimpleMeshTechnique;
	ID3D10InputLayout*      pSimpleMeshVertexLayout;
	ID3D10EffectMatrixVariable* pSimpleMeshWorldVariable;
	ID3D10EffectMatrixVariable* pSimpleMeshViewVariable;
	ID3D10EffectMatrixVariable* pSimpleMeshProjectionVariable;
	ID3D10EffectShaderResourceVariable *simplemesh_texture;


	ID3D10Texture2D*            g_pDepthStencil;
	ID3D10DepthStencilView*     g_pDepthStencilView;


	D3DXMATRIX                  g_View;
	D3DXMATRIX                  g_Projection;

    D3DXVECTOR3 Up,Look,Right,Position;
	D3DXMATRIX yawMatrix,pitchMatrix,rollMatrix;
};

#endif