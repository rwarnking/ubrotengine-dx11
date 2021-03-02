///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: view_matrix_handler.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <cmath>
#include <directxmath.h>
#include <fstream>


namespace graphics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: ViewMatrixHandler
/// The camera class represents a virtual camera with a position and rotation. This camera
/// can be used to create different view matrices for rendering.
///////////////////////////////////////////////////////////////////////////////////////////////////
class ViewMatrixHandler
{
public:
	ViewMatrixHandler() = default;
	ViewMatrixHandler(const ViewMatrixHandler& other) = delete;
	ViewMatrixHandler(ViewMatrixHandler&& other) noexcept = delete;
	auto operator=(const ViewMatrixHandler& other) -> ViewMatrixHandler = delete;
	auto operator=(ViewMatrixHandler&& other) -> ViewMatrixHandler & = delete;
	~ViewMatrixHandler() = default;

	/**
	 * Computes the view matrix using \c XMMatrixLookAtLH, the camera position and camera
	 * rotation.This function should be called every frame to ensure correctness.
	 */
	void RenderViewMatrix(
		float pox_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z
	);

	/**
	 * Computes the reflection matrix using \c XMMatrixLookAtLH, the camera position and camera
	 * rotation. This function should be called every frame to ensure correctness.
	 * 
	 * param[height] the height at which the reflecting objects are positioned
	 */
	void RenderReflectionMatrix(
		float pox_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float height
	);

	/**
	 * Computes the base view matrix using \c XMMatrixLookAtLH, the camera position and camera
	 * rotation. This function should be called once at the beginning of the application and not
	 * updated again afterwards to ensure correctness (e.g. for GUI-rendering).
	 */
	void RenderBaseViewMatrix(
		float pox_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z
	);

	/**
	 * Returns the computed view matrix.
	 * @return view_matrix
	 */
	auto GetViewMatrix() -> DirectX::XMMATRIX&;

	/**
	 * Returns the computed reflection matrix.
	 * @return reflection_matrix
	 */
	auto GetReflectionMatrix() -> DirectX::XMMATRIX&;

	/**
	 * Returns the computed base view matrix.
	 * @return baseViewMatrix
	 */
	auto GetBaseViewMatrix() -> DirectX::XMMATRIX&;

private:
	static void Render(
		DirectX::XMMATRIX& result_matrix,
		float pox_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z
	);

// private:
	DirectX::XMMATRIX m_view_matrix;
	DirectX::XMMATRIX m_base_view_matrix;
	DirectX::XMMATRIX m_reflection_matrix;
};

} // namespace graphics