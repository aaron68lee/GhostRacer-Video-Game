#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool ccollide)
	:GraphObject(imageID, startX, startY, startDir, size, depth)
{
	// initialize each actor as alive
	alive = true;
	speedX = sx;
	speedY = sy;
	world = w;
	collide = ccollide;
	hp = -1;
	//id = imageID; // used for debugging only
}

bool Actor::isAlive()
{
	return alive;
}

void Actor::setAlive(bool a)
{
	alive = a;
}

void Actor::setSpeedX(double x)
{
	speedX = x;
}

double Actor::getSpeedX()
{
	return speedX;
}

void Actor::setSpeedY(double y)
{
	speedY = y;
}

double Actor::getSpeedY()
{
	return speedY;
}


StudentWorld* Actor::getWorld()
{
	return world;
}

// return if actor is collision-worthy avoidance
bool Actor::canCollide()
{
	return collide;
}

// virtual destructor
Actor::~Actor()
{

}

// check if this type of actor overlaps with ghost racer
bool Actor::overlapGhost()
{
	double deltaX = abs(getX() - getWorld()->getGhost()->getX());
	double deltaY = abs(getY() - getWorld()->getGhost()->getY());
	double radiusSum = getRadius() + getWorld()->getGhost()->getRadius();

	return (deltaX < radiusSum * 0.25 && deltaY < radiusSum * 0.6);
}

void Actor::setHP(int amt)
{
	hp = amt;
}

int Actor::getHP()
{
	return hp;
}

/* // used for debugging
int Actor::getId()
{
	return id;
}
*/

//===================================== Borderline ==================================================


BorderLine::BorderLine(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Actor(w, imageID, startX, startY, startDir, size, depth, sx, sy, false)
{

}

void BorderLine::doSomething()
{
	// move borderline to its new position

	int vertSpeed = getSpeedY() - getWorld()->getGhost()->getSpeedY();
	int horiSpeed = getSpeedX();

	const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

	moveTo(getX() + horiSpeed, getY() + vertSpeed);

	// change studentWorld's lastY for last white borderline Y value

	if (getX() != RIGHT_EDGE && getX() != LEFT_EDGE) // verify white border line
		getWorld()->setLastY(getY());

	// kill borderline if out of bounds

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setAlive(false);
}

bool BorderLine::isEnemy()
{
	return false;
}

bool BorderLine::canSpecialMove()
{
	return false;
}

BorderLine::~BorderLine()
{

}


//===================================== GhostRacer ==================================================


GhostRacer::GhostRacer(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Actor(w, imageID, startX, startY, startDir, size, depth, sx, sy, collide)
{
	// ghost racer's initial conditions
	setHP(100);
	sprays = 10;
	setSpeedY(0);
}

void GhostRacer::doSomething()
{
	// return if ghost rider is already dead

	if (!isAlive())
		return;

	const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	bool noCrash = true;

	// check to see if ghost racer hit left side of road

	if (getX() <= LEFT_EDGE)
	{
		if (getDirection() > 90)
		{
			setHP(getHP() - 10);
			if (getHP() <= 0)
				setAlive(false);

			setDirection(82);
			// play sound 
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			// go to step 5 on pg 29
			noCrash = false;
		}
	}

	// check to see if ghost racer hit right side of road

	if (getX() >= RIGHT_EDGE)
	{
		if (getDirection() < 90)
		{
			setHP(getHP() - 10);
			if (getHP() <= 0)
				setAlive(false);
			setDirection(98);
			// play sound 
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			// go to step 5 on pg 29
			noCrash = false;
		}
	}

	int ch;
	//get user input only if didn't hit left or right border

	if (getWorld()->getKey(ch) && noCrash)
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() < 114)
				setDirection(getDirection() + 8);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66)
				setDirection(getDirection() - 8);
			break;
		case KEY_PRESS_DOWN:
			if (getSpeedY() > -1)
				setSpeedY(getSpeedY() - 1);
			break;
		case KEY_PRESS_UP:
			if (getSpeedY() < 5)
				setSpeedY(getSpeedY() + 1);
			break;
		case KEY_PRESS_SPACE:
			if (sprays > 0)
			{
				//create holy water spray with angle = GH angle in front of ghost racer
				double startX = 0, startY = 0;
				getPositionInThisDirection(getDirection(), SPRITE_HEIGHT, startX, startY);

				HolyWaterProjectile* projectile = new HolyWaterProjectile(getWorld(), IID_HOLY_WATER_PROJECTILE, startX, startY, getDirection(), 1.0, 1, 0, 0, false);

				getWorld()->getActors()->push_back(projectile);

				// play spray sound
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				sprays--;
				// go to step 5 pg 30
			}
			break;
		default:
			break;
		}

	}

	// move ghost racer with new direction

	double max_shift_per_tick = 4.0;
	static const double PI = 4 * atan(1.0);
	double deltaX = cos(getDirection() * PI / 180) * max_shift_per_tick;
	moveTo(getX() + deltaX, getY());
}

