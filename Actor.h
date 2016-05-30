#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
#include "GameWorld.h"
#include "StudentWorld.h"

class StudentWorld;
class Actor: public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world)
	 : GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), is_alive(true),count(0) {}
	virtual ~Actor() { }
	virtual void doSomething() = 0;
	virtual int ID() { return 0; }
	int dir();
	bool checkAlive(){ return is_alive; }
	void setDead(){ is_alive = false; }
	virtual bool moving(Direction d);
	virtual bool can_annoy() { return false; }
	virtual bool can_be_annoy() { return true; }
	virtual bool is_secondHand() { return false; }
	int get_count() { return count; }
	void set_count(int c) { count = c; }
	StudentWorld* getWorld() { return m_world; }
	virtual void beHit(int damage) { }
private:
	StudentWorld* m_world;
	bool is_alive;
	int count;
};

class Dirt: public Actor
{
public:
	Dirt(int startX, int startY, StudentWorld* world)
	 :Actor(IID_DIRT, startX, startY, right, 0.25, 3, world){
	}
	virtual ~Dirt(){}
	virtual void doSomething() {}
private:	
};

class man : public Actor
{
public:
	man(int imageID, int startX, int startY, Direction dir, StudentWorld* world, int health)
	:Actor(imageID, startX, startY, dir, 1, 0, world), m_hit(health) {}
	virtual ~man() {}
	virtual int ID() { return 1; }
	virtual void doSomething() {}
	int getHitPoints() const { return m_hit; }
	void beHit(int damage) { m_hit -= damage; }
private:
	int m_hit;
};

class FrackMan: public man
{
public:
	FrackMan(StudentWorld* world)
	 :man(IID_PLAYER, 30, 60, right, world, 10), m_oil(0), m_water(5), m_sonar(1), m_gold(0){}
	virtual ~FrackMan() {};
	virtual bool moving(Direction d);
	virtual void doSomething();
	int get_water() { return m_water; }
	void add_water(int w) { m_water += w; }
	int get_sonar() { return m_sonar; }
	void add_sonar(int w) { m_sonar += w; }
	int get_gold() { return m_gold; }
	void add_gold(int w) { m_gold += w; }
	void add_oil() { m_oil += 1; }
	int get_oil() { return m_oil; }
private:
	int m_water;
	int m_sonar;
	int m_gold;
	int m_oil;
};

class bad_man : public man
{
public:
	bad_man(int mID, StudentWorld* world, int h)
		:man(mID, 60, 60, left, world, h), state(0), restcount(0), count200(200), count15(0)
		,stepsToMoveInCurrentDirection(0){}
	virtual ~bad_man() {}
	void prot_will_do();
	void leave_map(int x, int y);
	virtual int findFrackman(int x, int y) = 0;
	virtual bool bribe(int x, int y) = 0;
	int getState() { return state; }
	void setState(int s) { state = s; }
	int getRestCount() { return restcount; }
	void setRestCount(int s) { restcount = s; }
	int getCount200() { return count200; }
	void setCount200(int s) { count200 = s; }
	int getCount15() { return count15; }
	void setCount15(int s) { count15 = s; }
	virtual bool can_be_annoy() { return (state != 2); }

private:
	int state; // 0: free moving, 1: stunned, 2: leaving, 3: resting
	int restcount;	// move interval
	int count200;  // turn left or right interval
	int count15;   // shout interval
	int stepsToMoveInCurrentDirection;
};

class Protester : public bad_man
{
public:
	Protester(StudentWorld* world) :bad_man(IID_PROTESTER, world, 5) {}
	virtual ~Protester() {}
	virtual int ID() { return 2; }
	virtual int findFrackman(int x, int y);
	virtual bool bribe(int x, int y);
	virtual void doSomething();
private:

};

class Hardcore_Protester : public bad_man
{
public:
	Hardcore_Protester(StudentWorld* world) :bad_man(IID_HARD_CORE_PROTESTER, world, 20) {}
	virtual ~Hardcore_Protester() {}
	virtual int ID() { return 3; }
	virtual int findFrackman(int x, int y);
	virtual bool bribe(int x, int y);
	virtual void doSomething();
private:

};

class Boulder : public Actor
{
public:
	Boulder(int startX, int startY, StudentWorld* world)
		:Actor(IID_BOULDER, startX, startY, down, 1.0, 1, world), canAnnoy(false){}
	virtual ~Boulder() {}
	virtual void doSomething();
	virtual bool can_annoy() { return canAnnoy; }
	virtual int ID() { return 4; }
	//virtual bool can_pass_through_me() { return false; }

private:
	bool canAnnoy;
};

class Squirt : public Actor
{
public:
	Squirt(int startX, int startY, Direction dir, StudentWorld* world)
		: Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 2, world) {}
	virtual ~Squirt() {}
	virtual int ID() { return 5; }
	virtual void doSomething();
private:

};

class pick : public Actor
{
public:
	pick(int imageID, int startX, int startY, StudentWorld* world)
		: Actor(imageID, startX, startY, right, 1.0, 1, world){}
	virtual ~pick() {};
	
	virtual void stayAlive();
	bool be_pickup();
};

class Gold : public pick
{
public:
	Gold(int startX, int startY, StudentWorld* world)
		: pick(IID_GOLD, startX, startY, world), secondHand(false){}
	~Gold() {};
	virtual void doSomething();
	virtual int ID() { return 6; }
	virtual void stayAlive();
	virtual bool is_secondHand() { return secondHand; }
	void set_secondHand(bool s) { secondHand = s; }
private:
	bool secondHand;
};

class Oil : public pick
{
public:
	Oil(int startX, int startY, StudentWorld* world)
		: pick(IID_BARREL, startX, startY, world) {}
	virtual ~Oil() {};
	virtual int ID() { return 7; }
	virtual void doSomething();
private:

};

class Pool : public pick
{
public:
	Pool(int startX, int startY, StudentWorld* world)
		: pick(IID_WATER_POOL, startX, startY, world) {}
	virtual ~Pool() {};
	virtual int ID() { return 8; }
	virtual void doSomething();
private:
};

class Sonar : public pick
{
public:
	Sonar(int startX, int startY, StudentWorld* world)
		: pick(IID_SONAR, startX, startY, world) {}
	virtual ~Sonar() {};
	virtual int ID() { return 9; }
	virtual void doSomething();
private:
};

#endif // ACTOR_H_
