//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_playerMoving = false;
	m_deviceResources->RegisterDeviceNotify(this);
	m_playerScore = 0;
	m_oppScore = 0;
	m_rotateMan = false;
	m_manStartRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), 0);
	m_manEndRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), 360);
	m_rotateStartFrame = 0;
	m_rotateEndFrame = 0;
}

Game::~Game() {
	if (m_audEngine) {
		m_audEngine->Suspend();
	}

	m_audio_ambience_loop_1.reset();
}

void Game::Initialize(HWND window, int width, int height) {

	m_input.Initialise(window);

	m_deviceResources->SetWindow(window, width, height);

	m_scoreLocation.x = width - (width / 1.5);
	m_scoreLocation.y = 10;

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.0f, 18.0f, -10.0f);
	m_Light.setDirection(-1.0f, -1.0f, -0.5f);

	m_Camera01.setPosition(Vector3(0.0f, 1.0f, 2.0f));
	m_Camera01.setRotation(Vector3(0.0f, -180.0f, 0.0f));

	m_football_position = XMFLOAT3(0, 0.32, 0);
	m_football_drag = 100;

	m_checkBallMoveFlag = false;

	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	m_audEngine = std::make_unique<AudioEngine>(eflags);
	m_audio_walk = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/walk.wav");
	m_audio_ball_hit = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/ball-kick.wav");
	m_audio_crowd_cheer = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/crowd-cheer.wav");
	m_audio_crowd_boo = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/crowd-boo.wav");
	m_audio_crowd_ambience = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/walk.wav");
	m_audio_crowd_ambience = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/crowd-ambience.wav");

	m_audio_ambience_loop_1 = m_audio_crowd_ambience->CreateInstance(SoundEffectInstance_Use3D);
	m_audio_ambience_loop_1->Play(true);

	m_audio_ambience_loop_2 = m_audio_crowd_ambience->CreateInstance(SoundEffectInstance_Use3D);
	m_audio_ambience_loop_2->Play(true);

	playerScoreString = std::to_string(m_playerScore);
	oppScoreString = std::to_string(m_oppScore);
	scoreText = std::wstring(L"Player ") +
		std::wstring(playerScoreString.begin(), playerScoreString.end()) +
		std::wstring(L" - ") +
		std::wstring(oppScoreString.begin(), oppScoreString.end()) +
		std::wstring(L" Opponent");
}

#pragma region Frame Update
void Game::Tick() {
	m_input.Update();
	m_gameInputCommands = m_input.getGameInput();

	m_timer.Tick([&]() {
		Update(m_timer);
		});

	Render();
}

Camera Game::GetCamera() {
	return m_Camera01;
}

