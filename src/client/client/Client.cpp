#include "engine.h"
#include "render.h"
#include "state.h"
#include "client.h"
#include "ai.h"
#include <iostream>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <pthread.h>
using namespace state;
using namespace engine;
using namespace render;
using namespace std;
using namespace client;
using namespace ai;

bool aux1 = false;
bool aux2 = true;
void threadEngine(Engine *ptr)
{
    while (aux2) // will be changed by the thread
    {
        usleep(1000);
        if (aux1)
        {
            ptr->update();
            aux1 = false;
        }
    }
}

Client::Client(sf::RenderWindow &window, std::string mode) : window(window), engine("game")
{
    this->mode = mode;

    std::string map_path = (mode == "test") ? "../../../res/map_v0.txt" : "res/map_v0.txt";
    engine.getState().initializeMapCell(map_path);
    engine.getState().initializeCharacters();
    aiTeamA = new DeepAI(engine, 1); // because by default is 2
    aiTeamB = new DeepAI(engine);
    engine.registerObserver(this);
    engine.multithread = true;
}

void Client::engineUpdating()
{
    aux1 = true;
    usleep(150000);
}

void Client::engineUpdated()
{
}
bool once = true;
void Client::run()
{
    StateLayer stateLayer(engine.getState(), window);
    stateLayer.initSurfaces(engine.getState());

    StateLayer *ptr_stateLayer = &stateLayer;
    engine.getState().registerObserver(ptr_stateLayer);
    // stateLayer.registerObserver(&engine);
    sf::Music backMusic;
    std::string music = (mode == "test") ? "../../../res/epic_music.wav" : "res/epic_music.wav";
    if (backMusic.openFromFile(music))
    {
        backMusic.setVolume(40);
        backMusic.setLoop(true);
        backMusic.play();
    }
    std::thread th(threadEngine, &engine);
    while (!engine.getState().getEnd())
    {
        if (once)
        {
            stateLayer.draw(window);
            once = false;
        }

        aiTeamA->run(engine);
        aiTeamB->run(engine);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                engine.getState().setEnd(true);
                cout << "\tWindow closed" << endl;
                break;
            }
        }
    }
    aux2 = false;
    th.join();
}

std::string Client::getMode() const
{
    return mode;
}
