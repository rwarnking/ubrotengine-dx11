///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: camera.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/view_matrix_handler.h"


//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

void ViewMatrixHandler::RenderViewMatrix(
	float pox_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z
)
{
	Render(m_view_matrix, pox_x, pos_y, pos_z, rot_x, rot_y, rot_z);
}


void ViewMatrixHandler::RenderBaseViewMatrix(
	float pox_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z
)
{
	Render(m_base_view_matrix, pox_x, pos_y, pos_z, rot_x, rot_y, rot_z);
}


void ViewMatrixHandler::RenderReflectionMatrix(
	float pox_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z,
	float height
)
{
	// Initialize the position vector and convert it to a XMVECTOR and make the y value
	// correspond to the height of the reflecting object
	const float H_MUL = 2.0F;
	Render(m_reflection_matrix, pox_x, pos_y + height * H_MUL, pos_z, rot_x, rot_y, rot_z);
}


void ViewMatrixHandler::Render(
	DirectX::XMMATRIX& result_matrix,
	float pox_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z
)
{
	using DirectX::XMFLOAT3;
	using DirectX::XMVECTOR;
	using DirectX::XMMATRIX;
	using DirectX::XMMatrixRotationRollPitchYaw;
	using DirectX::XMVectorAdd;
	using DirectX::XMMatrixLookAtLH;

	// Initialize the up vector and convert it to a XMVECTOR
	XMFLOAT3 up(0.0F, 1.0F, 0.0F);
	XMVECTOR upVector = XMLoadFloat3(&up);

	// Initialize the position vector and convert it to a XMVECTOR
	XMFLOAT3 position(pox_x, pos_y, pos_z);
	XMVECTOR positionVector = XMLoadFloat3(&position);

	// Initialize the look-at vector and convert it to a XMVECTOR
	XMFLOAT3 lookAt(0.0F, 0.0F, 1.0F);
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	// Create a rotation matrix around each axis (yaw, pitch and roll) whose content is in radians
	const float scale_angle = 0.0174532925F;
	float pitch = rot_x * scale_angle;
	float yaw = rot_y * scale_angle;
	float roll = rot_z * scale_angle;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the look-at and up vectors using the rotation matrix
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	//upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAt.x = rot_x;
	lookAt.y = rot_y;
	lookAt.z = rot_z;

	lookAtVector = XMLoadFloat3(&lookAt);

	// Translate the look-at vector to the current camera position
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Create the view matrix from the three vectors
	result_matrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


auto ViewMatrixHandler::GetViewMatrix() -> DirectX::XMMATRIX&
{
	return m_view_matrix;
}


auto ViewMatrixHandler::GetReflectionMatrix() -> DirectX::XMMATRIX&
{
	return m_reflection_matrix;
}


auto ViewMatrixHandler::GetBaseViewMatrix() -> DirectX::XMMATRIX&
{
	return m_base_view_matrix;
}

} // namespace graphics
