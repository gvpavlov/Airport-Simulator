#include "airport.h"

Airport::Airport(const int time, const int runwayCount, const int planes)
{
	this->time = time;
	this->planesPerTurn = planes;
	turn = 1;
	for(int i = 0; i < runwayCount; i++)
		runways.push_back(Runway());
}

void Statistics::doMath(const vector<int>& stats, float& max, float& average)
{
	// Accumulates the averages and finds the max from every vector.
	for(int i = 0; i < stats.size(); i++)
	{
		if(max < stats[i])
			max = stats[i];
		average += stats[i];
	}
	average /= stats.size();
}

void Statistics::print()
{
	cout << "======================================" << endl;
	cout << "              STATISTICS              " << endl;
	cout << "======================================" << endl;
	cout << "Landed planes: " << landedPlanes << endl;
	cout << "Taken-off planes: " << takenoffPlanes << endl;
	cout << "Denied planes: " << deniedPlanes << endl;
	cout << "-------------------------------------" << endl;

	float mp = 0, ap = 0;
	doMath(averageLandingPlanes, mp, ap);
	cout << "Average landing planes: " << ap << endl;
	cout << "Max landing planes: " << mp << endl;

	float awt = 0, mwt = 0;
	doMath(averageLandingWaitTime, mwt, awt);
	cout << "Average landing wait time: " << awt << endl;
	cout << "Max landing wait time: " << mwt << endl;
	cout << "-------------------------------------" << endl;

	doMath(averageTakingOffPlanes, mp, ap);
	cout << "Average taking-off planes: " << ap << endl;
	cout << "Max taking-off planes: " << mp << endl;

	doMath(averageTakingOffWaitTime, mwt, awt);
	cout << "Average takeoff wait time: " << awt << endl;
	cout << "Max takeoff wait time: " << mwt << endl;
	cout << "-------------------------------------" << endl;
}

void Airport::spawnPlanes()
{
	random_device random;
	int n = random() % (planesPerTurn + 1);
	stats.averageLandingPlanes.push_back(0);
	stats.averageTakingOffPlanes.push_back(0);
	for(int i = 0 ; i < n; i++)
	{
		Airplane temp(turn);
		if(random() % 2)
		{
			landing.push(temp);
			stats.averageLandingPlanes.back()++;
		}
		else
		{
			takeoff.push(temp);
			stats.averageTakingOffPlanes.back()++;
		}
	}
}

void Airport::firstAct()
{
	// First remove planes which have landed in the previous turn.
	// The plane's runway and those before it are closed for the turn
	// (unless there is another plane which is waiting to be removed)

	// Blocks all previous runways for further use in the same turn
	// by finding the last landed plane.
	bool stopper = true;
	for(int i = runways.size() - 1; i >= 0 && stopper; i--)
	{
		if(runways[i].plane == Landed)
		{
			for(int j = 0; j <= i; j++)
				runways[j].occupied = true;
			stopper = false;
		}
	}

	bool empty = true;
	for(int i = 0; i < runways.size() && empty; i++)
	{
		if(runways[i].plane == TakingOff)
			// If there is a plane blocking this stops others from
			// clearing the following runways.
			empty = false;
		else if(runways[i].plane == Landed)
			// Otherwise it removes the plane.
			runways[i].plane = NoPlane;
	}
}

void Airport::secondAct()
{
	// Secondly, land planes (if possible) on the last available runway.
	for(int i = runways.size() - 1; i >= 0 && !(landing.empty()); i--)
	{
		if(!runways[i].occupied)
		{
			stats.landedPlanes++;
			if(landing.front().spawnTime != turn)
				stats.deniedPlanes++;
			stats.averageLandingWaitTime.push_back(turn - landing.front().spawnTime); 
			landing.pop();
			runways[i].occupied = true;
			runways[i].plane = Landed;
		}
	}
}

void Airport::thirdAct()
{
	// Next - remove planes waiting to fly away.
	for(int i = 0; i < runways.size(); i++)
	{
		if(runways[i].plane == TakingOff)
			runways[i].plane = NoPlane;
	}
}

void Airport::lastActHelper(int& i)
{
	if(takeoff.front().spawnTime != turn)
		stats.deniedPlanes++;
	stats.averageTakingOffWaitTime.push_back(turn - takeoff.front().spawnTime);
	stats.takenoffPlanes++;
	takeoff.pop();
	runways[i].occupied = true;
	runways[i].plane = TakingOff;
	// Resets the index so the search can start anew.
	i = 0;
}

void Airport::lastAct()
{
	// Last - guide planes for takeoff to the last empty runway if 
	// there are no more landing planes in this turn.
	if(landing.empty())
	{
		// 1. When a place for the plane is found it restarts
		// the search from the first runway.
		// 2. It searches until it hits an occupied runway, then
		// it must go back a step and place the plane on the previous.
		// 2* Edgecases
		// 2.1. if the first is occupied it will block the rest.
		// 2.2. if no runways are occupied it will place on last track.
		bool blocked = false;
		for(int i = 0; i < runways.size() && (!takeoff.empty()) && (!blocked); i++)
		{
			if(runways[i].occupied)
			{
				if(i == 0)
					blocked = true;
				else
					lastActHelper(i);
			}
			else if(i == (runways.size() - 1))
				lastActHelper(i);
		}
	}
}

void Airport::organization()
{
	firstAct();
	secondAct();
	thirdAct();
	lastAct();
}

void Airport::runwayReset()
{
	// Reset the status of the runways which have been temporarily
	// blocked during this turn.
	for(int i = 0; i < runways.size(); i++)
	{
		if(runways[i].occupied)
			runways[i].occupiedTime++;
		if(runways[i].plane == NoPlane)
			runways[i].occupied = false;
	}
}

void Airport::printRunways()
{
	cout << "Percentage of time in which each runway was free:" << endl;
	for(int i = 0; i < runways.size(); i++)
	{
		int percent = ((time - runways[i].occupiedTime) / (float)time) * 100;
		cout << i + 1 << ". " << percent << "%" << endl;
	}
}

void Airport::finalizeStats()
{
	while(!landing.empty())
	{
		if(landing.front().spawnTime != turn)
			stats.deniedPlanes++;
		stats.averageLandingWaitTime.push_back(turn - landing.front().spawnTime); 
		landing.pop();
	}
	while(!takeoff.empty())
	{
		if(takeoff.front().spawnTime != turn)
			stats.deniedPlanes++;
		stats.averageTakingOffWaitTime.push_back(turn - takeoff.front().spawnTime);
		takeoff.pop();
	}
}

void Airport::print()
{
	finalizeStats();
	cout << "Airport with " << runways.size() << " runways for " 
		<< time << " turns" << " with " << planesPerTurn << 
		" planes landing per turn." << endl;
	stats.print();
	printRunways();
	cout << endl;
}

void Airport::simulation()
{
	int tmpTime = time;
	while(tmpTime)
	{
		spawnPlanes();
		organization();
		runwayReset();
		tmpTime--; 
		turn++;
	}
	print();
}