void Game::Update(DX::StepTimer const& timer) {
	double deltaTime = timer.GetElapsedSeconds();

	if (m_gameInputCommands.rotX != 0) {
		Vector3 rotation = m_Camera01.getRotation();
		rotation.x += (-m_gameInputCommands.rotX * deltaTime * m_Camera01.getRotationSpeed());
		m_gameInputCommands.rotX = 0;
		if (rotation.x > 90) {
			rotation.x = 90;
		}
		if (rotation.x < -90) {
			rotation.x = -90;
		}
		m_Camera01.setRotation(rotation);
	}

	if (m_gameInputCommands.rotY != 0) {
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y += (-m_gameInputCommands.rotY * deltaTime * m_Camera01.getRotationSpeed());
		m_gameInputCommands.rotY = 0;
		m_Camera01.setRotation(rotation);
	}

	if (m_gameInputCommands.left) {
		Vector3 position = m_Camera01.getPosition();
		position -= (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * deltaTime * m_gameInputCommands.sprint);
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.right) {
		Vector3 position = m_Camera01.getPosition();
		position += (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * deltaTime * m_gameInputCommands.sprint);
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.forward) {
		Vector3 position = m_Camera01.getPosition();
		position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime * m_gameInputCommands.sprint);
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.back) {
		Vector3 position = m_Camera01.getPosition();
		position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * deltaTime * m_gameInputCommands.sprint);
		m_Camera01.setPosition(position);
	}

	if (m_gameInputCommands.forward || m_gameInputCommands.back || m_gameInputCommands.left || m_gameInputCommands.right) {
		if (!m_audio_walk->IsInUse()) {
			float pitch = 0.1;
			if (m_gameInputCommands.sprint > 1) {
				pitch = 0.5;
			}
			m_audio_walk->Play(0.3, pitch, 0);
		}
	}

	m_Camera01.Update();

	m_view = m_Camera01.getCameraMatrix();
	m_Light.setLookAt(m_Camera01.getPosition());
	m_world = Matrix::Identity;
	
	m_listener1.Update(m_Camera01.getPosition(), Vector3::Up, static_cast<float>(timer.GetElapsedSeconds()));
	m_listener2 = m_listener1;
	
	m_emitter1.Update(SimpleMath::Vector3(-10, 0, -7), Vector3::Up, static_cast<float>(timer.GetElapsedSeconds()));
	m_emitter2.Update(SimpleMath::Vector3(10, 0, 7), Vector3::Up, static_cast<float>(timer.GetElapsedSeconds()));

	m_audio_ambience_loop_1->Apply3D(m_listener1, m_emitter1);
	m_audio_ambience_loop_2->Apply3D(m_listener2, m_emitter2);
	/*if (m_retryAudio) {
		m_retryAudio = false;

		if (m_audEngine->Reset()) {
			if (m_audio_ambience_loop_1)
		}
	}*/

	if (m_input.Quit()) {
		ExitGame();
	}

	DirectX::SimpleMath::Vector3 curPosition = m_Camera01.getPosition();
	float dx = abs(curPosition.x - m_football_position.x);
	float dz = abs(curPosition.z - m_football_position.z);

	if (dx + dz < 0.4) {
		// Ball is in range
		DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3(m_football_position.x - curPosition.x, 0, m_football_position.z - curPosition.z);
		direction.Normalize();
		direction *= m_gameInputCommands.sprint;
		m_football_offset = direction;
		if (!m_audio_ball_hit->IsInUse()) {
			m_audio_ball_hit->Play(0.4, 0.8, 0);
		}
	}


	// Ball logic
	if (m_football_offset.x > 0) {
		m_football_offset.x -= (abs(m_football_offset.x) / m_football_drag);
		m_football_position.x += (abs(m_football_offset.x) / m_football_drag);
		m_football_rotation.x = 1;
	}

	if (m_football_offset.z > 0) {
		m_football_offset.z -= (abs(m_football_offset.z) / m_football_drag);
		m_football_position.z += (abs(m_football_offset.z) / m_football_drag);
		m_football_rotation.z = 1;
	}

	if (m_football_offset.x < 0) {
		m_football_offset.x += (abs(m_football_offset.x) / m_football_drag);
		m_football_position.x -= (abs(m_football_offset.x) / m_football_drag);
		m_football_rotation.x = -1;
	}

	if (m_football_offset.z < 0) {
		m_football_offset.z += (abs(m_football_offset.z) / m_football_drag);
		m_football_position.z -= (abs(m_football_offset.z) / m_football_drag);
		m_football_rotation.z = -1;
	}

	if ((m_football_offset.x <= 0.05 && m_football_offset.x >= -0.05) && (m_football_offset.z <= 0.05 && m_football_offset.z >= -0.05)) {
		m_football_offset.x = 0;
		m_football_offset.z = 0;
		m_football_rotation.x = 0;
		m_football_rotation.z = 0;
	}

	if (m_football_position.x >= 8.1) {
		m_football_position.x = 8;
		m_football_offset.x = 0;
		m_football_offset.z = 0;
		m_football_rotation.x = 0;
		m_football_rotation.z = 0;
	}

	if (m_football_position.x <= -8.1) {
		m_football_position.x = -8;
		m_football_offset.x = 0;
		m_football_offset.z = 0;
		m_football_rotation.x = 0;
		m_football_rotation.z = 0;
	}

	if (m_football_position.z > 15.1) {
		// Own Goal
		m_football_position.x = m_football_position.z = 0;
		m_football_offset.x = 0;
		m_football_offset.z = 0;
		m_football_rotation.x = 0;
		m_football_rotation.z = 0;
		if (!m_audio_crowd_boo->IsInUse()) {
			m_audio_crowd_boo->Play(0.5, 0, 0);
		}
		m_oppScore += 1;
		playerScoreString = std::to_string(m_playerScore);
		oppScoreString = std::to_string(m_oppScore);
		scoreText = std::wstring(L"Player ") +
			std::wstring(playerScoreString.begin(), playerScoreString.end()) +
			std::wstring(L" - ") +
			std::wstring(oppScoreString.begin(), oppScoreString.end()) +
			std::wstring(L" Opponent");
	}

	if (m_football_position.z < -15.1) {
		// Goal!!!
		m_football_position.x = m_football_position.z = 0;
		m_football_offset.x = 0;
		m_football_offset.z = 0;
		m_football_rotation.x = 0;
		m_football_rotation.z = 0;
		if (!m_audio_crowd_cheer->IsInUse()) {
			m_audio_crowd_cheer->Play(0.5, 0, 0);
		}
		m_playerScore += 1;
		playerScoreString = std::to_string(m_playerScore);
		oppScoreString = std::to_string(m_oppScore);
		scoreText = std::wstring(L"Player ") +
			std::wstring(playerScoreString.begin(), playerScoreString.end()) +
			std::wstring(L" - ") +
			std::wstring(oppScoreString.begin(), oppScoreString.end()) +
			std::wstring(L" Opponent");
	}

}
#pragma endregion

