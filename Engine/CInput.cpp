#include "CInput.h"

int CInput::Init()
{
#pragma region //get instance of window
	m_window = &m_window->getInstance();
#pragma endregion

#pragma region //Create Input
	DirectInput8Create(m_window->getHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
#pragma endregion

#pragma region // Create Keyboard and Mouse Inputdevices
	m_directInput->CreateDevice(GUID_SysKeyboard, &m_diKeyboard, NULL);
	m_directInput->CreateDevice(GUID_SysMouse, &m_diMouse, NULL);
#pragma endregion

#pragma region // Setup Keyboard
	m_diKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_diKeyboard->SetCooperativeLevel(m_window->getHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
#pragma endregion

#pragma region //Setup Mouse
	m_diMouse->SetDataFormat(&c_dfDIMouse);
	m_diMouse->SetCooperativeLevel(m_window->getHWND(), DISCL_EXCLUSIVE | DISCL_NOWINKEY);
#pragma endregion

	return 0;
}

void CInput::Update()
{
	m_diMouse->Acquire();
	m_diMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState);
	m_diKeyboard->Acquire();
	m_diKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	return;
}

void CInput::Release()
{
	m_window->Release();
	m_window = nullptr;
}
