#include "StudentWorld.h"
#include <string>
#include "GraphObject.h"
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

Cood move_cood(int x, int y, GraphObject::Direction d)
{
	x = x + (d == GraphObject::right);
	x = x - (d == GraphObject::left);
	y = y + (d == GraphObject::up);
	y = y - (d == GraphObject::down);
	Cood re;
	re.x = x; re.y = y;
	return re;
}
vector<struct Cood> randObject(int num)
{
	srand(unsigned(time(NULL)));
	vector<struct Cood> map, result;
	Cood tmp;
	for (int i = 0; i <= 26; i++)
		for (int j = 20; j <= 56; j++)
		{
			tmp.x = i;
			tmp.y = j;
			map.push_back(tmp);
			tmp.x += 34;
			map.push_back(tmp);
		}
	for (int i = 0; i < num; i++)
	{
		tmp = map.at(rand() % map.size());
		result.push_back(tmp);
		vector<struct Cood>::iterator it = map.begin();
		for (int n = 0; n < map.size(); n++)
		{
			if (abs(it->x - tmp.x) <= 5 && abs(it->y - tmp.y) <= 5 && abs((it->x - tmp.x)*(it->y - tmp.y)) <= 16) {
				if (it != map.begin())
					it = map.erase(it);
				else {
					map.erase(it);
					it = map.begin();
				}
				n--;
			}
			else it++;
		}
	}
	return result;
}
string itos(int value, int num_digit)
{
	string s;
	char* tmp = new char;
	sprintf(tmp, "%d", value);
	s.append(tmp);
	int size = s.size();
	if (num_digit < 6)
		if (num_digit == size);
		else s.insert(0, " ");
	else
		for (int i = 0; i < num_digit - size; i++)
			s.insert(0, "0");
	return s;
}

int min(int a, int b) { return (a <= b) ? a : b; }
int max(int a, int b) { return (a >= b) ? a : b; }
int pow2(int a) { return a*a; }
void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = m_frackman->getHitPoints() * 10;
	int squirts = m_frackman->get_water();
	int gold = m_frackman->get_gold();
	int sonar = m_frackman->get_sonar();
	int barrelsLeft = min(2 + getLevel(), 20) - m_frackman->get_oil();
	string s;
	s = "Scr: " + itos(score, 6) + "  Lvl: " + itos(level, 2) + "  Lives: " + itos(lives, 1) + "  Hlth: " + itos(health, 3)
		+ "%  Wtr: " + itos(squirts, 2) + "  Gld: " + itos(gold, 2) + "  Sonar: " + itos(sonar, 2) + "  Oil Left: "
		+ itos(barrelsLeft, 2);
	//sprintf(s, "Scr: ","%6d", "   Lvl: ", "%2d", "   Lives: ", "%d", "   Hlth: ", "%3d","%   Water: ", "%2d","  Gld: ", "%2d",
	//		"  Sonar: ", "%2d", "  Oil Left: ", "%2d", score, level, lives, health, squirts, gold, sonar, barrelsLeft);
	setGameStatText(s);
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld:: ~StudentWorld()
{
	vector<Actor*>::iterator it;
	for (it = m_actor.begin(); it != m_actor.end(); it++)
		delete *it;
	m_actor.clear();

	delete m_frackman;
	for (int x = 0; x < 64; x++)
		for (int y = 0; y < 64; y++) {
			if(m_dirt[x][y]!=NULL)
				delete m_dirt[x][y];
			m_dirt[x][y] = NULL;
		}
}

