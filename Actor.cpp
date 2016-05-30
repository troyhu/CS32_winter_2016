#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
using namespace std;

GraphObject::Direction toDirection(int d)
{
	if (d == 1)			return GraphObject::up;
	else if (d == 2)	return GraphObject::down;
	else if (d == 3)	return GraphObject::left;
	else if (d == 4)	return GraphObject::right;
	else				return GraphObject::none;

}
bool perpendicular(int a, int b)
{
	if ((a == 1 || a == 2) && (b == 3 || b == 4))	return true;
	else if ((a == 3 || a == 4) && (b == 2 || b == 1))	return true;
	else return false;
	
}

int dir_cw(int a)
{
	if (a == 1)	return 4;
	if (a == 4)	return 2;
	if (a == 2)	return 3;
	else		return 1;
}
int dir_ccw(int a)
{
	if (a == 1)	return 3;
	if (a == 4)	return 1;
	if (a == 2)	return 4;
	else		return 2;
}

int Actor::dir() //none, up, down, left, right 
{
	if (getDirection() == up)	return 1;
	if (getDirection() == down)	return 2;
	if (getDirection() == left)	return 3;
	if (getDirection() == right)	return 4;
	else return 0;
}

bool Actor::moving(Direction d)
{
	int x = getX();
	int y = getY();
	if (getDirection() != d)
	{
		setDirection(d);
		moveTo(x, y);
		return false;
	}
	x = x + (d == right);	x = x - (d == left);	y = y + (d == up);		y = y - (d == down);
	moveTo(x, y);
	return true;
}

bool FrackMan::moving(Direction d)
{
	int x = getX();
	int y = getY();
	if (getDirection() != d)
	{
		setDirection(d);
		moveTo(x, y);
		return false;
	}
	else
	{
		x = x + (d == right);	x = x - (d == left);	y = y + (d == up);		y = y - (d == down);
		if (getWorld()->is_wall_block_me(getX(), getY(), dir(), 0) || getWorld()->is_boulder_block_me(getX(), getY(), dir(), 0)) {
			moveTo(getX(), getY());
			return false;
		}
		else {
			moveTo(x, y);
			return false;
		}
	}
}

void FrackMan::doSomething()
{
	int x = getX();
	int y = getY();
	if (getHitPoints() <= 0) {
		getWorld()->decLives();
		getWorld()->change_frackman("kill");
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
		
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
			case KEY_PRESS_LEFT:
				moving(left);
				getWorld()->digDirt(getX(), getY());
				break;
			case KEY_PRESS_RIGHT:
				moving(right);
				getWorld()->digDirt(getX(), getY());
				break;
			case KEY_PRESS_UP:
				moving(up);
				getWorld()->digDirt(getX(), getY());
				break;
			case KEY_PRESS_DOWN:
				moving(down);
				getWorld()->digDirt(getX(), getY());
				break;
			case KEY_PRESS_SPACE:
				if (m_water > 0)
					getWorld()->shoot(getX(), getY(), dir());
				break;
			case KEY_PRESS_TAB:
				if (m_gold > 0)
					getWorld()->drop_gold(getX(), getY());
				break;
			case 'z':
				if (m_sonar > 0)
					getWorld()->radar(getX(), getY());
				break;
			default:
				moveTo(getX(), getY());
				break;
		}
	}


}

void Boulder::doSomething()
{
	int x = getX();
	int y = getY();
	if (get_count() == 0 && !getWorld()->is_dirt_block_me(x, y, 2, 0))
		set_count(1);
	if(get_count() > 0)
		set_count(get_count() + 1);
	if (get_count() >= 30 && checkAlive())
	{
		canAnnoy = true;
		getWorld()->annoyP(x, y, 4, 9);
		
		moveTo(x, y - 1);
		x = getX();  y = getY();
		if (getWorld()->is_wall_block_me(x, y, 2, 0) || getWorld()->is_dirt_block_me(x, y, 2, 0) || getWorld()->is_boulder_block_me(x, y, 2, 0))
			setDead();
		if (getWorld()->is_near_ID(x, y, 1, 9)) {
			getWorld()->change_frackman("kill");
			getWorld()->decLives();
			getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		}
	}
	if (get_count() == 30)
		getWorld()->playSound(SOUND_FALLING_ROCK);
}

void Squirt::doSomething()
{
	int x = getX();
	int y = getY();
	if (get_count() >= 1 && checkAlive() && getWorld()->annoyP(x, y, 5, 16)) {
		setDead();
		return;
	}
	if (get_count() == 0) {
		set_count(1);
		if (getWorld()->is_cover_dirt(x, y) || getWorld()->is_wall_block_me(x, y, dir(), -1)
			|| getWorld()->is_near_ID(x, y, 4, 9)) {
			setDead();
		}
	}
	else 
	if (get_count() >= 1 && get_count() <= 4) {
		
		if (!getWorld()->is_dirt_block_me(x, y, dir(), 0) && !getWorld()->is_wall_block_me(x, y, dir(), 0)
			&& !getWorld()->is_boulder_block_me(x, y, dir(), 0)) {
			moving(getDirection());
			set_count(get_count() + 1);
		}
		else {
			setDead();
		}
	}
	else {
		setDead();
	}
	
}

bool pick::be_pickup()
{
	int x = getX();
	int y = getY();
	if (getWorld()->is_near_ID(x, y, 1, 16))
		setVisible(true);
	if (!is_secondHand() && getWorld()->is_near_ID(x, y, 1, 9)) {
		setDead();
		return true;
	}
	return false;
}

void pick::stayAlive()
{
	int T = max(100, 300 - 10 * getWorld()->getLevel());
	if (get_count() == T)
		setDead();
	else
		set_count(get_count() + 1);
}