#pragma region Frame Render
void Game::Render() {
	if (m_timer.GetFrameCount() == 0) {
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullNone());

	m_SkyboxShader.EnableShader(context);
	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(180));
	m_world *= SimpleMath::Matrix::CreateTranslation(m_Camera01.getPosition().x, m_Camera01.getPosition().y, m_Camera01.getPosition().z);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureSkybox.Get());
	m_SkyboxCube.Render(context);

	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	m_BasicShader.EnableShader(context);
	m_world = SimpleMath::Matrix::Identity;
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureGrass.Get(), 150);
	m_GroundModel.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateTranslation(-3.225, 0, 15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureGoalPost.Get(), 25);
	m_GoalPost_1.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(180));
	m_world *= SimpleMath::Matrix::CreateTranslation(4, 0, -15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureGoalPost.Get(), 25);
	m_GoalPost_2.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	if (m_football_rotation.z != 0 || m_football_rotation.z != 0) {
		DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(m_football_rotation.z, 0, m_football_rotation.x), XMConvertToDegrees(m_timer.GetFrameCount() / 20.0f));
		DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
		m_world *= rotationMatrix;
	}
	m_football_translate = SimpleMath::Matrix::CreateTranslation(m_football_position);
	m_world *= m_football_translate;
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFootball.Get(), 3.5);
	m_Football.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90));
	m_world *= SimpleMath::Matrix::CreateTranslation(10, 1.1, -5);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texturePitchLine.Get());
	m_BenchCover_1.Render(context);
	m_world *= SimpleMath::Matrix::CreateTranslation(-0.05, 0, 0);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureBench.Get());
	m_BenchSeats_1.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(-90));
	m_world *= SimpleMath::Matrix::CreateTranslation(-10, 1.1, 5);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texturePitchLine.Get());
	m_BenchCover_2.Render(context);
	m_world *= SimpleMath::Matrix::CreateTranslation(0.05, 0, 0);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureBench.Get());
	m_BenchSeats_2.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.04);
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90));
	m_world *= SimpleMath::Matrix::CreateTranslation(-10, 0, -7);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureMetalBench.Get());
	m_MetalBench_1.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.04);
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(-90));
	m_world *= SimpleMath::Matrix::CreateTranslation(10, 0, 7);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureMetalBench.Get());
	m_MetalBench_2.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.35);
	m_world *= SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(90));

	if (m_rotateMan) {
		DirectX::SimpleMath::Quaternion interpolatedRotation = DirectX::SimpleMath::Quaternion::Slerp(m_manStartRotation, m_manEndRotation, m_rotateStartFrame / m_rotateEndFrame);
		m_world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(interpolatedRotation);
		m_rotateStartFrame++;
	}

	if (m_timer.GetFrameCount() == 1) {
		m_rotateMan = true;
		m_rotateStartFrame = 0; m_rotateEndFrame = 90;
	}

	m_world *= SimpleMath::Matrix::CreateTranslation(-8.5, 0.55, -0.5);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texturePitchLine.Get());
	m_Man_1.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.35);
	m_world *= SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(90));
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_timer.GetFrameCount() * 1.15));
	m_world *= SimpleMath::Matrix::CreateTranslation(-8.5, 0.55, 0.5);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFlag.Get());
	m_Man_2.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.15);
	m_world *= SimpleMath::Matrix::CreateTranslation(8, 0.8, -15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFlag.Get());
	m_Flag_1.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.15);
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(180));
	m_world *= SimpleMath::Matrix::CreateTranslation(-7.96, 0.8, -15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFlag.Get());
	m_Flag_2.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.15);
	m_world *= SimpleMath::Matrix::CreateTranslation(8, 0.8, 15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFlag.Get());
	m_Flag_3.Render(context);

	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateScale(0.15);
	m_world *= SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(180));
	m_world *= SimpleMath::Matrix::CreateTranslation(-7.96, 0.8, 15);
	m_BasicShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureFlag.Get());
	m_Flag_4.Render(context);

	m_PitchShader.EnableShader(context);
	m_world = SimpleMath::Matrix::Identity;
	m_world *= SimpleMath::Matrix::CreateTranslation(0, 0.21, 0);
	m_PitchShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texturePitchLine.Get());
	m_Pitch.Render(context);

	m_deviceResources->PIXBeginEvent(L"Draw sprite");
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), scoreText.c_str(), m_scoreLocation, Colors::Black);
	m_sprites->End();
	m_deviceResources->PIXEndEvent();
	m_deviceResources->Present();
}

