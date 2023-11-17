#include "pch.h"
#include "Input.h"

Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	m_quitApp = false;

	m_GameInput.forward = false;
	m_GameInput.back = false;
	m_GameInput.right = false;
	m_GameInput.left = false;
	m_GameInput.rotRight = false;
	m_GameInput.rotLeft = false;
	m_GameInput.rotUp = false;
	m_GameInput.rotDown = false;
	m_GameInput.rotX = 0;
	m_GameInput.rotY = 0;
	m_GameInput.sprint = 1;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	if (m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED) {
		m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	}

	if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED) {
		m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}

	if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE) {

		//Movement 

		//A key
		if (kb.A)	m_GameInput.left = true;
		else		m_GameInput.left = false;

		//D key
		if (kb.D)	m_GameInput.right = true;
		else		m_GameInput.right = false;

		//W key
		if (kb.W)	m_GameInput.forward = true;
		else		m_GameInput.forward = false;

		//S key
		if (kb.S)	m_GameInput.back = true;
		else		m_GameInput.back = false;

		if (kb.LeftShift || kb.RightShift)	m_GameInput.sprint = 2;
		else		m_GameInput.sprint = 1;

		// Camera Control
		m_GameInput.rotX = float(mouse.y);
		m_GameInput.rotY = float(mouse.x);
	}
	else {
		Input::DisableInput();
	}
}

void Input::DisableInput() {
	m_GameInput.forward = false;
	m_GameInput.back = false;
	m_GameInput.right = false;
	m_GameInput.left = false;
	m_GameInput.rotRight = false;
	m_GameInput.rotLeft = false;
	m_GameInput.rotUp = false;
	m_GameInput.rotDown = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
