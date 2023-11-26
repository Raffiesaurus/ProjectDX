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
	playerMoving = false;
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	if (m_audEngine) {
		m_audEngine->Suspend();
	}

	m_bgmLoop.reset();
}

void Game::Initialize(HWND window, int width, int height) {

	m_input.Initialise(window);

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	m_Camera01.setPosition(Vector3(0.0f, 1.0f, 4.0f));
	m_Camera01.setRotation(Vector3(0.0f, -180.0f, 0.0f));

	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	m_audEngine = std::make_unique<AudioEngine>(eflags);
	m_walk = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/walk.wav");
	m_bgm = std::make_unique<DirectX::SoundEffect>(m_audEngine.get(), L"Audio/bgm.wav");
	m_bgmLoop = m_bgm->CreateInstance();
	m_bgmLoop->Play(true);
	m_bgmLoop->SetVolume(0.1);
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
		if (!m_walk->IsInUse()) {
			float pitch = 0.1;
			if (m_gameInputCommands.sprint > 1) {
				pitch = 0.5;
			}
			m_walk->Play(0.5, pitch, 0);
		}
	}

	m_Camera01.Update();

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

	if (m_retryAudio) {
		m_retryAudio = false;

		if (m_audEngine->Reset()) {
			if (m_bgmLoop)
				m_bgmLoop->Play(true);
		}
	}

	if (m_input.Quit()) {
		ExitGame();
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


	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());

	m_GroundShader.EnableShader(context);
	m_GroundShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureGrass.Get());
	m_GroundModel.Render(context);

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

	m_GroundModel.InitializeBox(device, 100.0f, 0.5f, 100.0f);

	m_GrassModel.InitializeModel(device, "Models/tall_grass.obj");
	m_SkyboxModel.InitializeBox(device, 1, 1, 1);

	m_ParkModel.InitializeModel(device, "Models/OBJ.obj");
	m_LightShader.InitStandard(device, L"light_vs.cso", L"light_ps.cso");
	m_SkyBoxShader.InitStandard(device, L"skybox_vs.cso", L"skybox_ps.cso");
	m_GrassShader.InitStandard(device, L"grass_vs.cso", L"grass_ps.cso");
	m_GroundShader.InitStandard(device, L"ground_vs.cso", L"ground_ps.cso");

	CreateDDSTextureFromFile(device, L"Textures/cubemap.dds", nullptr, m_cubemap.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/grass.dds", nullptr, m_textureGrass.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/skybox.dds", nullptr, m_textureSkyBox.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/ground.dds", nullptr, m_textureGround.ReleaseAndGetAddressOf());
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
	m_testmodel.reset();
	m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored() {
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
