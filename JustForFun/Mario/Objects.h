#pragma once

#include "Object2D.h"

class Coin : public Object2D
{
public:
	Coin()
		: Base(eCoin)
	{
	}
	~Coin() = default;

	virtual void Update(float) override;
protected:
private:
};

class Bullet : public Object2D
{
public:
	Bullet()
		: Base(eBullet)
	{
	}
	~Bullet() = default;

	virtual void Update(float) override;
protected:
private:
};

class Exploder : public Object2D
{
public:
	Exploder()
		: Base(eExplode)
	{
	}
	~Exploder() = default;

	virtual void Update(float) override;
protected:
private:
};

class Mushroom : public Object2D
{
public:
	Mushroom()
		: Base(eMushroom)
	{
	}
	~Mushroom() = default;

	virtual void Update(float) override;
protected:
private:
};

class Monster : public Object2D
{
public:
	Monster()
		: Base(eMonster)
	{
	};
	~Monster() = default;

	virtual void Update(float) override;
protected:
private:
};

class WalkingTurtle : public Object2D
{
public:
	WalkingTurtle()
		: Base(eTurtle)
	{
	}
	~WalkingTurtle() = default;

	virtual void Update(float) override;
protected:
private:
};

class FlyingTurtle : public Object2D
{
public:
	FlyingTurtle()
		: Base(eTurtle)
	{
	}
	~FlyingTurtle() = default;

	virtual void Update(float) override;
protected:
private:
};
