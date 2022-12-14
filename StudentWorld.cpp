#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "StudentWorld.h" // overworld stuff
#include "GameConstants.h"

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}


// helper determines if given actor randomly spawns
bool chanceGen(int a, int b)
{
    int random = randInt(0, max(a, b));
    return random == 0;
}


int StudentWorld::init()
{
    // initialize level complete params and the ghost racer

    bonus = 5000;
    soulsToSave = getLevel() * 2 + 5;
    ghostracer = new GhostRacer(this, IID_GHOST_RACER, 128, 32, 90, 4.0, 0);

    // add yellow border lines

    const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;

    for (int i = 0; i < VIEW_HEIGHT / SPRITE_HEIGHT; i++)
    {
        BorderLine* leftYellow = new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT, 0, 2.0, 2, 0, -4, false); 
        BorderLine* rightYellow = new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT, 0, 2.0, 2, 0, -4, false);
        actors.push_back(leftYellow);
        actors.push_back(rightYellow);
    }

    // add white border lines

    for (int i = 0; i < VIEW_HEIGHT / (4*SPRITE_HEIGHT); i++)
    {
        BorderLine* leftWhite = new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, i * SPRITE_HEIGHT * 4, 0, 2.0, 2, 0, -4, false);
        BorderLine* rightWhite = new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, i * SPRITE_HEIGHT * 4, 0, 2.0, 2, 0, -4, false);
        lastY = rightWhite->getY();
        actors.push_back(leftWhite);
        actors.push_back(rightWhite);
    }

    return GWSTATUS_CONTINUE_GAME;
}

