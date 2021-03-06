#include "CCamera.h"

int CCamera::Init()
{
#pragma region Get Instances of Time, Input and Window
	p_time = &p_time->GetInstance();
	m_input = &m_input->GetInstance();
	p_window = &p_window->GetInstance();
#pragma endregion

	//Update variables, viewMatrix and projectionMatrix
	updateCameraVectors();

	return 0;
}

void CCamera::Update()
{
#pragma region //Input
	float tmpSpeed = 0.01f;
	if ((tmpSpeed *= m_input->getMouseState().lZ) != 0)
		m_camSpeed = (m_camSpeed + tmpSpeed < 0.1f) ? 0.1f : m_camSpeed + tmpSpeed;
	float movementspeed = p_time->GetDeltaTime() * m_camSpeed;							//mouseWheel input
	if (m_input->getKeyboardState(DIK_LSHIFT) & 0x80) movementspeed *= 2;				//movementspeed up
	if (m_input->getKeyboardState(DIK_LCONTROL) & 0x80) movementspeed *= 0.2f;			//movementspeed down
	if (m_input->getKeyboardState(DIK_A) & 0x80) m_position += movementspeed * m_right; //horizontal keyboard input
	if (m_input->getKeyboardState(DIK_D) & 0x80) m_position -= movementspeed * m_right; //horizontal keyboard input
	if (m_input->getKeyboardState(DIK_W) & 0x80) m_position += movementspeed * m_front; //vertical keyboard input
	if (m_input->getKeyboardState(DIK_S) & 0x80) m_position -= movementspeed * m_front; //vertical keyboard input
	if (m_input->getKeyboardState(DIK_E) & 0x80) m_position += movementspeed * m_up;	//height keyboard input
	if (m_input->getKeyboardState(DIK_Q) & 0x80) m_position -= movementspeed * m_up;	//height keyboard input
	if (m_input->getKeyboardState(DIK_E) & 0x80 && m_input->getKeyboardState(DIK_W) & 0x80) //horizontal and height input
		m_position += movementspeed * XMVector3Cross(m_right, m_front); //fix movement
	if (m_input->getKeyboardState(DIK_Q) & 0x80 && m_input->getKeyboardState(DIK_S) & 0x80) //horizontal and height input
		m_position -= movementspeed * XMVector3Cross(m_right, m_front); //fix movement

	if (m_input->getMouseState().rgbButtons[1] != 0)
	{
		//m_window->showCursor(false);
		m_mouseRot.x -= m_input->getMouseState().lX * 0.2f; //horizontal mouse input
		m_mouseRot.y -= m_input->getMouseState().lY * 0.2f; //vertical mouse input
	}

	//Clamp vertical rotation to 80 degree each
	m_mouseRot.y =
		(m_mouseRot.y > 80) ? 80 :
		(m_mouseRot.y < -80) ? -80 :
		m_mouseRot.y;
#pragma endregion

	//Update variables, viewMatrix and projectionMatrix
	updateCameraVectors();
}

void CCamera::Release()
{
	p_window->Release();
	p_window = nullptr;
	p_time->Release();
	p_time = nullptr;
	m_input->Release();
	m_input = nullptr;
}

void CCamera::updateCameraVectors()
{
	//https://github.com/TKscoot/Ivy/blob/master/projects/Ivy/source/scene/Camera.cpp

	//360 degree horizontal rotation
	XMFLOAT3 front = {
		front.x = cos(XMConvertToRadians(m_mouseRot.x)) * cos(XMConvertToRadians(m_mouseRot.y)),
		front.y = sin(XMConvertToRadians(m_mouseRot.y)),
		front.z = sin(XMConvertToRadians(m_mouseRot.x)) * cos(XMConvertToRadians(m_mouseRot.y)) };
	//update frontVector
	m_front = XMVector3Normalize(XMVectorSet(front.x, front.y, front.z, 0));
	//update rightVector
	m_right = XMVector3Normalize(XMVector3Cross(m_front, m_up));

	//update viewMatrix
	m_view = XMMatrixLookAtLH(m_position, m_position + m_front, m_up);
	//Update projectionMatrix
	m_projection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(80),
		GetAspectRatio(),
		0.1f, 1000);
}
