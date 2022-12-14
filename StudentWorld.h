#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <iostream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
    public:
        StudentWorld(std::string assetPath);
        virtual int init();
        virtual int move();
        virtual void cleanUp(); // basically the destructor of StudentWorld
        ~StudentWorld();
        GhostRacer* getGhost(); // get pointer to ghostracer for other actors to affect ghostracer
        int getSouls();
        void soulSaved();
        void setLastY(int y);
        std::vector<Actor*>* getActors(); // returns pointer to actor vector
        Actor* findBottomActor(int left, int right);
        Actor* findTopActor(int left, int right);
    private:
        GhostRacer* ghostracer;
        std::vector<Actor*> actors;
        int soulsToSave;
        double lastY;
        int bonus;

};

#endif // STUDENTWORLD_H_
