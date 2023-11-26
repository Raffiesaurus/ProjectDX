//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include <fstream>

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify {
public:

	Game() noexcept(false);
	~Game();

	void Initialize(HWND window, int width, int height);

	void Tick();

	Camera GetCamera();

	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);
	void GetDefaultSize(int& width, int& height) const;

private:

	struct MatrixBufferType {
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	void Update(DX::StepTimer const& timer);
	void Render();
	void Clear();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	DX::StepTimer                           m_timer;

	Input									m_input;
	InputCommands							m_gameInputCommands;

	std::unique_ptr<DirectX::CommonStates>                                  m_states;
	std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
	std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
	std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
	std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	Light																	m_Light;

	Camera																	m_Camera01;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSkyBox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrass;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>						m_cubemap;

	Shader																	m_LightShader;
	Shader																	m_GroundShader;
	Shader																	m_GrassShader;
	Shader																	m_SkyBoxShader;

	ModelClass																m_GroundModel;
	ModelClass																m_GrassModel;
	ModelClass																m_SkyboxModel;
	ModelClass																m_ParkModel;

	DirectX::SimpleMath::Matrix                                             m_world;
	DirectX::SimpleMath::Matrix                                             m_view;
	DirectX::SimpleMath::Matrix                                             m_projection;

	std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
	std::unique_ptr<DirectX::SoundEffect>                                   m_walk;
	std::unique_ptr<DirectX::SoundEffect>									m_bgm;
	std::unique_ptr<DirectX::SoundEffectInstance>							m_bgmLoop;
	bool																	m_retryAudio;
	bool																	playerMoving;
};