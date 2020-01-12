#include "engine.h"
#include "render.h"
#include "state.h"
#include "client.h"
#include "ai.h"
#include <iostream>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>

using namespace state;
using namespace engine;
using namespace render;
using namespace std;
using namespace client;
using namespace ai;

bool l1 = false;
bool l2 = true;
void threadEngine (void* engine)
{
	Engine* ptrEngine=(Engine*)engine;
    while (l2)
    {
        if (l1)
        {
            ptrEngine->update();
            l1 = false;
        }
    }
}

NetworkClient::NetworkClient(std::string &url, int port, int character, sf::RenderWindow &window, std::string mode) : url(url), port(port), character(character), window(window), mode(mode), engine("game")
{

    std::string map_path = (mode == "test") ? "../../../res/map_v0.txt" : "res/map_v0.txt";
    engine.getState().initializeMapCell(map_path);
    engine.getState().initializeCharacters();

    engine.registerObserver(this);
    engine.multithread = true;

    playerAI = new DeepAI(engine, character);
    displayFirstTime = true;
    
    engine.registerObserver(this);
}

std::string NetworkClient::getGameStatus()
{
    sf::Http http(url, port);
    sf::Http::Request players0;
    players0.setMethod(sf::Http::Request::Get);
    players0.setUri("/game");
    players0.setHttpVersion(1, 0);
    sf::Http::Response playersResp0 = http.sendRequest(players0);
    Json::Reader jsonReaderPlayers0;
    Json::Value jsonPlayers0;
    jsonReaderPlayers0.parse(playersResp0.getBody(), jsonPlayers0);
    return (jsonPlayers0["status"].asUInt() == 1) ? "CREATING" : "RUNNING";
}

bool NetworkClient::getServerCommands(Json::Value &out)
{
    Position pos(0, 0);
    for (unsigned int i = 0; i < out["commands"].size(); i++)
    {
        int commandId = out["commands"][i]["id"].asUInt();
        if (commandId == engine::MOVE)
        {
            pos.setX(out["commands"][i]["xDestination"].asUInt());
            pos.setY(out["commands"][i]["yDestination"].asUInt());

            MoveCommand moveComm(*engine.getState().getCharacters()[out["commands"][i]["target"].asUInt()], pos);
            unique_ptr<Command> ptr_move(new MoveCommand(moveComm));
            engine.addCommand(move(ptr_move));

            l1 = true;
            usleep(150000);
        }
        else if (commandId == engine::ATTACK)
        {
            AttackCommand attack(*engine.getState().getCharacters()[out["commands"][i]["attacker"].asUInt()], *engine.getState().getCharacters()[out["commands"][i]["target"].asUInt()]);
            unique_ptr<Command> ptr_attack(new AttackCommand(attack));
            engine.addCommand(move(ptr_attack));

            l1 = true;
            usleep(150000);
        }
        else if (commandId == engine::SELECT_CHARACTER)
        {
            SelectCharacterCommand select(*engine.getState().getCharacters()[out["commands"][i]["target"].asUInt()]);
            unique_ptr<Command> ptr_select(new SelectCharacterCommand(select));
            engine.addCommand(move(ptr_select));

            l1 = true;
            usleep(150000);
        }
        else if (commandId == engine::FINISH_TURN)
        {
            unique_ptr<Command> ptr_finturn(new FinishTurnCommand());
            engine.addCommand(move(ptr_finturn));

            l1 = true;
            usleep(150000);
        }
        else
        {
            cout << "Command id " << i << " unknown" << endl;
        }
    }
    return true;
}

void NetworkClient::putServerCommand(engine::Command *c)
{
    sf::Http http(url, port);
    sf::Http::Request request1;
    request1.setMethod(sf::Http::Request::Post);
    request1.setUri("/command");
    request1.setHttpVersion(1, 0);
    string body = c->serialize().toStyledString();
    request1.setBody(body);
    sf::Http::Response response = http.sendRequest(request1);
}

void NetworkClient::run()
{
    StateLayer stateLayer(engine.getState(), window);
    stateLayer.initSurfaces(engine.getState());

    StateLayer *ptr_stateLayer = &stateLayer;
    engine.getState().registerObserver(ptr_stateLayer);
    stateLayer.draw(window);
    std::thread th(threadEngine, &engine);
    while (!engine.getState().getEnd())
    {
        if (!engine.getState().getEnd())
        {
            StateEvent e(ALLCHANGED);
            engine.getState().notifyObservers(e, engine.getState());
        }
        if (displayFirstTime)
        {
            stateLayer.draw(window);
            cout << "Turn " << engine.getState().getTurn() << endl;
            displayFirstTime = false;
        }
        playerAI->setPlayerNumber(character);
        if (engine.getState().getTurnOwner() == playerAI->getPlayerNumber()) // is my turn
        {
            // play
            playerAI->run(engine);
        }
        else
        {
            // get commands from opponent passing my player number (1 or 2)
            sleep(3);
            sf::Http http(url, port);
            sf::Http::Request request;
            request.setMethod(sf::Http::Request::Get);
            string uri = "/command";
            request.setUri(uri);
            request.setHttpVersion(1, 0);
            Json::Reader jsonReader;
            Json::Value commands;

            sf::Http::Response response = http.sendRequest(request);
            cout << response.getBody() << endl;
            if (jsonReader.parse(response.getBody(), commands))
            {
                // get and apply opponent commands
                getServerCommands(commands);
            }
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            // Fermeture de la fenetre
            if (event.type == sf::Event::Closed)
            {
                window.close();
                engine.getState().setEnd(true);
                cout << "\tWindow closed" << endl;
                break;
            }
        }
    }
    l2 = false;
    th.join();
}

sf::RenderWindow& NetworkClient::getWindow(){
    return window;
}
void NetworkClient::engineUpdated(){}

void NetworkClient::engineUpdating()
{
    for (size_t i = 0; i < engine.getCurrentCommands().size(); i++)
    {
        Command *c = &(*engine.getCurrentCommands()[i]);
        putServerCommand(c);
    }
    cout << "engine updating" << endl;
    engine.update();
    usleep(150000);
}
