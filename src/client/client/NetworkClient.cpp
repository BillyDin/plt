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
// STATIC FUNCTIONS HERE
void NetworkClient::registerAsReady(sf::Http &http, int idPlayer)
{
    sf::Http::Request request1;
    request1.setMethod(sf::Http::Request::Post);
    request1.setUri("/player");
    request1.setHttpVersion(1, 0);

    Json::Value me;
    me["free"] = false;
    me["player_id"] = idPlayer;

    request1.setBody(me.toStyledString());

    sf::Http::Response response1 = http.sendRequest(request1);
}
void NetworkClient::runMultiplayer(NetworkClient& client){
    while (client.getWindow().isOpen())
    {
        client.run();
        cout << "closing in 10 secs" << endl;
        sleep(10);
        client.getWindow().close();
    }
}
void NetworkClient::runAsPlayer(int player, int idPlayer, sf::Http& http, string url, int port)
{
    usleep(100000);
    if (player == 1)
    {
        sf::RenderWindow window(sf::VideoMode((25 * 32) + 256, (20 * 32) + 32, 32), ("GameName - Player " + to_string(player) + ((player == 1) ? " Blue" : " Red")));
        NetworkClient net_client{url, port, player, window, "game"};
        registerAsReady(http, idPlayer);
        sleep(4); // some secs to wait p2
        runMultiplayer(net_client);
        exitFromLobby(http, idPlayer);
    }
    else if (player == 2)
    {
        sf::RenderWindow window(sf::VideoMode((25 * 32) + 256, (20 * 32) + 32, 32), ("GameName - Player " + to_string(player) + ((player == 1) ? " Blue" : " Red")));
        NetworkClient net_client{url, port, player, window, "game"};
        registerAsReady(http, idPlayer);
        runMultiplayer(net_client);
        exitFromLobby(http, idPlayer);
    }
}
bool NetworkClient::canRunMultiplayer(sf::Http& http){
    sf::Http::Request req;
    req.setMethod(sf::Http::Request::Get);
    req.setUri("/game");
    req.setHttpVersion(1, 0);
    sf::Http::Response resp = http.sendRequest(req);
    Json::Reader reader;
    Json::Value out;
    reader.parse(resp.getBody(), out);
    bool result = (out["status"].asString() == "2");
    if (result) {
        cout << "starting game ... " << endl;
    } else {
        cout << "waiting for players ... " << endl;
    }
    return result;// 1 => creating, 2 => running
}
int NetworkClient::getPlayerNumberOnServer(sf::Http& http, int id){
    sf::Http::Request req;
    req.setMethod(sf::Http::Request::Get);
    req.setUri("/player/" + to_string(id));
    req.setHttpVersion(1, 0);
    sf::Http::Response resp = http.sendRequest(req);
    Json::Reader reader;
    Json::Value out;
    reader.parse(resp.getBody(), out);
    return out["playerNumber"].asInt();
}
Json::Value NetworkClient::getPlayersOnServer(sf::Http &http)
{
    // query array of players in lobby.
    sf::Http::Request players;
    players.setMethod(sf::Http::Request::Get);
    players.setUri("/player");
    players.setHttpVersion(1, 0);
    sf::Http::Response resp = http.sendRequest(players);
    Json::Reader reader;
    Json::Value out;
    reader.parse(resp.getBody(), out);
    return out;
}
void NetworkClient::exitFromLobby(sf::Http& http, int idPlayer){
    sf::Http::Request request3;
    request3.setMethod(sf::Http::Request::Delete);
    string uri2 = "/player/" + to_string(idPlayer);
    request3.setUri(uri2);
    request3.setHttpVersion(1, 0);
    http.sendRequest(request3);
    cout << "Player " << idPlayer << " deleted" << endl;

    Json::Value jsonPlayers = getPlayersOnServer(http);

    cout << "Players in the lobby: (" << jsonPlayers["players"].size() << "/2)" << endl;
    for (auto &playerStillInLobby : jsonPlayers["players"])
    {
        cout << "\t-" << playerStillInLobby[1].asString() << " [id: " << playerStillInLobby[0].asString() << "]" << endl;
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
            usleep(100000);
        }
        else if (commandId == engine::ATTACK)
        {
            AttackCommand attack(*engine.getState().getCharacters()[out["commands"][i]["attacker"].asUInt()], *engine.getState().getCharacters()[out["commands"][i]["target"].asUInt()]);
            unique_ptr<Command> ptr_attack(new AttackCommand(attack));
            engine.addCommand(move(ptr_attack));
            l1 = true;
            usleep(100000);
        }
        else if (commandId == engine::SELECT_CHARACTER)
        {
            SelectCharacterCommand select(*engine.getState().getCharacters()[out["commands"][i]["target"].asUInt()]);
            unique_ptr<Command> ptr_select(new SelectCharacterCommand(select));
            engine.addCommand(move(ptr_select));
            l1 = true;
            usleep(100000);
        }
        else if (commandId == engine::FINISH_TURN)
        {
            unique_ptr<Command> ptr_finturn(new FinishTurnCommand());
            engine.addCommand(move(ptr_finturn));
            l1 = true;
            usleep(100000);
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
        if (engine.getState().getTurnOwner() == character) // is my turn
        {
            // play
            playerAI->run(engine);
            sleep(2);
        }
        else
        {
            // while is not my turn, wait
            sf::Http http(url, port);
            sleep(1);
            // get commands from opponent passing my player number (1 or 2)
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

// bool canIGetCommands(sf::Http &http){
//     sf::Http::Request req;
//     req.setMethod(sf::Http::Request::Get);
//     req.setUri("/command/1"); // consult if can i get commands endpoint
//     req.setHttpVersion(1, 0);
//     usleep(10000);
//     sf::Http::Response resp = http.sendRequest(req);
//     Json::Reader reader;
//     Json::Value jsonValue;
//     reader.parse(resp.getBody(), jsonValue);
//     return (jsonValue["canGet"].asUInt() == 1) ? true : false;
// }
// bool isPlayerOneReady(sf::Http &http){
//     sf::Http::Request req;
//     req.setMethod(sf::Http::Request::Get);
//     req.setUri("/game/1");
//     req.setHttpVersion(1, 0);
//     sleep(2);
//     sf::Http::Response resp = http.sendRequest(req);
//     Json::Reader reader;
//     Json::Value out;
//     reader.parse(resp.getBody(), out);
//     if(out["ready"].asInt() == 1) return true;
//     return false;
// }
// bool isPlayerTwoReady(sf::Http &http){
//     sf::Http::Request req;
//     req.setMethod(sf::Http::Request::Get);
//     req.setUri("/game/2");
//     req.setHttpVersion(1, 0);
//     sleep(2);
//     sf::Http::Response resp = http.sendRequest(req);
//     Json::Reader reader;
//     Json::Value out;
//     reader.parse(resp.getBody(), out);
//     if(out["ready"].asInt() == 1) return true;
//     return false;
// }