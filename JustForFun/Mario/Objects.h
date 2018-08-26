#pragma once

#include "Object2D.h"

class Brick : public Object2D
{
public:
	Brick(uint32_t left, uint32_t top)
		: Base(eCoin, left, top)
	{
		m_Area.ObjectWidth = 16U;
		m_Area.ObjectHeight = 16U;
	}
	~Brick() = default;

	virtual void Update(float) override;
protected:
private:
};

class Coin : public Object2D
{
public:
	Coin(uint32_t left, uint32_t top)
		: Base(eCoin, left, top)
	{
		m_Area.ObjectWidth = 16U;
		m_Area.ObjectHeight = 32U;
	}
	~Coin() = default;

	virtual void Update(float) override;
protected:
private:
};

class Bullet : public Object2D
{
public:
	Bullet(uint32_t left, uint32_t top)
		: Base(eBullet, left, top)
	{
		m_Area.ObjectWidth = 16U;
		m_Area.ObjectHeight = 16U;
	}
	~Bullet() = default;

	virtual void Update(float) override;
protected:
private:
};

class Exploder : public Object2D
{
public:
	Exploder(uint32_t left, uint32_t top)
		: Base(eExplode, left, top)
	{
		m_Area.ObjectWidth = 32U;
		m_Area.ObjectHeight = 32U;
	}
	~Exploder() = default;

	virtual void Update(float) override;
protected:
private:
};

class Mushroom : public Object2D
{
public:
	Mushroom(uint32_t left, uint32_t top)
		: Base(eMushroom, left, top)
	{
		m_Area.ObjectWidth = 32U;
		m_Area.ObjectHeight = 32U;
	}
	~Mushroom() = default;

	virtual void Update(float) override;
protected:
private:
};

class Monster : public Object2D
{
public:
	Monster(uint32_t left, uint32_t top)
		: Base(eMonster, left, top)
	{
		m_Area.ObjectWidth = 32U;
		m_Area.ObjectHeight = 32U;
	};
	~Monster() = default;

	virtual void Update(float) override {}
protected:
private:
};

class WalkingTurtle : public Object2D
{
public:
	WalkingTurtle(uint32_t left, uint32_t top)
		: Base(eWalkingTurtle, left, top)
	{
		m_Area.ObjectWidth = 32U;
		m_Area.ObjectHeight = 48U;
	}
	~WalkingTurtle() = default;

	virtual void Update(float) override {}
protected:
private:
};

class FlyingTurtle : public Object2D
{
public:
	FlyingTurtle(uint32_t left, uint32_t top)
		: Base(eFlyingTurtle, left, top)
	{
		m_Area.ObjectWidth = 32U;
		m_Area.ObjectHeight = 48U;
	}
	~FlyingTurtle() = default;

	virtual void Update(float) override {}
protected:
private:
};
