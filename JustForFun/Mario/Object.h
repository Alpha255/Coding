#pragma once

#include "D3DMath.h"

class Object2D
{
public:
	enum eType
	{
		eMario,
		eMonster,
		eTurtle,
		eTurtleFly,
		eCoin,
		eMushroom,
		eFlower,
		eBullet,
		eBrick,
		eSpark,
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

	::RECT m_ClientRect;

	Vec2 m_Velocity = {};
	Vec2 m_Accelerate = {};

	const class Image *m_Image = nullptr;
private:
};