#pragma once
#include "Camera.h"
#include "Texture.h"

struct SDL_Window;
struct SDL_Surface;
class Mesh;
//using namespace Utils;
namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		void CycleTecnhique();

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};
		float m_RotationAngle{ 0 };

		bool m_IsInitialized{ false };

		Camera m_pCamera{ };

		ID3D11Device* m_pDevice{ nullptr };
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };
		IDXGISwapChain* m_pSwapChain{ nullptr };
		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };
		ID3D11Resource* m_pRenderTargetBuffer{ nullptr };
		ID3D11RenderTargetView* m_PRenderTargetView{ nullptr };

		Mesh* m_pCombustionMesh{ nullptr };
		Texture* m_pCombustionTexture{ nullptr };

		Mesh* m_pMesh{ nullptr };
		Texture* m_pTexture{ nullptr };
		Texture* m_pTextureGloss{ nullptr };
		Texture* m_pTextureNormal{ nullptr };
		Texture* m_pTextureSpecular{ nullptr };

		Matrix m_TranslationTransform{};
		Matrix m_RotationTransform{};
		Matrix m_ScaleTransform{};

		//DIRECTX
		HRESULT InitializeDirectX();
		//...
	};
}