int StudentWorld:: init()
{
	for (int i = 0; i <= 60; i++)
		for (int j = 0; j <= 60; j++)
			m_map[i][j] = -1;
	for (int i = 0; i <= 60; i++)
		for (int j = 0; j <= 60; j++)
			m_map_f[i][j] = -1;
	tickCount = 0;
	num_prot = 0;
	m_frackman = new FrackMan(this);
	m_frackman->setVisible(true);
	addDirt();
	
	insert_BGL();
	
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld:: move()
{
	
	setDisplayText();
	insert_pool_sonar();
	m_frackman->doSomething();

	set_map(m_map, 60, 60);
	set_map(m_map_f, m_frackman->getX(), m_frackman->getY());

	/*for (int i = 60; i >= 0; i--) {
		for (int j = 0; j <= 60; j++)
			cerr << m_map_f[j][i] << " ";
		cerr << endl;
	}
	cerr << endl; cerr << endl;
	cerr << m_frackman->getX() << "  " << m_frackman->getY();
	cerr << endl; cerr << endl; cerr << endl; cerr << endl; cerr << endl; cerr << endl; cerr << endl;*/
	
	for (int n = 0; n < m_actor.size(); n++)
		m_actor.at(n)->doSomething();

	vector<Actor*>::iterator it = m_actor.begin();
	for (int n = 0; n < m_actor.size(); n++)
	{
		//m_actor.at(n)->doSomething();
		if (!m_actor.at(n)->checkAlive()) {
			if (it != m_actor.begin()) {
				delete m_actor.at(n);
				it = m_actor.erase(it);
			}
			else
			{
				delete m_actor.at(n);
				m_actor.erase(it);
				it = m_actor.begin();
			}
			n--;
		}
		else
			it++;
	}
	add_protester();
	
	return check_game_state();
}

void StudentWorld:: cleanUp()
{
	vector<Actor*>::iterator it;
	for(it = m_actor.begin(); it != m_actor.end(); it++)
		delete *it;
	m_actor.clear();
	delete m_frackman;
	for(int x = 0; x < 64; x++)
		for(int y = 0; y < 64; y++){
			if(m_dirt[x][y]!=NULL)
				delete m_dirt[x][y];
			m_dirt[x][y] = NULL;
		}
}

void StudentWorld::insert_BGL()
{
	int B = min(getLevel() / 2 + 2, 6);
	int G = max(5 - getLevel() / 2, 2);
	int L = min(2 + getLevel(), 20);
	vector<struct Cood> BGL = randObject(B + G + L);
	for (int i = 0; i < B; i++)
	{
		Boulder* bou = new Boulder(BGL.at(i).x, BGL.at(i).y, this);
		m_actor.push_back(bou);
		bou->setVisible(true);
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++) {
				delete m_dirt[x + BGL.at(i).x][y + BGL.at(i).y];
				m_dirt[x + BGL.at(i).x][y + BGL.at(i).y] = NULL;
			}
	}
	for (int i = 0 + G; i < B + G; i++)
	{
		Gold* gd = new Gold(BGL.at(i).x, BGL.at(i).y, this);
		m_actor.push_back(gd);
	}
	for (int i = 0 + B + G; i < B + G + L; i++)
	{
		Oil* bo = new Oil(BGL.at(i).x, BGL.at(i).y, this);
		m_actor.push_back(bo);
	}
}

void StudentWorld::insert_pool_sonar()
{
	int G = getLevel() * 25 + 300;
	if (rand() % G == 1)
	{
		if (rand() % 5 == 1)
		{
			Sonar* so = new Sonar(0, 60, this);
			so->setVisible(true);
			m_actor.push_back(so);
		}
		else
		{
			int x, y;
			while (1)
			{
				x = rand() % 61;
				y = rand() % 61;
				bool tmp = true;
				for (int i = 0; i < 4; i++)
					for (int j = 0; j < 4; j++)
						if (m_dirt[x + i][y + j])
							tmp = tmp && false;
				if (tmp)
					break;
			}
			Pool* po = new Pool(x, y, this);
			// Pool* po = new Pool(30, 60, this);
			po->setVisible(true);
			m_actor.push_back(po);
		}
	}
}

void StudentWorld::addDirt()
{
	for (int x = 0; x < 30; x++)
		for (int y = 0; y < 60; y++) {
			m_dirt[x][y] = new Dirt(x, y, this);
			m_dirt[x + 34][y] = new Dirt(x + 34, y, this);
			m_dirt[x][y]->setVisible(true);
			m_dirt[x + 34][y]->setVisible(true);
		}
	for (int x = 30; x < 34; x++)
		for (int y = 0; y < 4; y++) {
			m_dirt[x][y] = new Dirt(x, y, this);
			m_dirt[x][y]->setVisible(true);
		}
}

void StudentWorld::digDirt(int x, int y)
{
	bool sound_dig_flag = false;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (m_dirt[x + i][y + j] != NULL) {
				delete m_dirt[x + i][y + j];
				m_dirt[x + i][y + j] = NULL;
				sound_dig_flag = true;
			}
	if (sound_dig_flag)	playSound(SOUND_DIG);
}

void StudentWorld::shoot(int x, int y, int d)
{
	if (d == 4)	x += 4;
	if (d == 3)	x -= 4;
	if (d == 1)	y += 4;
	if (d == 2)	y -= 4;
	Squirt* sqt = new Squirt(x, y, m_frackman->getDirection(), this);
	sqt->setVisible(true);
	m_actor.insert(m_actor.begin(), sqt);
	m_frackman->add_water(-1);
	playSound(SOUND_PLAYER_SQUIRT);
}