void Gold::stayAlive()
{
	int T = 100;
	if (get_count() == T)
		setDead();
	else
		set_count(get_count() + 1);
}

void Gold::doSomething()
{
	if (be_pickup()) {
		getWorld()->increaseScore(10);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->change_frackman("gold", 1);

	}
	else
		/*if (getWorld()->is_near_ID(getX(), getY(), 2, 9) || getWorld()->is_near_ID(getX(), getY(), 3, 9)) {
			setDead();
		}
		else*/
	if(secondHand)
		stayAlive();

}
void Oil::doSomething()
{
	if (be_pickup()) {
		getWorld()->change_frackman("oil");
		getWorld()->increaseScore(1000);
		getWorld()->playSound(SOUND_FOUND_OIL);
	}
}
void Pool::doSomething()
{
	
	if (be_pickup()){
		getWorld()->change_frackman("water", 5);
		getWorld()->increaseScore(100);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
	else
	{
		stayAlive();
	}
	

}
void Sonar::doSomething()
{
	if (be_pickup()) {
		getWorld()->change_frackman("sonar", 1);
		getWorld()->increaseScore(75);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
	else
		stayAlive();
}

int Protester::findFrackman(int x, int y) // return the direction number
{
	return getWorld()->can_move_to_frackman(x, y);
}

int Hardcore_Protester::findFrackman(int x, int y) // return the direction number
{
	return getWorld()->hard_near_frack(x, y);
}

bool Protester::bribe(int x, int y)
{
	if (getWorld()->bribe(x, y)) {
		setState(2);
		getWorld()->increaseScore(25);
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		return true;
	}
	return false;
}

bool Hardcore_Protester::bribe(int x, int y)
{
	if (getWorld()->bribe(x, y)) {
		setState(1);
		getWorld()->increaseScore(50);
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		return true;
	}
	return false;
}

void bad_man::leave_map(int x, int y)
{
	int target = getWorld()->to_the_end(x, y);
		moving(toDirection(target));
}

void bad_man::prot_will_do()
{
	int T = max(25, 200 - getWorld()->getLevel());
	int P = min(15, 2 + getWorld()->getLevel() * 1.5);
	int tick = max(0, 3 - getWorld()->getLevel() / 4);
	int prob = min(90, getWorld()->getLevel() * 10 + 30);
	int N = max(50, 100 - getWorld()->getLevel() * 10);
	int x = getX();
	int y = getY();
	if (state != 2 && getHitPoints() <= 0) {
		state = 2;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		if (getHitPoints() == 0)
			getWorld()->increaseScore(250);
		else if (getHitPoints() == -1)
			getWorld()->increaseScore(100);
		else 
			getWorld()->increaseScore(500);
	}
	
	//===== check if is attacked by squirt or boulder
	if (state != 2 && getWorld()->annoy_by_squirt(x, y)) {
		state = 1;
		restcount = 0;
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	}
	if (state != 2 && getWorld()->annoy_by_boulder(x, y)) {
		state = 2;
		restcount = 0;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	}

	int wait = (state == 1) ? N : (state == 4)? tick*2:tick;

	//=========== enter here if U need to enable at the end of tick cycle
	if (getRestCount() == wait) {
		if (state == 1 || state == 4)	state = 0;
		setRestCount(0);
		if (count200 > 0)
			count200--;
		if (count15 > 0)
			count15--;
		
		//================= how to move
		Direction d = getDirection();
		int dire = dir();
		if (state == 2)
			if (x == 60 && y == 60)
				setDead();
			else
				leave_map(x, y);
		if (state == 0) // check if annoy the frackman
		{
			if (bribe(x, y)) {
				return;
				getWorld()->playSound(SOUND_DIG);
			}
			if (count15 == 0 && getWorld()->annoyF(x, y, dir())) {
				count15 = 15;
				return;
			}
			//========== find the frackman
			dire = findFrackman(x, y);
			if (dire != 0) {
				d = toDirection(dire); 
				stepsToMoveInCurrentDirection = 1;
			}
			else //=========== if not facing the frackman
			if (stepsToMoveInCurrentDirection == 0) {
				
				dire = getWorld()->choose_diretion(x, y);
				if (perpendicular(dir(), dire))
					count200 = 200;
				d = toDirection(dire);
				//setDirection(d);
				
				stepsToMoveInCurrentDirection = rand() % 53 + 8 + 1;
					
			}
			else if (count200 == 0)
			{
				bool cw = getWorld()->check_viable(x, y, dir_cw(dir()));
				bool ccw = getWorld()->check_viable(x, y, dir_ccw(dir()));
				if (cw)
					if (ccw && (rand() % 2 == 1))
						dire = dir_ccw(dir());
					else
						dire = dir_cw(dir());
				else
					if (ccw)
						dire = dir_ccw(dir());
					else
						dire = dir();

				if (perpendicular(dir(), dire)) {
					d = toDirection(dire);
					count200 = 200;
					//setDirection(d);
					stepsToMoveInCurrentDirection = rand() % 53 + 8 + 1;
				}
			}

			if (count15 == 0) {
				moving(d);					
				
				stepsToMoveInCurrentDirection--;
				x = getX(); y = getY();
				if (!getWorld()->check_viable(x, y, dir())) {
					state = 4;
					stepsToMoveInCurrentDirection = 0;
				}
			}
		}
	}
	else
		setRestCount(getRestCount() + 1);
	
}

void Protester::doSomething()
{
	if (checkAlive())
		prot_will_do();
}

void Hardcore_Protester::doSomething()
{
	if(checkAlive())
		prot_will_do();
}