void Game::Clear() {
	m_deviceResources->PIXBeginEvent(L"Clear");

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers

void Game::OnActivated() {
	m_audEngine->Resume();
}

void Game::OnDeactivated() {
	m_audEngine->Suspend();
}

void Game::OnSuspending() {
	m_audEngine->Suspend();
}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
	m_audEngine->Resume();
}

void Game::OnWindowMoved() {
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;
	m_scoreLocation.x = width - (width / 1.5);
	m_scoreLocation.y = 10;
	CreateWindowSizeDependentResources();
}

void Game::GetDefaultSize(int& width, int& height) const {
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
void Game::CreateDeviceDependentResources() {
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	m_states = std::make_unique<CommonStates>(device);
	m_fxFactory = std::make_unique<EffectFactory>(device);
	m_sprites = std::make_unique<SpriteBatch>(context);
	m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	m_GroundModel.InitializeBox(device, 50.0f, 0.5f, 50.0f);

	m_Pitch.InitializeBox(device, 16.0f, 0.1f, 30.0f);

	m_Football.InitializeSphere(device, 0.075);

	m_GoalPost_1.InitializeModel(device, "Models/football_goalpost.obj");
	m_GoalPost_2.InitializeModel(device, "Models/football_goalpost.obj");

	m_BenchCover_1.InitializeModel(device, "Models/bench_cover.obj");
	m_BenchCover_2.InitializeModel(device, "Models/bench_cover.obj");

	m_BenchSeats_1.InitializeModel(device, "Models/bench_seats.obj");
	m_BenchSeats_2.InitializeModel(device, "Models/bench_seats.obj");

	m_MetalBench_1.InitializeModel(device, "Models/metal_bench.obj");
	m_MetalBench_2.InitializeModel(device, "Models/metal_bench.obj");

	m_Man_1.InitializeModel(device, "Models/man.obj");
	m_Man_2.InitializeModel(device, "Models/man.obj");

	m_Flag_1.InitializeModel(device, "Models/flag.obj");
	m_Flag_2.InitializeModel(device, "Models/flag.obj");
	m_Flag_3.InitializeModel(device, "Models/flag.obj");
	m_Flag_4.InitializeModel(device, "Models/flag.obj");

	m_SkyboxCube.InitializeModel(device, "Models/skybox_cube.obj");

	m_BasicShader.InitStandard(device, L"basic_vs.cso", L"basic_ps.cso");
	m_PitchShader.InitStandard(device, L"pitch_boxes_vs.cso", L"pitch_boxes_ps.cso");
	m_SkyboxShader.InitStandard(device, L"skybox_vs.cso", L"skybox_ps.cso");

	CreateDDSTextureFromFile(device, L"Textures/grass_pitch.dds", nullptr, m_textureGrass.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/goal_post.dds", nullptr, m_textureGoalPost.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/white_line.dds", nullptr, m_texturePitchLine.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/football.dds", nullptr, m_textureFootball.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/bench.dds", nullptr, m_textureBench.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/metal_bench.dds", nullptr, m_textureMetalBench.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/leather_red.dds", nullptr, m_textureFlag.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/skybox.dds", nullptr, m_textureSkybox.ReleaseAndGetAddressOf());
	m_world = Matrix::Identity;
}

void Game::CreateWindowSizeDependentResources() {
	auto size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f) {
		fovAngleY *= 2.0f;
	}

	m_projection = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);
}


void Game::OnDeviceLost() {
	m_states.reset();
	m_fxFactory.reset();
	m_sprites.reset();
	m_font.reset();
	m_batch.reset();
	m_audEngine.reset();
	m_audio_crowd_ambience.reset();
	m_audio_ambience_loop_1.reset();
	m_textureGrass.Reset();
	m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored() {
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
