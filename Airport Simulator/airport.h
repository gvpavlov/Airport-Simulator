#ifndef _AIRPORT_H_
#define _AIRPORT_H_

#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <random>
using namespace std;


enum PlaneStatus {NoPlane, TakingOff = -1, Landed = 1};

struct Airplane
{
	int spawnTime;
	Airplane(const int turn = 0) : spawnTime(turn) {}
};

struct Runway
{
	bool occupied;
	PlaneStatus plane;
	int occupiedTime;
	Runway() : occupied(false), plane(NoPlane),
				occupiedTime(0) {}
};

struct Statistics
{
	int landedPlanes;
	int takenoffPlanes;
	int deniedPlanes;
	vector<int> averageLandingPlanes;
	vector<int> averageLandingWaitTime;
	vector<int> averageTakingOffPlanes;
	vector<int> averageTakingOffWaitTime;
	Statistics() : landedPlanes(0), 
				   takenoffPlanes(0),
				   deniedPlanes(0) {}
	void doMath(const vector<int>&, float&, float&); 
	void print();
};

class Airport
{
private:
	queue<Airplane> landing, takeoff;
	vector<Runway> runways;
	Statistics stats;
	int time;
	int turn;
	int planesPerTurn;
	void spawnPlanes();
	void organization();
	void runwayReset();
	void printStats();
	void firstAct();
	void secondAct();
	void thirdAct();
	void lastAct();
	void lastActHelper(int&);
	void printRunways();
	void print();
	void finalizeStats();
public:
	Airport(const int, const int, const int);
	void simulation();
};

#endif