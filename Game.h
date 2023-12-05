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

	int																		m_playerScore;
	int																		m_oppScore;

	float																	m_rotateStartFrame;
	float																	m_rotateEndFrame;
	float																	m_football_drag;
	float																	m_football_travel_time;

	bool																	m_retryAudio;
	bool																	m_playerMoving;
	bool																	m_checkBallMoveFlag;
	bool																	m_rotateMan;

	std::string																playerScoreString;
	std::string																oppScoreString;
	std::wstring															scoreText;

	XMFLOAT2																m_scoreLocation;

	std::unique_ptr<DX::DeviceResources>									m_deviceResources;

	DX::StepTimer															m_timer;

	Input																	m_input;

	InputCommands															m_gameInputCommands;

	std::unique_ptr<DirectX::CommonStates>                                  m_states;
	std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
	std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
	std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
	std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;

	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;

	Light																	m_Light;

	Camera																	m_Camera01;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGrass;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureGoalPost;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texturePitchLine;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureFootball;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureBench;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureMetalBench;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureFlag;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_textureSkybox;

	Shader																	m_BasicShader;
	Shader																	m_PitchShader;
	Shader																	m_SkyboxShader;

	ModelClass																m_GroundModel;
	ModelClass																m_Pitch;
	ModelClass																m_Football;
	ModelClass																m_GoalPost_1;
	ModelClass																m_GoalPost_2;
	ModelClass																m_BenchCover_1;
	ModelClass																m_BenchCover_2;
	ModelClass																m_BenchSeats_1;
	ModelClass																m_BenchSeats_2;
	ModelClass																m_MetalBench_1;
	ModelClass																m_MetalBench_2;
	ModelClass																m_Man_1;
	ModelClass																m_Man_2;
	ModelClass																m_SkyboxCube;
	ModelClass																m_Flag_1;
	ModelClass																m_Flag_2;
	ModelClass																m_Flag_3;
	ModelClass																m_Flag_4;

	DirectX::SimpleMath::Matrix                                             m_world;
	DirectX::SimpleMath::Matrix                                             m_view;
	DirectX::SimpleMath::Matrix                                             m_projection;
	DirectX::SimpleMath::Matrix												m_football_translate;

	DirectX::SimpleMath::Vector3											m_football_position;
	DirectX::SimpleMath::Vector3											m_football_rotation;
	DirectX::SimpleMath::Vector3                                            m_football_offset;

	std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
	std::unique_ptr<DirectX::SoundEffect>                                   m_audio_walk;
	std::unique_ptr<DirectX::SoundEffect>                                   m_audio_ball_hit;
	std::unique_ptr<DirectX::SoundEffect>                                   m_audio_crowd_cheer;
	std::unique_ptr<DirectX::SoundEffect>                                   m_audio_crowd_boo;
	std::unique_ptr<DirectX::SoundEffect>									m_audio_crowd_ambience;
	std::unique_ptr<DirectX::SoundEffectInstance>							m_audio_ambience_loop_1;
	std::unique_ptr<DirectX::SoundEffectInstance>							m_audio_ambience_loop_2;

	DirectX::AudioListener													m_listener1;
	DirectX::AudioListener													m_listener2;
	DirectX::AudioEmitter													m_emitter1;
	DirectX::AudioEmitter													m_emitter2;


	DirectX::SimpleMath::Quaternion											m_manStartRotation;
	DirectX::SimpleMath::Quaternion											m_manEndRotation;

};