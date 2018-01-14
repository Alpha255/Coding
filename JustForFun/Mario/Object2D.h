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

	struct Area
	{
		uint32_t Top = 0U;   /// Coordinate in window
		uint32_t Left = 0U;

		uint32_t Width = 0U;
		uint32_t Height = 0U;

		uint32_t ImageX = 0U;
		uint32_t ImageY = 0U;
	};

	Object2D() = default;
	virtual ~Object2D() = default;

	Object2D(eType type);

	inline virtual const class Image *GetImage() const
	{
		assert(m_Image);
		return m_Image;
	}

	inline virtual const Area &GetArea() const
	{
		return m_Area;
	}

	inline void Move(eOrientation orientation, eMotion motion)
	{
		m_Orientation = orientation;
		m_Motion = motion;
	}

	inline void UpdateArea(uint32_t top, uint32_t left, uint32_t width, uint32_t height, uint32_t imageX, uint32_t imageY)
	{
		m_Area.Top = top;
		m_Area.Left = left;
		m_Area.Width = width;
		m_Area.Height = height;
		m_Area.ImageX = imageX;
		m_Area.ImageY = imageY;
	}

	void Update(float elapseTime);

	bool IsCollision(const Object2D &object);
	bool IsCollision(const class Map &map);
protected:
	eType m_Type = eTypeCount;
	eOrientation m_Orientation = eLeft;
	eMotion m_Motion = eNone;

	Area m_Area;

	Vec2 m_Velocity = { 0.0f, 0.0f };
	Vec2 m_Acceleration = { 0.0f, 0.0f };

	const class Image *m_Image = nullptr;
	typedef Object2D Base;
private:
};