void StudentWorld::drop_gold(int x, int y)
{
	Gold* gd = new Gold(m_frackman->getX(), m_frackman->getY(), this);
	gd->setVisible(true);
	gd->set_secondHand(true);
	m_actor.push_back(gd);
	m_frackman->add_gold(-1);
}

void StudentWorld::radar(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++) {
		if (pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) <= 144)
			m_actor.at(i)->setVisible(true);
	}
	playSound(SOUND_SONAR);
	m_frackman->add_sonar(-1);
}

bool StudentWorld::is_near(int x, int y, int r_sqr, int tx, int ty)
{
	if (pow2(tx - x) + pow2(ty - y) <= r_sqr)
		return true;
	else
		return false;
}

bool StudentWorld::is_wall_block_me(int x, int y, int d, int dist)
{
	if (d == 1 && y >= 60 - dist)	return true;
	else if (d == 2 && y <= 0 + dist)	return true;
	else if (d == 3 && x <= 0 + dist)	return true;
	else if (d == 4 && x >= 60 - dist)	return true;
	else return false;
}

bool StudentWorld::is_dirt_block_me(int x, int y, int d, int dist)
{
	if (d == 1 && y<60 - dist && (m_dirt[x][y + 4 + dist] || m_dirt[x + 1][y + 4 + dist] 
		|| m_dirt[x + 2][y + 4 + dist] || m_dirt[x + 3][y + 4 + dist]))
		return true;
	else if (d == 2 && y>dist && (m_dirt[x][y - 1 - dist] || m_dirt[x + 1][y - 1 - dist] 
		|| m_dirt[x + 2][y - 1 - dist] || m_dirt[x + 3][y - 1 - dist]))
		return true;
	else if (d == 3 && x>dist && (m_dirt[x-1-dist][y] || m_dirt[x-1-dist][y + 1] 
		|| m_dirt[x-1-dist][y + 2] || m_dirt[x-1-dist][y + 3]))
		return true;
	else if (d == 4 && x<60-dist && (m_dirt[x+4+dist][y] || m_dirt[x+4+dist][y + 1] 
		|| m_dirt[x+4+dist][y + 2] || m_dirt[x+4+dist][y + 3]))
		return true;
	else
		return false;
}

bool StudentWorld::is_cover_dirt(int x, int y)
{
	bool flag = false;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (m_dirt[x + i][y + j] != NULL) {
				flag = true;
			}
	return flag;
}

bool StudentWorld::is_boulder_block_me(int x, int y, int d, int dist)
{
	for (int i = 0; i < m_actor.size(); i++) {
		if (am_I_facing_it(x, y, d, m_actor.at(i)->getX(), m_actor.at(i)->getY()))
			if (m_actor.at(i)->ID() == 4 && pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) <= 16+dist)
				return true;
	}
	return false;
}

bool StudentWorld::am_I_facing_it(int x, int y, int d, int tx, int ty)
{
	if (d == 1 && ty >= y + 1 && x <= tx + 3 && x >= tx - 3)	return true;
	else if (d == 2 && ty <= y - 1 && x <= tx + 3 && x >= tx - 3)	return true;
	else if (d == 3 && tx <= x - 1 && y <= ty + 3 && y >= ty - 3)	return true;
	else if (d == 4 && tx >= x + 1 && y <= ty + 3 && y >= ty - 3)	return true;
	else return false;
}

bool StudentWorld::annoy_by_boulder(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++) {
		if (m_actor.at(i)->can_annoy() && pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) < 16)
			return true;
	}
	return false;
}

bool StudentWorld::annoy_by_squirt(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++) {
		if (m_actor.at(i)->ID() == 5 && m_actor.at(i)->checkAlive() && pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) < 16)
			return true;
	}
	return false;
}

bool StudentWorld::is_near_ID(int x, int y, int ID, int sqr)
{
	if (ID == 1)
		return is_near(x, y, sqr, m_frackman->getX(), m_frackman->getY());
	for (int i = 0; i < m_actor.size(); i++) {
		if (m_actor.at(i)->ID() == ID)
			if (pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) <= sqr)
				return true;
	}
	return false;
}

