#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Effect.h"
#include "Camera.h"
#include "Utils.h"
using namespace dae::Utils;

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";

			/*std::vector<Vertex_PosCol>vertices{
					Vertex_PosCol{{-3,3,-2},{1,1,1}, {0.f,0.f},{},{}},
					Vertex_PosCol{{0,3,-2},{1,1,1}, {.5f,0.f},{},{}},
					Vertex_PosCol{{3,3,-2},{1,1,1}, {1.f,0.f},{},{}},
					Vertex_PosCol{{ -3,0,-2},{1,1,1}, {0.f,0.5f},{},{}},
					Vertex_PosCol{{0,0,-2},{1,1,1}, {.5f,.5f},{},{}},
					Vertex_PosCol{{3,0,-2},{1,1,1}, {1.f,.5f},{},{}},
					Vertex_PosCol{{-3,-3,-2},{1,1,1}, {0.f,1.f},{},{}},
					Vertex_PosCol{{0,-3,-2},{1,1,1}, {.5,1.f},{},{}},
					Vertex_PosCol{{3,-3,-2},{1,1,1}, {1.f,1.f},{},{}}
				
			};
			/*std::vector<uint32_t> indices{
					3,0,1,   1,4,3,   4,1,2,
					2,5,4,   6,3,4,   4,7,6,
					7,4,5,   5,8,7
			};
			m_pTexture = Texture::LoadFromFile("Resources/uv_grid_2.png", m_pDevice);*/

			std::vector<Vertex_PosCol>vertices{};
			std::vector<uint32_t> indices{};			
			
			std::vector<Vertex_PosCol>combustionVertices{};
			std::vector<uint32_t> combustionIndices{};

			m_pTexture = Texture::LoadFromFile("Resources/vehicle_diffuse.png", m_pDevice);
			m_pTextureGloss = Texture::LoadFromFile("Resources/vehicle_gloss.png", m_pDevice);
			m_pTextureNormal = Texture::LoadFromFile("Resources/vehicle_normal.png", m_pDevice);
			m_pTextureSpecular = Texture::LoadFromFile("Resources/vehicle_specular.png", m_pDevice);
			Utils::ParseOBJ("Resources/vehicle.obj",vertices, indices );
			for (Vertex_PosCol& vert : vertices) {
				vert.Color = { 1,1,1 };
			}
			m_pMesh = new Mesh{ m_pDevice, vertices, indices };

			m_TranslationTransform = Matrix::CreateTranslation(0, 0, 0);
			m_RotationTransform = Matrix::CreateRotationZ(0);
			m_ScaleTransform = Matrix::CreateScale(1, 1, 1);
			m_pMesh->SetWorldMatrix(m_ScaleTransform * m_RotationTransform * m_TranslationTransform);
			m_pMesh->m_pEffect->SetMaps(m_pTexture, m_pTextureSpecular, m_pTextureNormal, m_pTextureGloss);

			const float screenWidth{ static_cast<float>(m_Width) };
			const float screenHeight{ static_cast<float>(m_Height) };

			m_pCamera.Initialize(45.f, Vector3{ 0.f, 0.f, -50.f });
			m_pCamera.m_AspectRatio= screenWidth / screenHeight;
			m_pCamera.m_WorldViewProjectionMatrix = m_pMesh->m_WorldMatrix * m_pCamera.m_ViewMatrix * m_pCamera.GetProjectionMatrix();

			m_pMesh->SetMatrix(&m_pCamera.m_WorldViewProjectionMatrix, &m_pMesh->m_WorldMatrix, &m_pCamera.m_Origin);

			//combustion
			m_pCombustionTexture = Texture::LoadFromFile("Resources/fireFX_diffuse.png", m_pDevice);
			Utils::ParseOBJ("Resources/firefX.obj", combustionVertices, combustionIndices);
			for (Vertex_PosCol& vert : combustionVertices) {
				vert.Color = { 1,1,1 };
			}
			m_pCombustionMesh = new Mesh{ m_pDevice, combustionVertices, combustionIndices };
			m_pCombustionMesh->SetWorldMatrix(m_ScaleTransform * m_RotationTransform * m_TranslationTransform);
			m_pCombustionMesh->m_pEffect->SetMaps(m_pCombustionTexture);
			m_pCombustionMesh->SetMatrix(&m_pCamera.m_WorldViewProjectionMatrix, &m_pCombustionMesh->m_WorldMatrix, &m_pCamera.m_Origin);
			m_pCombustionMesh->m_pEffect->ChangeEffect("FlatTechnique");


		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
	}

	Renderer::~Renderer()
	{
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		delete m_PRenderTargetView;
		delete m_pRenderTargetBuffer;
		delete m_pDepthStencilView;
		delete m_pDepthStencilBuffer;
		delete m_pSwapChain;
		delete m_pDeviceContext;
		delete m_pDevice;
		delete m_pCombustionMesh;
		delete m_pCombustionTexture;
		m_PRenderTargetView = nullptr;
		m_pRenderTargetBuffer = nullptr;
		m_pDepthStencilView = nullptr;
		m_pDepthStencilBuffer = nullptr;
		m_pSwapChain = nullptr;
		m_pDeviceContext = nullptr;
		m_pDevice = nullptr;
		m_pCombustionMesh = nullptr;
		m_pCombustionTexture = nullptr;
	}

	void Renderer::Update(const Timer* pTimer) {
		m_RotationAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		const float screenWidth{ static_cast<float>(m_Width) };
		const float screenHeight{ static_cast<float>(m_Height) };
		m_pCamera.m_AspectRatio = screenWidth / screenHeight;
		m_pCamera.Update(pTimer->GetElapsed());

		m_RotationTransform = Matrix::CreateRotationY(m_RotationAngle);
		m_pMesh->SetWorldMatrix(m_ScaleTransform * m_RotationTransform * m_TranslationTransform);
		m_pCamera.m_WorldViewProjectionMatrix = m_pMesh->m_WorldMatrix * m_pCamera.m_ViewMatrix * m_pCamera.GetProjectionMatrix();
		m_pMesh->SetMatrix(&m_pCamera.m_WorldViewProjectionMatrix, &m_pMesh->m_WorldMatrix, &m_pCamera.m_Origin);
		
		m_pCombustionMesh->SetWorldMatrix(m_ScaleTransform * m_RotationTransform * m_TranslationTransform);
		m_pCombustionMesh->SetMatrix(&m_pCamera.m_WorldViewProjectionMatrix, &m_pCombustionMesh->m_WorldMatrix, &m_pCamera.m_Origin);
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;
		//1. CLEAR RTV & DSV
		ColorRGB clearColor{ ColorRGB{ 0.f,0.f,0.3f } };
		m_pDeviceContext->ClearRenderTargetView(m_PRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		//...
		m_pMesh->Render(m_pDeviceContext);
		m_pCombustionMesh->Render(m_pDeviceContext);


		//3. PRESENT BACKBUFFER (SWAP)
		m_pSwapChain->Present(0, 0);

	}

	void Renderer::CycleTecnhique()
	{
		m_pMesh->CycleTechnique();
	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Create Device & DeviceContext
		//=====
		D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_1 };
		uint32_t createDeviceFlags{ 0 };
		#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		HRESULT result{ D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext) };

		
		if (FAILED(result))
			return result;

		//Create DXGI factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;
		//2. Create Swapchain
		//=====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		//Get the handle (HWND) from the SDL Backbuffer
		SDL_SysWMinfo sysWMinfo{};
		SDL_VERSION(&sysWMinfo.version)
			SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
		swapChainDesc.OutputWindow = sysWMinfo.info.win.window;
		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;
		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resources
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_PRenderTargetView);
		if (FAILED(result))
			return result;

		//5. Bind RTV & DSV to Ouput Merger Stage
		//=====
		m_pDeviceContext->OMSetRenderTargets(1, &m_PRenderTargetView, m_pDepthStencilView);


		//6. Set Viewport
		//=====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);
		//return S_FALSE;

		//here?
		pDxgiFactory->Release();

		return result;
	}
}
