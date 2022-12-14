#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor : public GraphObject
{
	public:
		Actor(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
		virtual void doSomething() = 0; // each doSomething() method is unique
		bool isAlive();
		void setAlive(bool a);
		void setSpeedX(double x);
		double getSpeedX();
		void setSpeedY(double y);
		double getSpeedY();
		StudentWorld* getWorld(); // get pointer to world actors are in
		virtual bool isEnemy() = 0;
		virtual bool canSpecialMove() = 0; // true only for actors that can have their movement affected by holy water projectile
		bool canCollide();
		virtual ~Actor(); // implement destructor in all subclasses
		bool overlapGhost(); // every actor knows it's location relative to ghost racer
		void setHP(int amt);
		int getHP();
		//int getId(); // used for debugging only
private:
		bool alive;
		double speedX;
		double speedY;
		StudentWorld* world;
		bool collide;
		int hp;
		//int id; debugging only

};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//===================================== Borderline ==================================================

class BorderLine : public Actor
{
public:
	BorderLine(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void doSomething();
	virtual bool isEnemy();
	virtual bool canSpecialMove();
	~BorderLine();
private:

};

//===================================== GhostRacer ==================================================


class GhostRacer : public Actor
{
public:
	GhostRacer(StudentWorld* w, int imageID, double startX = 128, double startY = 32, int startDir = 90, double size = 1.0, int depth = 0, int sx = 0, int sy = 0, bool collide = true);
	virtual void doSomething();
	int getSprays();
	void damageGhost(int damage);
	void spinGhost();
	void healGhost(int heal);
	void increaseSprays(); // increases by 10
	void decreaseSprays(); // decreases by 1
	~GhostRacer();
	virtual bool isEnemy();
	virtual bool canSpecialMove();
private:
	int sprays;


};

//===================================== Goodie ==================================================

class Goodie : public Actor
{
public:
	Goodie(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth = 2, int sx = 0, int sy = -4, bool collide = false);
	virtual void doSomething();
	virtual void goodieSpecific() = 0;
	~Goodie();
	virtual bool isEnemy();
	virtual bool canSpecialMove();
private:

};


//===================================== Pedestrian ==================================================


class Pedestrian : public Actor
{
public:
	Pedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void doSomething();
	int getMovePlan();
	void setMovePlan(int plan);
	virtual void pedSpecifics() = 0;
	~Pedestrian();
	virtual bool isEnemy();
private:
	int movePlan;

};

//===================================== Healing Goodie ==================================================

class HealingGoodie : public Goodie
{
public:
	HealingGoodie(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void goodieSpecific();
	~HealingGoodie();
private:
};

//===================================== Holy Water Projectile ==================================================


class HolyWaterProjectile :
	public Actor
{
public:
	HolyWaterProjectile(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void doSomething();
	~HolyWaterProjectile();
	bool overlapEnemy(Actor* a);
	virtual bool isEnemy();
	virtual bool canSpecialMove();
private:
	int maxDist;
};

//===================================== Holy Water Refill ==================================================

class HolyWaterRefill : public Goodie
{
public:
	HolyWaterRefill(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void goodieSpecific();
	~HolyWaterRefill();
private:

};

//===================================== Human Pedestrian ==================================================

class HumanPedestrian :
	public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void pedSpecifics();
	~HumanPedestrian();
	virtual bool canSpecialMove();
private:


};

//===================================== Oil Slick ==================================================

class OilSlick : public Goodie
{
public:
	OilSlick(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void goodieSpecific();
	~OilSlick();

private:
};

//===================================== Soul ==================================================

class Soul :
	public Goodie
{
public:
	Soul(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void goodieSpecific();
	~Soul();
private:

};

//===================================== Zombie Cab ==================================================


class ZombieCab :
	public Actor
{
public:
	ZombieCab(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide, int l);
	virtual void doSomething();
	~ZombieCab();
	virtual bool isEnemy();
	virtual bool canSpecialMove();
private:
	bool alreadyHitGhost;
	int lane;
	int movePlan;
	bool gotHit1;
	bool gotHit2;

};

//===================================== Zombie Pedestrian ==================================================

class ZombiePedestrian :
	public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide);
	virtual void pedSpecifics();
	~ZombiePedestrian();
	virtual bool canSpecialMove();
private:
	int tickTillGrunt;
	bool gotHit;

};

#endif // ACTOR_H_

