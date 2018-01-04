#pragma once

#include "D3DMath.h"

class Object2D
{
public:
	enum eType
	{
		eBrickDebris,
		eBullet,
		eCoin,
		eExplode,
		eFlower,
		eMario,
		eMonster,
		eMushroom,
		eTile,
		eTurtle,
		eTypeCount
	};

	enum eOrientation
	{
		eLeft,
		eRight
	};

	enum eMotion
	{
		eNone,
		eStatic,
		eWalk,
		eCrouch,
		eJump,
		eStop,
		eSlide,
		eDead,
		eUpsidedown
	};

	Object2D() = default;
	~Object2D() = default;

	inline virtual const class Image *GetImage() const
	{
		assert(m_Image);
		return m_Image;
	}

	inline void Move(eOrientation orientation, eMotion motion)
	{
		m_Orientation = orientation;
		m_Motion = motion;
	}

	void Update(float elapseTime);

	bool IsCollision(const Object2D &object);
	bool IsCollision(const class Map &map);
protected:
	eType m_Type = eTypeCount;
	eOrientation m_Orientation = eLeft;
	eMotion m_Motion = eNone;

	struct Area
	{
		uint32_t Top = 0U;
		uint32_t Left = 0U;
		uint32_t Width = 0U;
		uint32_t Height = 0U;
	}m_Area;

	Vec2 m_Velocity = {};
	Vec2 m_Accelerate = {};

	const class Image *m_Image = nullptr;
private:
};