void StudentWorld::add_protester()
{
	int T = max(25, 200 - getLevel());
	int P = min(15, 2 + getLevel() * 1.5);
	int wait = max(0, 3 - getLevel() / 4);
	int prob = min(90, getLevel() * 10 + 30);
	if (tickCount == 0 && num_prot < P)
	{
		if (rand() % 100 > prob) {
			Protester* prot = new Protester(this);
			prot->setVisible(true);
			m_actor.push_back(prot);
			// prot->set_count(wait);
		}
		else {
			Hardcore_Protester* prot = new Hardcore_Protester(this);
			prot->setVisible(true);
			m_actor.push_back(prot);
			// prot->set_count(wait);
		}
		num_prot++;
	}

	tickCount++;
	if (tickCount == T)
		tickCount = 0;

}

void StudentWorld::change_frackman(string s, int val)
{
	if (s == "gold")	m_frackman->add_gold(val);
	if (s == "oil")		m_frackman->add_oil();
	if (s == "water")	m_frackman->add_water(val);
	if (s == "sonar")	m_frackman->add_sonar(val);
	if (s == "kill")	m_frackman->setDead();
}

bool StudentWorld::annoyP(int x, int y, int ID, int sqr)
{
	bool re = false;
	for (int i = 0; i < m_actor.size(); i++) {
		if (m_actor.at(i)->ID() == 2 || m_actor.at(i)->ID() == 3)
			if (m_actor.at(i)->can_be_annoy() && pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) <= sqr) {
				m_actor.at(i)->beHit((ID == 4)?100 : 2);
				re = re || true;
			}
	}
	return re;
}

bool StudentWorld::annoyF(int x, int y, int d)
{
	if (am_I_facing_it(x, y, d, m_frackman->getX(), m_frackman->getY()) && is_near_ID(x, y, 1, 16))
	{
		m_frackman->beHit(2);
		playSound(SOUND_PROTESTER_YELL);
		playSound(SOUND_PLAYER_ANNOYED);
		return true;
	}
	else
		return false;
}

int StudentWorld::choose_diretion(int x, int y)
{
	// GraphObject::Direction re;
	int d;
	while (1)
	{
		d = rand() % 4 + 1;
		if (check_viable(x, y, d))
			break;
	}
	return d;
	// if (d == 1)			return GraphObject::up;
	// else if (d == 2)	return GraphObject::down;
	// else if (d == 3)	return GraphObject::left;
	// else				return GraphObject::right;
}

bool StudentWorld::check_viable(int x, int y, int d)
{
	if (!is_wall_block_me(x, y, d, 0) && !is_dirt_block_me(x, y, d, 0)
		&& !is_boulder_block_me(x, y, d, 0))
		return true;
	else 
		return false;
}

int StudentWorld::can_move_to_frackman(int x, int y)
{
	int d = 0;
	int tx = m_frackman->getX();
	int ty = m_frackman->getY();
	if (!is_near_ID(x, y, 1, 16)) {
		if (am_I_facing_it(x, y, 1, tx, ty))
			d = 1;
		else if (am_I_facing_it(x, y, 2, tx, ty))
			d = 2;
		else if (am_I_facing_it(x, y, 3, tx, ty))
			d = 3;
		else if (am_I_facing_it(x, y, 4, tx, ty))
			d = 4;
	}
	if (d == 0)	return 0;
	//============== if we find a direction, try to make sure this path is clean.
	if (d == 1) {
		for (int i = 0; i < ty - y - 4; i++)
			if (!check_viable(x, y+i, 1))	return 0;
		return 1;
	}
	else if (d == 2) {
		for (int i = 0; i < y - ty - 4; i++)
			if (!check_viable(x, y-i, 2))	return 0;
		return 2;
	}
	else if (d == 3) {
		for (int i = 0; i < x - tx - 4; i++)
			if (!check_viable(x-i, y, 3))	return 0;
		return 3;
	}
	else if (d == 4) {
		for (int i = 0; i < tx - x - 4; i++)
			if (!check_viable(x+i, y, 4))	return 0;
		return 4;
	}
	return 0;
}

int StudentWorld::hard_near_frack(int x, int y)
{
	int md = m_map_f[x][y];
	int M = 16 + getLevel() * 2;
	if (md > M)
		return 0;
	else
		return to_the_frackman(x, y);
}

bool StudentWorld::bribe(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++) {
		if (m_actor.at(i)->is_secondHand())
			if (pow2(m_actor.at(i)->getX() - x) + pow2(m_actor.at(i)->getY() - y) <= 9) {
				m_actor.at(i)->setDead();
				return true;
			}
	}
	return false;
}

