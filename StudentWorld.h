#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include <vector>
#include <map>
#include <queue>
#include "Actor.h"
#include "GraphObject.h"

struct Cood
{
	int x;
	int y;
	bool operator < (const Cood &c) const
	{
		if (x + y * 60  < c.x + c.y * 60)
			return true;
		return false;
	}
};
struct SuperCood
{
	int x, y, d;
};
int min(int a, int b);
int max(int a, int b);
int pow2(int a);
class Actor;
class Dirt;
class FrackMan;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir), tickCount(0), num_prot(0)
	{
		m_frackman = NULL;
		for(int x = 0; x < 64; x++)
			for(int y =0; y < 64; y++)
				m_dirt[x][y] = NULL;
	}

	virtual ~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	void setDisplayText();
	void addDirt();
	void digDirt(int x, int y);
	void insert_BGL();
	void insert_pool_sonar();
	void shoot(int x, int y, int d);
	void drop_gold(int x, int y);
	void radar(int x, int y);
	bool is_near(int x, int y, int r_sqr, int tx, int ty);
	bool is_wall_block_me(int x, int y, int d, int dist);
	bool is_dirt_block_me(int x, int y, int d, int dist);
	bool is_cover_dirt(int x, int y);
	bool is_boulder_block_me(int x, int y, int d, int dist);
	bool am_I_facing_it(int x, int y, int d, int tx, int ty);
	bool annoy_by_squirt(int x, int y);
    bool annoy_by_boulder(int x, int y);
	bool is_near_ID(int x, int y, int ID, int sqr);
	int check_game_state();
	void add_protester();
	void change_frackman(std::string s, int val = 0);
	bool annoyP(int x, int y, int ID, int sqr);
	bool annoyF(int x, int y, int d);
	int choose_diretion(int x, int y);
	bool check_viable(int x, int y, int d);
	int can_move_to_frackman(int x, int y);
	bool bribe(int x, int y);
	int to_the_end(int x, int y);
	int to_the_frackman(int x, int y);
	void set_map(int m_map[61][61], int tx, int ty);
	//void set_map_f(int tx, int ty);
	int hard_near_frack(int x, int y);

private:
	FrackMan* m_frackman;
	Dirt* m_dirt[64][64];
	std::vector<Actor*> m_actor;
	int tickCount;
	int num_prot;
	std::queue<SuperCood> mq;
	int m_map[61][61]; // record the distance between (x,y) to (60, 60)
	int m_map_f[61][61];
};



#endif // STUDENTWORLD_H_
