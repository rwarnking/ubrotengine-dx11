///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: model_factory.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "vertex_types.h"


namespace io
{
namespace gv = graphics::vertices;
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: ModelFactory
///////////////////////////////////////////////////////////////////////////////////////////////////
class ModelFactory
{
public:
	ModelFactory() = delete;

	template <class T>
	static void GenerateTriangle(gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices);

	template <class T>
	static void GeneratePlane(gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices);

	template <class T>
	static void GenerateCube(gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices);

	template <class T>
	static void GenerateSphere(gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices);

	template <class T>
	static void GenerateThorus(gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices);
};

} // namespace io