//=========================================================== student move ==========================================
int StudentWorld::move()
{

    const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    const double LANE_WIDTH = ROAD_WIDTH / 3;

    // ========================== add new actors ======================
    
    if (chanceGen(200 - getLevel() * 10, 30)) //gen human ped
    {
        HumanPedestrian* humanPed = new HumanPedestrian(this, IID_HUMAN_PED, randInt(0, VIEW_WIDTH), VIEW_HEIGHT, 0, 2.0, 0, 0, -4, true);
        actors.push_back(humanPed);
    }
        
    if (chanceGen(100 - getLevel() * 10, 20)) //gen zombie ped
    {
        ZombiePedestrian* zombiePed = new ZombiePedestrian(this, IID_ZOMBIE_PED, randInt(0, VIEW_WIDTH), VIEW_HEIGHT, 0, 3.0, 0, 0, -4, true);
        actors.push_back(zombiePed);
    }

    if (chanceGen(150 - getLevel() * 10, 40)) //gen oil slick goodie
    {
        OilSlick* oilSlick = new OilSlick(this, IID_OIL_SLICK, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, randInt(2, 5), 2, 0, -4, false);
        actors.push_back(oilSlick);
    }
        
    if (chanceGen(100, 100)) // gen lost soul goodie
    {
        Soul* soul = new Soul(this, IID_SOUL_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, 4.0, 2, 0, -4, false);
        actors.push_back(soul);
    }
        
    if (chanceGen(100 + 10 * getLevel(), 100 + 10 * getLevel())) //gen holy water refill goodie
    {

        HolyWaterRefill* refill = new HolyWaterRefill(this, IID_HOLY_WATER_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 90, 2.0, 2, 0, -4, false);
        actors.push_back(refill);
    }
        
    if (chanceGen(100 - getLevel() * 10, 20)) //gen zombie cab
    {
        int lane = randInt(1, 3);
        bool lane1 = false, lane2 = false, lane3 = false;
        bool generated = false;

        // starting lane for attempt at zombie cab generation 

        switch (lane)
        {
            case 1: // lane 1
                lane1 = true;
                lane2 = false;
                lane3 = false;

                // check if generate at bottom is viable
                
                if (findBottomActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH) == nullptr ||
                    findBottomActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH)->getY() > VIEW_HEIGHT / 3)
                {
                    //std::cout << "\ngenerating bottom\n";
                    ZombieCab* zombieCab1 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH/3, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 1);
                    actors.push_back(zombieCab1);
                    generated = true;
                    break;
                }

                // check if generate at top is viable

                if (findTopActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH) == nullptr ||
                    findTopActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH)->getY() < VIEW_HEIGHT*2 / 3)
                {
                    //std::cout << "\ngenerating top\n";
                    ZombieCab* zombieCab2 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH / 3, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 1);
                    actors.push_back(zombieCab2);
                    generated = true;
                }

                break;
            case 2: // lane 2
                lane1 = false;
                lane2 = true;
                lane3 = false;
                
                // check if generate at bottom is viable

                if (findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr || 
                    findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() > VIEW_HEIGHT / 3)
                {
                    /*
                    std::cout << "ghostracer: " << ghostracer << " bottom: " << findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) << std::endl;
                    if(findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) != nullptr)
                        std::cout << "bottomY: " << findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() << std::endl;
                    */

                    ZombieCab* zombieCab3 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 2);
                    actors.push_back(zombieCab3);
                    generated = true;
                    break;
                }

                // check if generate at top is viable

                if (findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr ||
                    findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() < VIEW_HEIGHT*2 / 3)
                {
                    ZombieCab* zombieCab4 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 2);
                    actors.push_back(zombieCab4);
                    generated = true;
                }

                break;
            case 3: // lane 3
                lane1 = false;
                lane2 = false;
                lane3 = true;
                
                // check if generate at bottom is viable

                if (findBottomActor(RIGHT_EDGE - LANE_WIDTH, RIGHT_EDGE) == nullptr ||
                    findBottomActor(RIGHT_EDGE - LANE_WIDTH, RIGHT_EDGE)->getY() > VIEW_HEIGHT / 3)
                {
                    ZombieCab* zombieCab5 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 3);
                    actors.push_back(zombieCab5);
                    generated = true;
                    break;
                }

                // check if generate at top is viable

                if (findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr ||
                    findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() < VIEW_HEIGHT * 2 / 3)
                {
                    ZombieCab* zombieCab6 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 3);
                    actors.push_back(zombieCab6);
                    generated = true;
                }

                break;
        }

        // check other lanes for viable generations of zombie cab if first one didn't work

        if (!lane1 && !generated)
        {
            // check if generate at bottom is viable
            
            if (findBottomActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH) == nullptr ||
                findBottomActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH)->getY() > VIEW_HEIGHT / 3)
            {
                //std::cout << "\nuh oh bottom\n";
                ZombieCab* zombieCab7 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 1);
                actors.push_back(zombieCab7);
                generated = true;
            }

            // check if generate at top is viable

            if (!generated && findTopActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH) == nullptr ||
                findTopActor(LEFT_EDGE, LEFT_EDGE + LANE_WIDTH)->getY() < VIEW_HEIGHT * 2 / 3)
            {
                //std::cout << "\nuh oh top\n";
                ZombieCab* zombieCab8 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH / 3, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 1);
                actors.push_back(zombieCab8);
                generated = true;
            }
        }
        
        if (!lane2 && !generated)
        {
            // check if generate at bottom is viable

            if (findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr ||
                findBottomActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() > VIEW_HEIGHT / 3)
            {
                ZombieCab* zombieCab9 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 2);
                actors.push_back(zombieCab9);
                generated = true;
               
            }

            // check if generate at top is viable

            if (!generated && findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr ||
                findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() < VIEW_HEIGHT * 2 / 3)
            {
                ZombieCab* zombieCab10 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 2);
                actors.push_back(zombieCab10);
                generated = true;
            }
        }

        if (!lane3 && !generated)
        {
            // check if generate at bottom is viable

            if (findBottomActor(RIGHT_EDGE - LANE_WIDTH, RIGHT_EDGE) == nullptr ||
                findBottomActor(RIGHT_EDGE - LANE_WIDTH, RIGHT_EDGE)->getY() > VIEW_HEIGHT / 3)
            {
                ZombieCab* zombieCab11 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() + randInt(2, 4), true, 3);
                actors.push_back(zombieCab11);
                generated = true;
                
            }

            // check if generate at top is viable

            if (!generated && findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH) == nullptr ||
                findTopActor(LEFT_EDGE + LANE_WIDTH, RIGHT_EDGE - LANE_WIDTH)->getY() < VIEW_HEIGHT * 2 / 3)
            {
                ZombieCab* zombieCab12 = new ZombieCab(this, IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, VIEW_HEIGHT - SPRITE_HEIGHT / 2, 90, 4.0, 0, 0, ghostracer->getSpeedY() - randInt(2, 4), true, 3);
                actors.push_back(zombieCab12);
                generated = true;
            }
        }
        
    }
        
    
    
    //========================= add new borders ================================

    const double borderY = VIEW_HEIGHT - SPRITE_HEIGHT;
    const double deltaY = borderY - lastY;
    
    if(deltaY >= SPRITE_HEIGHT) // add yellow borders
    {
        BorderLine* leftYellow = new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH/2, borderY, 0, 2.0, 2, 0, -4, false); 
        BorderLine* rightYellow = new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, borderY, 0, 2.0, 2, 0, -4, false);
        actors.push_back(leftYellow);
        actors.push_back(rightYellow);
    }

    if (deltaY >= 4 * SPRITE_HEIGHT) // add white borders
    {
        BorderLine* leftWhite = new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, borderY, 0, 2.0, 2, 0, -4, false);
        BorderLine* rightWhite = new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, borderY, 0, 2.0, 2, 0, -4, false);
        lastY = rightWhite->getY();
        actors.push_back(leftWhite);
        actors.push_back(rightWhite);
    }
    


    // make ghost racer doSomething if it's alive

    if (ghostracer->isAlive())
        ghostracer->doSomething();
    else
    {
        // ghost racer died 

        decLives();
        delete ghostracer;
        return GWSTATUS_PLAYER_DIED;
    }

    vector<Actor*>::iterator it;
    it = actors.begin();

    // ==================================== make all Actors doSomething() ===========================

    while (it != actors.end())
    {
        if ((*it)->isAlive())
            (*it)->doSomething();
        else
        {
            // delete actor if it's not alive

            delete (*it);
            it = actors.erase(it);
            continue;
        }

        // if ghost racer died by any of the actors, level is over

        if (!ghostracer->isAlive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        it++;
    }

    //================= update game status text =============================
    
    bonus--; // bonus goes down by 1 per tick
    
    ostringstream oss;
    oss.setf(ios::fixed);
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << getSouls() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << ghostracer->getHP() << "  ";
    oss << "Sprays: " << ghostracer->getSprays() << "  ";
    oss << "Bonus: " << bonus;
    //oss << "SS: " << ghostracer->getSpeedY(); // for debugging ghostracer's speed
    string status = oss.str();

    //cout << "Ghostracer (x, y): " << ghostracer->getX() << " , " << ghostracer->getY() << endl;

    setGameStatText(status);

    // check if level finished

    if (soulsToSave == 0)
    {
        increaseScore(bonus);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    // otherwise continue game assuming Ghost Racer still alive as checked initially in this function
   
    return GWSTATUS_CONTINUE_GAME;
}

//=============================================== other studentWorld functions =========================================

void StudentWorld::cleanUp()
{
    // delete ghost racer and iterate through all actors, deleting them 

    delete ghostracer;

    vector<Actor*>::iterator it;
    it = actors.begin();

    while (it != actors.end())
    {
        delete (*it);
        it = actors.erase(it);
    }
}

//==================================== start of find actor functions ============================================
Actor* StudentWorld::findBottomActor(int left, int right)
{
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    bool initBottom = false;
    Actor* bottom = nullptr;

    // iterate through all actors and check if x is within left and right bounds of lane

    while (it != actors.end())
    {
        if ((*it)->canCollide())
        {
            if ((*it)->getX() >= left && (*it)->getX() < right)
            {
                if (!initBottom)
                {
                    bottom = *it;
                    initBottom = true;
                }

                // update bottom to actor within lane with lowest Y

                if ((*it)->getY() < bottom->getY())
                {
                    bottom = *it;
                }
                    
            }
        }

        it++;
    }

    // check to see if ghostracer is bottommost actor in this lane

    if (bottom != nullptr && ghostracer->getX() >= left && ghostracer->getX() < right && ghostracer->getY() < bottom->getY())
        bottom = ghostracer;
    else if (bottom == nullptr && ghostracer->getX() >= left && ghostracer->getX() < right)
        bottom = ghostracer;

    //std::cout << "Ghostracer: " << ghostracer << " bottom: " << bottom << std::endl << std::endl;

    return bottom;
}

// like findBottomActor for actors closest to the top
Actor* StudentWorld::findTopActor(int left, int right)
{
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    bool initTop = false;
    Actor* top = nullptr;

    while (it != actors.end())
    {
        if ((*it)->canCollide())
        {
            if ((*it)->getX() >= left && (*it)->getX() < right)
            {
                if (!initTop)
                {
                    top = *it;
                    initTop = true;
                }

                if ((*it)->getY() > top->getY())
                    top = *it;
            }
        }
        it++;
    }

    // update top to be ghostracer if ghostY is > current topY

    if (top != nullptr && ghostracer->getX() >= left && ghostracer->getX() < right && ghostracer->getY() > top->getY())
        top = ghostracer;

    return top;
}
//==================================== end of find actor functions ============================================


StudentWorld::~StudentWorld()
{
    cleanUp();
}

GhostRacer* StudentWorld::getGhost()
{
    return ghostracer;
}

int StudentWorld::getSouls()
{
    return soulsToSave;
}

void StudentWorld::soulSaved()
{
    soulsToSave--;
}

void StudentWorld::setLastY(int y)
{
    lastY = y;
}

std::vector<Actor*>* StudentWorld::getActors()
{
    return &actors;
    // use *getActors() in subclasses to get actors* vector
}
