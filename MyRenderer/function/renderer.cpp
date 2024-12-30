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
		// 防止进行透视除法时出现数值溢出 就是说w可能为0
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
		default: 
			break;
	}
	return result;
}

// 获取裁剪空间下，两点被裁剪平面分割的比例系数，用于生成与裁剪平面的交点
float GetIntersectRatio(Renderer::ClipPlane clip_plane, const Vec4f& pre_vertex, const Vec4f& cur_vertex)
{
	float intersect_ratio = 1.0f;
	switch (clip_plane)
	{
	case Renderer::ClipPlane::X_RIGHT:
		intersect_ratio = (pre_vertex.w - pre_vertex.x) /                   // 可以除w理解
			((pre_vertex.w - pre_vertex.x) - (cur_vertex.w - cur_vertex.x));
		break;
	case Renderer::ClipPlane::X_LEFT:
		intersect_ratio = (pre_vertex.w + pre_vertex.x) /
			((pre_vertex.w + pre_vertex.x) - (cur_vertex.w + cur_vertex.x));
		break;
	case Renderer::ClipPlane::Y_TOP:
		intersect_ratio = (pre_vertex.w - pre_vertex.y) /
			((pre_vertex.w - pre_vertex.y) - (cur_vertex.w - cur_vertex.y));
		break;
	case Renderer::ClipPlane::Y_BOTTOM:
		intersect_ratio = (pre_vertex.w + pre_vertex.y) /
			((pre_vertex.w + pre_vertex.y) - (cur_vertex.w + cur_vertex.y));
		break;
	case Renderer::ClipPlane::Z_Near:
		// 由于使用DirectX中的投影矩阵，将near plane映射到z=0上，因此比例系数的计算公式有所改变
		intersect_ratio = (pre_vertex.z / pre_vertex.w) /
			(pre_vertex.z / pre_vertex.w - cur_vertex.z / cur_vertex.w);
		break;
	case Renderer::ClipPlane::Z_FAR:
		intersect_ratio = (pre_vertex.w + pre_vertex.z) /
			((pre_vertex.w + pre_vertex.z) - (cur_vertex.w + cur_vertex.z));
		break;
	default:;
	}

	return  intersect_ratio;
}

// 详见https://fabiensanglard.net/polygon_codec/
int Renderer::ClipWithPlane(ClipPlane clip_plane, Vertex vertex[3])
{
	int out_vertex_count = 0;
	constexpr int vertex_count = 3;
	bool isInside[vertex_count] = {false};


	for (int i = 0; i < vertex_count; i++)
	{
		const int cur_index = i;
		const int pre_index = (i - 1 + vertex_count) % vertex_count;

		Vec4f cur_vertex = vertex[cur_index].position;
		Vec4f pre_vertex = vertex[pre_index].position;

		// 由于操作简单, 在GPU上运行时,直接调用函数反而比存储结果要快
		const bool is_cur_inside = IsInsidePlane(clip_plane, cur_vertex);
		const bool is_pre_inside = IsInsidePlane(clip_plane, pre_vertex);

		// 一点在内，一点在外, 裁剪,即插值除交点
		if (is_cur_inside ^ is_pre_inside) // 异或 一个在内一个在外
		{
			const float ratio = GetIntersectRatio(clip_plane, pre_vertex, cur_vertex);
			Vertex& new_vertex = VertexLerp(vertex[pre_index], vertex[cur_index], ratio);
			clip_vertex_[out_vertex_count] = &new_vertex;
			out_vertex_count++;
		}
		/// 当前点在内部,
		if (is_cur_inside)
		{
			clip_vertex_[out_vertex_count] = &vertex[cur_index];
			out_vertex_count++;
		}
	}

	return  out_vertex_count;
}
