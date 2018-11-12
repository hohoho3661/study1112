#include "헤더.h"

// Vertex(정점)

// 3D 그래픽에서 최소 처리 단위
// 평면(삼각형)을 구성하는 3개의 정보

// 3차원상에서 어떠한 오브젝트를 표시하기위한 ,,, 최소(기본) 정보들        ( 2차원에서는 표시하기위한 기본정보가 좌표인것 처럼 )
// 색정보, 텍스쳐정보 등을 포함할수 있다.
// 하나의 표현단위의 기본은 삼각형,,, 3개의 버텍스
// 평면에 대한 처리 -> rasterize

// 임의로 만든거 = 커스텀 버텍스



// DirectX 관련 전역 변수

LPDIRECT3D9				g_pD3D;			// D3D 객체 포인터					다이렉트3D 객체
LPDIRECT3DDEVICE9		g_pD3DDevice;	// D3D Device 객체 포인터			다이렉트로 접근하는 3D Device 객체
LPDIRECT3DVERTEXBUFFER9	g_pVB;			// D3D Vertex Buffer 객체 포인터

//
struct CUSTOMVERTEX
{
	FLOAT				x, y, z, rtw;	// x y z 3차원 정보에 ,,, rtw( 4차원백터 (4차 행렬로 변환하기 위해) )를 추가
	DWORD				color;
};

#define FVF				(D3DFVF_XYZRHW | D3DFVF_DIFFUSE) // 정점정보를 담고있는것


// Window 관련 전역 변수
HWND					g_hWnd;
HINSTANCE				g_hInstance;

HRESULT					InitD3D(HWND hwnd);
HRESULT					InitVB();
VOID					CleanUp();
VOID					Render();

LRESULT WINAPI	MsgProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI	WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutorial", NULL
	};

	RegisterClassEx(&wc);

	g_hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial", WS_OVERLAPPEDWINDOW,
		100, 100, 1000, 800, NULL, NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(g_hWnd)))
	{
		if (SUCCEEDED(InitVB()))
		{
			ShowWindow(g_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(g_hWnd);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					Render();
				}
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance);
	
	return 0;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		Render();
		ValidateRect(hWnd, NULL);
		return 0;
	case WM_DESTROY:
		CleanUp();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HRESULT InitD3D(HWND hWnd) // 다이렉트     초기화 작업
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) // 헤더버전과 DLL버전이 맞는지 확인
	{
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp; // PRESENT = 화면에 표시한다.
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 백퍼버 간의 스왑 이펙트,, DISCARD = 지우는것,,
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if (FAILED(g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,  // 하드웨어 하위 규약을 지켯는지? 확인
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, // 버택스를 관리하는곳이 하드웨어인지 소프트웨어인지
		&d3dpp,
		&g_pD3DDevice)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT InitVB()
{
	CUSTOMVERTEX vertices[] =						// 버텍스를 만들어서 배열에 넣기
	{
		//{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, },
		//{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		//{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff0000ff, },

		{ 250.0f, 150.0f, 150.5f, 1.0f, 0xffff0000, },
		{ 350.0f, 550.0f, 150.5f, 1.0f, 0xff00ff00, },
		{ 150.0f, 250.0f, 150.5f, 1.0f, RGB(0,0,1), },
	};

	if (FAILED(g_pD3DDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0,
		FVF, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;

	memcpy(pVertices, vertices, sizeof(vertices));

	g_pVB->Unlock();


	return S_OK;
}

VOID CleanUp() // 생성순서의 역순으로 해제,, g_pD3D를 기반으로 g_pD3DDevice를 만들었으니까,
{
	if (g_pVB != NULL)
		g_pVB->Release();
	if (g_pD3DDevice != NULL)
		g_pD3DDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();
}

VOID Render()
{
	if (NULL == g_pD3DDevice) // 있는지 없는지 확인 ,, 얘를 통해 동작이 이루어지니까
		return;

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene())) // 비긴에 접속할수있게 해줌
	{
		//
		g_pD3DDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pD3DDevice->SetFVF(FVF);
		g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// 이 사이에 랜더를 작업해야됨,,

		//
		g_pD3DDevice->EndScene(); //
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL); // 화면출력
}