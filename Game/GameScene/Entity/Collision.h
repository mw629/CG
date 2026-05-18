#pragma once
#include <Engine.h>

/// <summary>
/// AABB同士の当たり判定を行うユーティリティクラス
/// </summary>
class Collision
{
public:

	/// <summary>
	/// AABB同士の交差判定
	/// </summary>
	/// <param name="a">AABB A</param>
	/// <param name="b">AABB B</param>
	/// <returns>交差していたらtrue</returns>
	static bool CheckAABB(const AABB& a, const AABB& b);

	/// <summary>
	/// TransformとサイズからAABBを生成する
	/// </summary>
	/// <param name="transform">トランスフォーム</param>
	/// <param name="width">横幅</param>
	/// <param name="height">高さ</param>
	/// <param name="depth">奥行き</param>
	/// <returns>生成されたAABB</returns>
	static AABB MakeAABB(const Transform& transform, float width, float height, float depth);
};