int StudentWorld::check_game_state()
{
	if (min(2 + getLevel(), 20) - m_frackman->get_oil() == 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	else if (!m_frackman->checkAlive())
		return GWSTATUS_PLAYER_DIED;


}

/*void StudentWorld::set_map()
{
	bool check[61][61];
	for (int i = 0; i <= 60; i++)
		for (int j = 0; j <= 60; j++)
			check[i][j] = true;
	m_map.clear();
	queue<SuperCood> mq;
	SuperCood cd;
	cd.x = 60; cd.y = 60; cd.d = 0;
	mq.push(cd);
	while (!mq.empty())
	{
		cd = mq.front();
		SuperCood c;
		int x = cd.x; int y = cd.y; int d = cd.d;
		check[x][y] = false;
		
		if (x>0 && check_viable(x, y, 3) && check[x-1][y])
		{
			c.x = x - 1; c.y = y; c.d = d + 1;
			mq.push(c);
		}
		if (y>0 && check_viable(x, y, 2) && check[x][y-1])
		{
			c.x = x; c.y = y - 1; c.d = d + 1;
			mq.push(c);
		}
		if (y<60 && check_viable(x, y, 1) && check[x][y + 1])
		{
			c.x = x; c.y = y + 1; c.d = d + 1;
			mq.push(c);
		}
		if (x<60 && check_viable(x, y, 4) && check[x + 1][y])
		{
			c.x = x + 1; c.y = y; c.d = d + 1;
			mq.push(c);
		}
		
		Cood ins;
		ins.x = cd.x; ins.y = cd.y;
		m_map[ins] = cd.d;
		mq.pop();
	}
}*/

/*int StudentWorld::maze_path(int x, int y, int tx, int ty)
{
	Cood mc, tc, re;
	mc.x = x; mc.y = y;
	tc.x = tx; tc.y = ty;
	int md = m_map[mc];
	int td = m_map[tc];
	int delta = 0;
	if (md == td)
		delta = 0;
	else
		delta = 1 - 2 * (md > td);
	re.x = x + 1; re.y = y;
	if (m_map[re] == md + delta)
		return 4;
	re.x = x; re.y = y + 1;
	if (m_map[re] == md + delta)
		return 1;
	re.x = x; re.y = y - 1;
	if (m_map[re] == md + delta)
		return 2;
	re.x = x - 1; re.y = y;
	if (m_map[re] == md + delta)
		return 3;
}*/

void StudentWorld::set_map(int mymap[61][61], int tx, int ty)
{
	bool check[61][61];
	for (int i = 0; i <= 60; i++)
		for (int j = 0; j <= 60; j++)
			check[i][j] = true;

	SuperCood cd;
	cd.x = tx; cd.y = ty; cd.d = 0;
	check[tx][ty] = false;
	mq.push(cd);
	int ct = 0;
	while (!mq.empty())
	{
		cd = mq.front();
		SuperCood c;
		int x = cd.x; int y = cd.y; int d = cd.d;
		

		if (x>0 && check[x - 1][y] && check_viable(x, y, 3))
		{
			c.x = x - 1; c.y = y; c.d = d + 1;
			check[c.x][c.y] = false;
			mq.push(c);
		}
		if (y>0 && check[x][y - 1] && check_viable(x, y, 2))
		{
			c.x = x; c.y = y - 1; c.d = d + 1;
			check[c.x][c.y] = false;
			mq.push(c);
		}
		if (y<60 && check[x][y + 1] && check_viable(x, y, 1))
		{
			c.x = x; c.y = y + 1; c.d = d + 1;
			check[c.x][c.y] = false;
			mq.push(c);
		}
		if (x<60 && check[x + 1][y] && check_viable(x, y, 4))
		{
			c.x = x + 1; c.y = y; c.d = d + 1;
			check[c.x][c.y] = false;
			mq.push(c);
		}
		
		Cood ins;
		ins.x = cd.x; ins.y = cd.y;
		mymap[x][y] = cd.d;
		mq.pop();
	}
}



int StudentWorld::to_the_end(int x, int y)
{
	int md = m_map[x][y];

	if (m_map[x+1][y] == md - 1)
		return 4;

	else if (m_map[x][y+1] == md - 1)
		return 1;

	else if (m_map[x][y-1] == md - 1)
		return 2;

	else if (m_map[x - 1][y] == md - 1)
		return 3;
	else
		return 0;
}

int StudentWorld::to_the_frackman(int x, int y)
{
	int md = m_map_f[x][y];

	if (m_map_f[x + 1][y] == md - 1)
		return 4;

	else if (m_map_f[x][y + 1] == md - 1)
		return 1;

	else if (m_map_f[x][y - 1] == md - 1)
		return 2;

	else if (m_map_f[x - 1][y] == md - 1)
		return 3;
	else return 0;
}



