//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {

}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height) {

	m_input.Initialise(window);

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 1.0f, 4.0f));
	m_Camera01.setRotation(Vector3(0.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick() {
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

	//Update all game objects
	m_timer.Tick([&]() {
		Update(m_timer);
		});

	//Render all game content. 
	Render();

}

Camera Game::GetCamera() {
	return m_Camera01;
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	double deltaTime = timer.GetElapsedSeconds();
	//note that currently.  Delta-time is not considered in the game object movement. 

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

	m_Camera01.Update();

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;

	if (m_input.Quit()) {
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0) {
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Draw Text to the screen
  //  m_deviceResources->PIXBeginEvent(L"Draw sprite");
  //  m_sprites->Begin();
		//m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
  //  m_sprites->End();
  //  m_deviceResources->PIXEndEvent();

	//Set Rendering states. 
	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullNone());

	m_GrassShader.EnableShader(context);
	m_GrassShader.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_textureGrass.Get());
	m_GroundModel.Render(context);

	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear() {
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {
}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
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

// Properties
void Game::GetDefaultSize(int& width, int& height) const {
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources() {
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	m_states = std::make_unique<CommonStates>(device);
	m_fxFactory = std::make_unique<EffectFactory>(device);
	m_sprites = std::make_unique<SpriteBatch>(context);
	m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
	m_BasicSphere.InitializeSphere(device);

	m_GroundModel.InitializeBox(device, 500.0f, 0.5f, 500.0f);	//box includes dimensions

	m_GrassModel.InitializeModel(device, "Models/tall_grass.obj");

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");
	m_SkyBoxShader.InitStandard(device, L"skybox_vs.cso", L"skybox_ps.cso");
	m_GrassShader.InitStandard(device, L"grass_vs.cso", L"grass_ps.cso");
	//load Textures
	CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"EvilDrone_Diff.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Textures/grass.dds", nullptr, m_textureGrass.ReleaseAndGetAddressOf());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources() {
	auto size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f) {
		fovAngleY *= 2.0f;
	}

	// This sample makes use of a right-handed coordinate system using row-major matrices.
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