int GhostRacer::getSprays()
{
	return sprays;
}

// any enemy actor can damage ghost racer

void GhostRacer::damageGhost(int damage)
{
	setHP(getHP() - damage);
	if (getHP() <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

// make sure oil slick can spin ghost racer

void GhostRacer::spinGhost()
{
	int spin = randInt(5, 20);
	int spinDir = randInt(0, 1);

	if (spinDir == 0)
		setDirection(getDirection() + spin);
	else
		setDirection(getDirection() - spin);

	// makes sure ghost racer direction always in bounds

	if (getDirection() < 60)
		setDirection(60);
	else if (getDirection() > 120)
		setDirection(120);

}

// allows other actors to heal ghost racer (i.e healing goodie)

void GhostRacer::healGhost(int heal)
{
	setHP(getHP() + heal);
}

void GhostRacer::increaseSprays()
{
	sprays += 10;
}

void GhostRacer::decreaseSprays()
{
	sprays--;
}

GhostRacer::~GhostRacer()
{
}

bool GhostRacer::isEnemy()
{
	return false;
}

bool GhostRacer::canSpecialMove()
{
	return false;
}

//===================================== Goodie ==================================================


Goodie::Goodie(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Actor(w, imageID, startX, startY, startDir, size, depth, 0, -4, false)
{

}

void Goodie::doSomething()
{
	// move the goodie down the screen 

	int vertSpeed = getSpeedY() - getWorld()->getGhost()->getSpeedY();
	int horiSpeed = 0;

	moveTo(getX() + horiSpeed, getY() + vertSpeed);

	// check to see if goodie is off screen

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}


	// do differentiated goodie stuff

	goodieSpecific();
}

Goodie::~Goodie()
{
}

bool Goodie::isEnemy()
{
	return false;
}

bool Goodie::canSpecialMove()
{
	return false;
}

//===================================== Healing Goodie ==================================================


HealingGoodie::HealingGoodie(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Goodie(w, imageID, startX, startY, startDir, size, depth, 0, -4, false)
{
}

void HealingGoodie::goodieSpecific()
{
	// ghostracer picked up healing goodie 

	if (overlapGhost())
	{
		setAlive(false);
		getWorld()->increaseScore(250);
		getWorld()->getGhost()->healGhost(10);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}

}

HealingGoodie::~HealingGoodie()
{
}

//===================================== Holy Water Projectile ==================================================


HolyWaterProjectile::HolyWaterProjectile(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Actor(w, imageID, startX, startY, startDir, 1, 1, sx, sy, false)
{
	maxDist = 160;
}

// helper function to determine if holy water overlaps with this other actor 

bool HolyWaterProjectile::overlapEnemy(Actor* a)
{
	double deltaX = abs(getX() - a->getX());
	double deltaY = abs(getY() - a->getY());
	double radiusSum = getRadius() + a->getRadius();

	return (deltaX < radiusSum * 0.25 && deltaY < radiusSum * 0.6);
}

bool HolyWaterProjectile::isEnemy()
{
	return false;
}

bool HolyWaterProjectile::canSpecialMove()
{
	return false;
}

void HolyWaterProjectile::doSomething()
{
	// have zombie peds, zombie cabs, ... check for overlap and damage themselves by 1 HP

	// iterate through list of actors to damage all enemies

	std::vector<Actor*>::iterator it;

	it = getWorld()->getActors()->begin();

	while (it != getWorld()->getActors()->end())
	{
		if ((*it)->canSpecialMove())
		{
			if (overlapEnemy(*it))
			{
				setAlive(false);

				// change direction of human ped
				(*it)->setSpeedX(-1 * ((*it)->getSpeedX()));

				if ((*it)->getDirection() == 180)
					(*it)->setDirection(0);
				else
					(*it)->setDirection(180);

				getWorld()->playSound(SOUND_PED_HURT);

				return;
			}
		}

		if ((*it)->isEnemy())
		{
			if (overlapEnemy(*it))
			{
				// damage enemy

				setAlive(false);
				(*it)->setHP((*it)->getHP() - 1);
				return;
			}
		}

		it++;
	}

	// move holy water projectile foward 

	moveForward(SPRITE_HEIGHT);
	maxDist -= SPRITE_HEIGHT;

	// check to see if projectile is off screen
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setAlive(false);

	// projectile dies if moved more than 160 pixels
	if (maxDist <= 0)
		setAlive(false);
}

HolyWaterProjectile::~HolyWaterProjectile()
{
}


//===================================== Holy Water Refill ==================================================


HolyWaterRefill::HolyWaterRefill(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Goodie(w, imageID, startX, startY, startDir, size, depth, 0, -4, false)
{
}

void HolyWaterRefill::goodieSpecific()
{
	// ghost racer picked up holy water refill

	if (overlapGhost())
	{
		setAlive(false);
		getWorld()->getGhost()->increaseSprays();
		getWorld()->increaseScore(50);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}

}

HolyWaterRefill::~HolyWaterRefill()
{
}

//===================================== Human Pedestrian ==================================================


HumanPedestrian::HumanPedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Pedestrian(w, imageID, startX, startY, startDir, size, depth, sx, sy, true)
{

}

void HumanPedestrian::pedSpecifics()
{
	// check if touching ghost racer

	if (!isAlive() || getHP() <= 0)
		return;

	// ghost racer will die if it touches human ped

	if (overlapGhost())
	{
		getWorld()->getGhost()->damageGhost(100);
	}
}


HumanPedestrian::~HumanPedestrian()
{
}

bool HumanPedestrian::canSpecialMove()
{
	return true;
}

//===================================== Oil Slick ==================================================


OilSlick::OilSlick(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Goodie(w, imageID, startX, startY, startDir, size, 2, 0, -4, false)
{
}

void OilSlick::goodieSpecific()
{
	// spin ghost if ghostracer touches the oil slick

	if (overlapGhost())
	{
		setAlive(false);
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getGhost()->spinGhost();

	}

}

OilSlick::~OilSlick()
{
}

//===================================== Pedestrian ==================================================


Pedestrian::Pedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Actor(w, imageID, startX, startY, startDir, size, depth, sx, sy, false)
{
	setHP(2);
	movePlan = 0;
}

void Pedestrian::doSomething()
{
	// do differentiated pedestrian stuff 

	pedSpecifics();

	// move the pedestrian 

	int vertSpeed = getSpeedY() - getWorld()->getGhost()->getSpeedY();
	int horiSpeed = getSpeedX();

	moveTo(getX() + horiSpeed, getY() + vertSpeed);

	// check to see if pedestrian is off screen

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setAlive(false);

	// update move plan

	setMovePlan(getMovePlan() - 1);
	if (getMovePlan() > 0)
		return;
	else
	{
		// move if move plan still valid

		int newSpeedX = randInt(-3, 3);

		while (newSpeedX == 0)
			newSpeedX = randInt(-3, 3);

		setSpeedX(newSpeedX);

		setMovePlan(randInt(4, 32));

		if (newSpeedX < 0)
			setDirection(180);
		else
			setDirection(0);

	}
}

int Pedestrian::getMovePlan()
{
	return movePlan;
}

void Pedestrian::setMovePlan(int plan)
{
	movePlan = plan;
}

Pedestrian::~Pedestrian()
{

}

bool Pedestrian::isEnemy()
{
	return true;
}


//===================================== Soul ==================================================


Soul::Soul(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Goodie(w, imageID, startX, startY, startDir, size, depth, 0, -4, false)
{

}

void Soul::goodieSpecific()
{
	// ghostracer rescued a lost soul 

	if (overlapGhost())
	{
		setAlive(false);
		getWorld()->soulSaved();
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
	}

	// rotate lost soul every tick

	setDirection(getDirection() + 10);

}

Soul::~Soul()
{
}

//===================================== Zombie Cab ==================================================


ZombieCab::ZombieCab(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide, int l)
	:Actor(w, imageID, startX, startY, startDir, size, depth, sx, sy, collide)
{
	alreadyHitGhost = false;
	lane = l;
	movePlan = 0;
	setHP(3);
	gotHit1 = false;
	gotHit2 = false;
}

void ZombieCab::doSomething()
{
	// zombie cab died by holy water projectile

	if (getHP() <= 0)
	{
		setAlive(false);
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		getWorld()->increaseScore(200);

		// 20% chance of generating an oil slick

		if (randInt(1, 5) == 1)
		{
			OilSlick* oil = new OilSlick(getWorld(), IID_OIL_SLICK, getX(), getY(), 0, randInt(2, 5), 2, 0, -4, false);
			getWorld()->getActors()->push_back(oil);
		}

	}

	// checks to see if zombie cab still alive but got hit at least once 

	if (getHP() == 2 && !gotHit1)
	{
		gotHit1 = true;
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}

	if (getHP() == 1 && !gotHit2)
	{
		gotHit2 = true;
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}

	// don't update zombie cab's position if it's dead

	if (!isAlive())
	{
		return;
	}


	// if zombie cab overlaps with ghost and didn't damage it already

	if (overlapGhost() && !alreadyHitGhost)
	{
		alreadyHitGhost = true;
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getWorld()->getGhost()->damageGhost(20);

		// change zombie cab orientation accordingly for bump into animation

		if (getX() <= getWorld()->getGhost()->getX())
		{
			setSpeedX(-5);
			setDirection(60 - randInt(0, 20));
		}
		else
		{
			setSpeedX(5);
			setDirection(120 + randInt(0, 20));
		}
	}

	// move zombie cab

	double vertSpeed = getSpeedY() - getWorld()->getGhost()->getSpeedY();
	//std::cout << "Cab: " << getSpeedY() << "\n\n\n";
	double horiSpeed = getSpeedX();

	moveTo(getX() + horiSpeed, getY() + vertSpeed);

	// check to see if zombie cab off screen

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return;
	}

	// slow zombie cab down

	const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	const double LANE_WIDTH = ROAD_WIDTH / 3;

	if (getSpeedY() > getWorld()->getGhost()->getSpeedY())
	{

		std::vector<Actor*>::iterator it;
		it = getWorld()->getActors()->begin();

		while (it != getWorld()->getActors()->end())
		{
			int otherX = (*it)->getX();
			int otherY = (*it)->getY();

			// check if there's a collision avoidance worthy actor in same lane and close enough IN FRONT OF CAB

			if (otherX > (((lane - 1) * LANE_WIDTH) + LEFT_EDGE) && otherX < ((lane * LANE_WIDTH) + LEFT_EDGE)
				&& otherY - getY() < 96 && (otherY - getY() > 0) && (*it)->canCollide() && this != (*it))
			{
				/*
				std::cout << "-------Cab-----:" << getSpeedY() << " @ " << getY() << std::endl;
				std::cout << otherY - getY() << "\n\n";

				std::cout << "-------Cab-----:" << getSpeedY() << std::endl;
				std::cout << (*it)->getId() << std::endl;
				std::cout << ((lane - 1) * LANE_WIDTH) + LEFT_EDGE << std::endl;
				std::cout << (lane * LANE_WIDTH) + LEFT_EDGE << "\n\n";
				std::cout << otherY << std::endl;
				std::cout << getY() << std::endl;
				std::cout << "-------Cab-----:" << getSpeedY() << std::endl << std::endl;
				*/

				int ghostX = getWorld()->getGhost()->getX();
				int ghostY = getWorld()->getGhost()->getY();

				if ((ghostX > ((lane - 1) * LANE_WIDTH) + LEFT_EDGE) && ((ghostX < (lane * LANE_WIDTH) + LEFT_EDGE))
					&& ghostY - getY() < 96 && ghostY - getY() > 0)
				{
					setSpeedY(getSpeedY() - 0.5);
					return;
				}
				//std::cout << getY() << ": about to Slowing down\n";
				setSpeedY(getSpeedY() - 0.5);
				//std::cout << getY() << ": Slowing down\n";
				return;
			}
			it++;
		}
	}
	else // speed zombie cab up if conditions are met
	{

		std::vector<Actor*>::iterator it2;
		it2 = getWorld()->getActors()->begin();
		int count = 0;
		while (it2 != getWorld()->getActors()->end())
		{
			int otherX = (*it2)->getX();
			int otherY = (*it2)->getY();

			// check if actor is in same lane and close enough BEHIND CAB

			if ((otherX > ((lane - 1) * LANE_WIDTH) + LEFT_EDGE) && (otherX < (lane * LANE_WIDTH) + LEFT_EDGE)
				&& getY() - otherY < 96 && getY() - otherY > 0 && (*it2)->canCollide() && this != (*it2))
			{
				/*
				std::cout << "::::::Cab::::::" << getSpeedY() << " @ " << getY() << std::endl;
				std::cout << getY() - otherY << "\n\n";

				std::cout << "::::::Cab::::::" << getSpeedY() << std::endl;
				std::cout << (*it2)->getId() << std::endl;
				std::cout << ((lane - 1) * LANE_WIDTH) + LEFT_EDGE << std::endl;
				std::cout << (lane * LANE_WIDTH) + LEFT_EDGE << "\n\n";
				std::cout << otherY << std::endl;
				std::cout << getY() << std::endl;
				std::cout << "::::::Cab::::::" << getSpeedY() << std::endl << std::endl;
				*/
				//std::cout << getY() << ": Speeding up\n";
				setSpeedY(getSpeedY() + 0.5);
				//std::cout << getY() << ": Speeding up\n";
				return;
			}
			it2++;
		}
	}

	movePlan--;
	//std::cout << "::" << movePlan << "::" << std::endl;
	//std::cout << "::::::Cab::::::" << getSpeedY() << " @ " << getY() << " lane: " << lane << std::endl;
	//std::cout << "\n\n";

	// update move plan for zombie cab (random movement)

	if (movePlan > 0)
		return;
	else
	{
		movePlan = randInt(4, 32);
		setSpeedY(getSpeedY() + randInt(-2, 2));
		/*
		std::cout << "====Ghost====" << getWorld()->getGhost()->getSpeedY() << std::endl;
		std::cout << "====Cab======" << getSpeedY() << std::endl << std::endl;
		*/
	}

}

ZombieCab::~ZombieCab()
{
}

bool ZombieCab::isEnemy()
{
	return true;
}

bool ZombieCab::canSpecialMove()
{
	return false;
}

//===================================== Zombie Pedestrian ==================================================


ZombiePedestrian::ZombiePedestrian(StudentWorld* w, int imageID, double startX, double startY, int startDir, double size, int depth, int sx, int sy, bool collide)
	:Pedestrian(w, imageID, startX, startY, startDir, size, depth, sx, sy, true)
{
	tickTillGrunt = 0;
	setHP(2);
	gotHit = false;
}

void ZombiePedestrian::pedSpecifics()
{
	// zombie ped touched ghost racer and died

	if (overlapGhost())
	{
		getWorld()->getGhost()->damageGhost(5);
		setHP(getHP() - 2);
		setAlive(false);
		getWorld()->playSound(SOUND_PED_DIE);
		getWorld()->increaseScore(150);
		return;
	}

	// got hit by holy water projectile but didn't die

	if (getHP() == 1 && !gotHit)
	{
		gotHit = true;
		getWorld()->playSound(SOUND_PED_HURT);
	}

	// killed by holy water projectile 

	if (getHP() == 0 && !overlapGhost())
	{
		setAlive(false);
		getWorld()->playSound(SOUND_PED_DIE);
		getWorld()->increaseScore(150);

		// 1 in 5 chance of dropping healing goodie

		if (randInt(1, 5) == 1)
		{
			HealingGoodie* heal = new HealingGoodie(getWorld(), IID_HEAL_GOODIE, getX(), getY(), 0, 1.0, 2, 0, -4, false);

			getWorld()->getActors()->push_back(heal);
		}
	}

	if (!isAlive() || getHP() <= 0)
		return;

	// zombie adjusts orientation towards ghost racer

	if (abs(getX() - getWorld()->getGhost()->getX()) <= 30 && (getY() > getWorld()->getGhost()->getY()))
	{
		setDirection(270);

		if (getX() - getWorld()->getGhost()->getX() > 0)
			setSpeedX(-1);
		else if (getX() - getWorld()->getGhost()->getX() < 0)
			setSpeedX(1);
		else
			setSpeedX(0);


	}

	// determine if zombie grunts

	tickTillGrunt--;

	if (tickTillGrunt <= 0)
	{
		getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
		tickTillGrunt = 20;
	}

	// move zombie done in ped doSomething()

	// check to see if pedestrian is off screen done in doSomething() method in Ped

	// update move plan done in Ped doSomething()

}


ZombiePedestrian::~ZombiePedestrian()
{
}

bool ZombiePedestrian::canSpecialMove()
{
	return false;
}

