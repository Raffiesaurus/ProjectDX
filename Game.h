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

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify {
public:

	Game() noexcept(false);
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	Camera GetCamera();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);
	void NewAudioDevice();
	// Properties
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

	// Device resources.
	std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

	// DirectXTK objects.
	std::unique_ptr<DirectX::CommonStates>                                  m_states;
	std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
	std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
	std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
	std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
	Camera																	m_Camera01;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSkyBox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrass;

	//Shaders
	Shader																	m_BasicShaderPair;
	Shader																	m_GrassShader;
	Shader																	m_SkyBoxShader;
	ModelClass																m_BasicSphere;
	ModelClass																m_GroundModel;
	ModelClass																m_GrassModel;

	DirectX::SimpleMath::Matrix                                             m_world;
	DirectX::SimpleMath::Matrix                                             m_view;
	DirectX::SimpleMath::Matrix                                             m_projection;

	std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
	std::unique_ptr<DirectX::SoundEffect>                                   m_walk;
	std::unique_ptr<DirectX::SoundEffect>									m_bgm;
	std::unique_ptr<DirectX::SoundEffectInstance>							m_bgmLoop;
	bool																	m_retryAudio;

	bool playerMoving;
};