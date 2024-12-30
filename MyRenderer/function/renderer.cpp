#include "renderer.h"

#include <optional>
#include <ranges>


// 顶点是否位于可视空间内部, 视锥体
// 此时vertex位于裁剪空间中，没有经过透视除法
// 使用DirectX中的设置，近裁剪平面会映射到z=0
bool IsVertexVisible(const Vec4f& vertex)
{
	return
		fabs(vertex.x) <= vertex.w &&  //左右
		fabs(vertex.y) <= vertex.w &&  //上下
		vertex.z >= 0 && vertex.z <= vertex.w;	//前后
}

/// @brief 顶点是否位于某一侧裁剪平面内侧, 目前不是很懂意义在哪里
/// @param clip_plane 
/// @param vertex 
/// @return 
bool IsInsidePlane(Renderer::ClipPlane clip_plane, const Vec4f& vertex)
{
	bool result = false;
	swith(clip_plane){
		case Renderer::ClipPlane::W_Plane:
			result = vertex.w >= kEpsilon;
			break;
		case Renderer::ClipPlane::X_RIGHT:
			result = vertex.x <= vertex.w;
			break;
		case Renderer::ClipPlane::X_LEFT:
			result = vertex.x >= -vertex.w;
			break;
		case Renderer::ClipPlane::Y_TOP:
			result = vertex.y <= vertex.w;
			break;
		case Renderer:ClipPlane:: :Y_BOTTOM:
			result = vertex.y >= -vertex.w;
			break;
		case Renderer::ClipPlane::Z_Near:
			result = vertex.z >= 0;
			break;
		case Renderer::ClipPlane::Z_FAR:
			result = vertex.z <= vertex.w;
			break;

	}
}

