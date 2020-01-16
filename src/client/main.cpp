#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <map>
#include <memory>
#include <unistd.h>
#include <thread>
//#include <microhttpd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <state.h>
#include "render.h"
#include "engine.h"
#include "ai.h"
#include "client.h"

using namespace std;
using namespace state;
using namespace render;
using namespace client;
using namespace ai;

bool running = false;

void imReady(sf::Http &http, int idPlayer)
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

bool canStart(sf::Http &http){
    sf::Http::Request req;
    req.setMethod(sf::Http::Request::Get);
    req.setUri("/game/1");
    req.setHttpVersion(1, 0);
    sleep(2);
    sf::Http::Response resp = http.sendRequest(req);
    Json::Reader reader;
    Json::Value out;
    reader.parse(resp.getBody(), out);
    if(out["canStart"].asInt() == 1) return true;
    return false;
}

bool canRunMultiplayer(sf::Http& http){
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
        running = true;
        cout << "starting game ... " << endl;
    } else {
        cout << "waiting for players ... " << endl;
    }
    return result;// 1 => creating, 2 => running
}

int getPlayerNumberOnServer(sf::Http& http, int id){
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

void runMultiPlayer(NetworkClient& client){
    while (client.getWindow().isOpen())
    {
        client.run();
        sleep(2);
        client.getWindow().close();
    }
}

Json::Value getPlayersOnServer(sf::Http &http)
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

void exitFromLobby(sf::Http& http, int idPlayer){
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

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "hello") == 0)
            cout << "Bonjour " << ((argv[2]) ? argv[2] : "tout le monde") << endl;
        else if (strcmp(argv[1], "deep_ai") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            DeepAI deep{ngine, 2};

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 1)
                        {
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                                kl.triggerAction(ngine, LEFT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                                kl.triggerAction(ngine, TOP);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                                kl.triggerAction(ngine, RIGHT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                                kl.triggerAction(ngine, DOWN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                                kl.triggerAction(ngine, MOVE);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                                kl.triggerAction(ngine, ATTACK);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                                kl.triggerAction(ngine, PASS_TURN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
                                kl.triggerAction(ngine, SELECT);
                        }
                    }
                    if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 2)
                        deep.run(ngine);
                }
            }
        }
        else if (strcmp(argv[1], "heuristic_ai") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            HeuristicAI heu2{ngine, 2};

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 1)
                        {
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                                kl.triggerAction(ngine, LEFT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                                kl.triggerAction(ngine, TOP);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                                kl.triggerAction(ngine, RIGHT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                                kl.triggerAction(ngine, DOWN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                                kl.triggerAction(ngine, MOVE);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                                kl.triggerAction(ngine, ATTACK);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                                kl.triggerAction(ngine, PASS_TURN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
                                kl.triggerAction(ngine, SELECT);
                        }
                    }
                    if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 2)
                        heu2.run(ngine);
                }
            }
        }
        else if (strcmp(argv[1], "thread") == 0)
        {
            sf::RenderWindow window(sf::VideoMode((25 * 32) + 256, (20 * 32) + 32, 32), "map");

            Client client(window);

            while (window.isOpen())
            {
                client.run();
                sleep(2);
                window.close();
            }
        }
        else if (strcmp(argv[1], "network") == 0)
        {

            string name;
            cout << "Enter your name: ";
            cin >> name;
            while (name.length() < 3 || name.length() > 15)
            {
                cout << "Invalid name. At least 3 characters and up to 15. Re-enter: ";
                cin >> name;
            }

            sf::Http http("http://localhost/", 80);

            sf::Http::Request request1;
            request1.setMethod(sf::Http::Request::Post);
            request1.setUri("/player");
            request1.setHttpVersion(1, 0);

            Json::Value me;
            me["name"] = name;
            me["free"] = true;

            request1.setBody(me.toStyledString());

            sf::Http::Response response1 = http.sendRequest(request1);

            Json::Reader jsonReader;
            Json::Value rep1;
            if (jsonReader.parse(response1.getBody(), rep1))
            {
                int idPlayer = rep1["id"].asInt();

                // query array of players in lobby.
                sf::Http::Request players0;
                players0.setMethod(sf::Http::Request::Get);
                players0.setUri("/player");
                players0.setHttpVersion(1, 0);
                sf::Http::Response playersResp0 = http.sendRequest(players0);
                Json::Reader jsonReaderPlayers0;
                Json::Value jsonPlayers0;
                jsonReaderPlayers0.parse(playersResp0.getBody(), jsonPlayers0);

                cout << "Hello " << name << "! You joined the lobby succesfully!" << endl;
                cout << "Your ID is: " << idPlayer << endl << endl;
                cout << "Players in the lobby: (" << jsonPlayers0["players"].size() << "/2)" << endl;
                
                for(auto& playerInLobby : jsonPlayers0["players"]){
                    cout << "\t-" << playerInLobby[1].asString() << " [id: " << playerInLobby[0].asString() << "]" << endl;
                }

                cout << "Press q to exit from the lobby" << endl;

                while (getchar() != 'q'){/*endless retry*/}
                
                sf::Http::Request request3;
                request3.setMethod(sf::Http::Request::Delete);
                string uri2 = "/player/" + to_string(idPlayer);
                request3.setUri(uri2);
                request3.setHttpVersion(1, 0);
                http.sendRequest(request3);
                cout << "Player " << idPlayer << " deleted" << endl;

                // query players in lobby again
                sf::Http::Request players;
                players.setMethod(sf::Http::Request::Get);
                players.setUri("/player");
                players.setHttpVersion(1, 0);
                sf::Http::Response playersResp = http.sendRequest(players);

                Json::Reader jsonReaderPlayers;
                Json::Value jsonPlayers;
                jsonReaderPlayers.parse(playersResp.getBody(), jsonPlayers);

                cout << "Players in the lobby: (" << jsonPlayers["players"].size() << "/2)" << endl;
                for (auto &playerStillInLobby : jsonPlayers["players"])
                {
                    cout << "\t-" << playerStillInLobby[1].asString() << " [id: " << playerStillInLobby[0].asString() << "]" << endl;
                }
            }
            else
            {
                cout << "Out of places: 2/2 players in the lobby." << endl;
            }
        }
        else if (strcmp(argv[1], "network_test") == 0)
        {
            string name;
            string url="http://localhost";
            int port=80;

            cout << "Enter your name: ";
            cin >> name;
            while (name.length() < 3 || name.length() > 15)
            {
                cout << "Invalid name. At least 3 characters and up to 15. Re-enter: ";
                cin >> name;
            }

            sf::Http http(url, port);

            sf::Http::Request request1;
            request1.setMethod(sf::Http::Request::Post);
            request1.setUri("/player");
            request1.setHttpVersion(1, 0);

            Json::Value me;
            me["name"] = name;
            me["free"] = true;

            request1.setBody(me.toStyledString());

            sf::Http::Response response1 = http.sendRequest(request1);

            Json::Reader jsonReader;
            Json::Value rep1;
            if (jsonReader.parse(response1.getBody(), rep1))
            {
                int idPlayer = rep1["id"].asInt();

                Json::Value jsonPlayers0 = getPlayersOnServer(http);

                cout << "Hello " << name << "! You joined the lobby succesfully!" << endl;
                cout << "Your ID is: " << idPlayer << endl << endl;
                cout << "Players in the lobby: (" << jsonPlayers0["players"].size() << "/2)" << endl;
                
                for(auto& playerInLobby : jsonPlayers0["players"]){
                    cout << "\t-" << playerInLobby[1].asString() << " [id: " << playerInLobby[0].asString() << "]" << endl;
                }
                cout << "The game will automatically start when 2 player were connected" << endl;

                cout << "Press q to exit from the lobby" << endl;

                while (getchar() != 'q')
                {
                    if (canRunMultiplayer(http))
                    {
                        int playerNumberInGame = getPlayerNumberOnServer(http, idPlayer);
                        sf::RenderWindow window(sf::VideoMode((25 * 32) + 256, (20 * 32) + 32, 32), ("GameName - Player " + to_string(playerNumberInGame) + ((playerNumberInGame == 1) ? " Blue" : " Red")));
                        NetworkClient net_client{url, port, playerNumberInGame, window, "game"};
                        imReady(http, idPlayer);
                        // play if all ready
                        while(!canStart(http)){}
                        runMultiPlayer(net_client);
                    }
                }
                if (!running)
                    exitFromLobby(http, idPlayer);
            }
            else
            {
                cout << "Out of places: 2/2 players in the lobby." << endl;
            }
        }
        else if (strcmp(argv[1], "game") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();

            //-----------------------------

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            window.setFramerateLimit(60);
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();

                    else if (event.type == sf::Event::KeyPressed)
                    {
                        bool isMyTurn = true; // validate
                        if (isMyTurn)
                        {
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                                kl.triggerAction(ngine, LEFT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                                kl.triggerAction(ngine, TOP);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                                kl.triggerAction(ngine, RIGHT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                                kl.triggerAction(ngine, DOWN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                                kl.triggerAction(ngine, MOVE);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                                kl.triggerAction(ngine, ATTACK);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                                kl.triggerAction(ngine, PASS_TURN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
                                kl.triggerAction(ngine, SELECT);
                        }
                    }
                }
            }
        }
        else if (strcmp(argv[1], "random_ai") == 0)
        {
            srand(time(0));
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            RandomAI rai2;

            rai2.setPlayerNumber(2);
            //-----------------------------

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 1)
                        {
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                                kl.triggerAction(ngine, LEFT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                                kl.triggerAction(ngine, TOP);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                                kl.triggerAction(ngine, RIGHT);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                                kl.triggerAction(ngine, DOWN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                                kl.triggerAction(ngine, MOVE);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                                kl.triggerAction(ngine, ATTACK);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                                kl.triggerAction(ngine, PASS_TURN);
                            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
                                kl.triggerAction(ngine, SELECT);
                        }
                    }
                    if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 2)
                        rai2.run(ngine);
                }
            }
        }
        else if (strcmp(argv[1], "raivsrai") == 0)
        {
            srand(time(0));
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            RandomAI rai1;
            RandomAI rai2;

            rai1.setPlayerNumber(1);
            rai2.setPlayerNumber(2);
            //-----------------------------

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        while (ngine.getState().getEnd() == false)
                        {
                            rai1.run(ngine);
                            if (ngine.getState().getEnd() == false)
                                rai2.run(ngine);
                        }
                    }
                }
            }
        }
        else if (strcmp(argv[1], "haivshai") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            HeuristicAI hai1{ngine, 1};
            HeuristicAI hai2{ngine, 2};

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        while (ngine.getState().getEnd() == false)
                        {
                            hai1.run(ngine);
                            if (ngine.getState().getEnd() == false)
                                hai2.run(ngine);
                        }
                    }
                }
            }
        }
        else if (strcmp(argv[1], "daivsdai") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();

            ngine.getState().initializeCharacters();
            DeepAI deep1{ngine, 1};
            DeepAI deep2{ngine, 2};

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        while (ngine.getState().getEnd() == false)
                        {
                            deep1.run(ngine);
                            if (ngine.getState().getEnd() == false)
                                deep2.run(ngine);
                        }
                    }
                }
            }
        }
        else if (strcmp(argv[1], "render") == 0)
        {
            cout << "--- Render ---" << endl;
            State state{"render"};
            state.initializeMapCell();
            state.initializeCharacters();
            sf::RenderWindow window(sf::VideoMode(state.getMap()[0].size() * 32 + 256, state.getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(state, window);
            layer.initSurfaces(state);
            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }
                // render
                layer.draw(window);
            }
            return 0;
        }

        else if (strcmp(argv[1], "rollback") == 0)
        {
            engine::Engine ngine{"game"};

            ngine.getState().initializeMapCell();
            ngine.getState().initializeCharacters();

            HeuristicAI heu1{ngine, 1};
            HeuristicAI heu2{ngine, 2};

            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            StateLayer stateLayer(ngine.getState(), window);
            stateLayer.initSurfaces(ngine.getState());
            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            Caretaker caretaker;
            KeyboardListener kl{ngine};

            StateEvent se{StateEventID::ALERT};
            se.text = "Welcome!";
            ngine.getState().notifyObservers(se, ngine.getState());
            time_t epoch = time(nullptr);

            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            while (window.isOpen())
            {
                epoch = time(nullptr);
                if (ngine.getState().getWinner() > 0)
                    stateLayer.showAlertMessage(epoch);
                sf::Event event;
                if (once)
                {
                    stateLayer.draw(window);
                    cout << "Welcome to rollback function. Touch any key to let the IA play." << endl
                         << " When they reach turn 10 our patterns can rollback over all the previous states. " << endl
                         << "In this case, to they will go to start." << endl;
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {
                        MementoState m{ngine.getState().saveToMemento()};
                        caretaker.addMemento(m);
                        if (ngine.getState().getTurn() == 10)
                        {
                            ngine.getState().recoverMemento(caretaker.getMemento(0));
                            cout << "always returning to memento (start) " << endl;
                        }
                        if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 1)
                            heu1.run(ngine);

                        else if (ngine.getState().getEnd() == false && ngine.getState().getTurnOwner() == 2)
                            heu2.run(ngine);
                    }
                }
            }
        }
        else if (strcmp(argv[1], "play") == 0)
        {
            Json::Value root;
            engine::Engine engine{"game"};
            std::string commandsPath = "res/replay.txt";
            engine.getState().initializeMapCell();
            sf::RenderWindow window(sf::VideoMode(engine.getState().getMap()[0].size() * 32 + 256, engine.getState().getMap().size() * 32 + 32, 32), "map");
            engine.getState().initializeCharacters();
            StateLayer stateLayer(engine.getState(), window);
            stateLayer.initSurfaces(engine.getState());

            StateLayer *ptr_stateLayer = &stateLayer;
            engine.getState().registerObserver(ptr_stateLayer);

            cout << "Playing recorded game." << endl;

            bool once = true;

            std::string musicPath = "res/epic_music.wav";
            sf::Music backMusic;
            if (backMusic.openFromFile(musicPath))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }

            bool gameShown = false;
            sf::Event event;
            StateEvent stateEvent(ALLCHANGED);

            while (window.isOpen())
            {
                if (gameShown == false)
                {
                    std::ifstream commandsFile(commandsPath);
                    if (commandsFile)
                    {
                        Json::Value root;
                        Json::Reader reader;
                        if (!reader.parse(commandsFile, root))
                        {
                            cout << "Failed to parse commands\n"
                                 << reader.getFormattedErrorMessages();
                            return 0;
                        }
                        commandsFile.close();
                        Position pos(0, 0);
                        for (unsigned int i = 0; i < root["CommandArray"].size(); i++)
                        {
                            if (root["CommandArray"][i]["id"].asUInt() == engine::MOVE)
                            {

                                pos.setX(root["CommandArray"][i]["xDestination"].asUInt());
                                pos.setY(root["CommandArray"][i]["yDestination"].asUInt());
                                engine::MoveCommand deplacement(*engine.getState().getCharacters()[root["CommandArray"][i]["target"].asUInt()], pos);
                                unique_ptr<engine::Command> ptr_deplacement(new engine::MoveCommand(deplacement));
                                engine.addCommand(move(ptr_deplacement));
                                engine.update();
                            }
                            // Cas de l'attaque
                            else if (root["CommandArray"][i]["id"].asUInt() == engine::ATTACK)
                            {
                                engine::AttackCommand attack(*engine.getState().getCharacters()[root["CommandArray"][i]["attacker"].asUInt()], *engine.getState().getCharacters()[root["CommandArray"][i]["target"].asUInt()]);
                                unique_ptr<engine::Command> ptr_attack(new engine::AttackCommand(attack));
                                engine.addCommand(move(ptr_attack));
                                engine.update();
                            }
                            else if (root["CommandArray"][i]["id"].asUInt() == engine::SELECT_CHARACTER)
                            {
                                engine::SelectCharacterCommand scc{*engine.getState().getCharacters()[root["CommandArray"][i]["target"].asUInt()]};
                                unique_ptr<engine::Command> ptr_select(new engine::SelectCharacterCommand(scc));
                                engine.addCommand(move(ptr_select));
                                engine.update();
                            }
                            else if (root["CommandArray"][i]["id"].asUInt() == engine::FINISH_TURN)
                            {
                                engine::FinishTurnCommand finish;
                                unique_ptr<engine::Command> ptr_finish(new engine::FinishTurnCommand(finish));
                                engine.addCommand(move(ptr_finish));
                                engine.update();
                            }
                            else
                                cout << "we can't handle " << i << " command" << endl;
                        }
                    }
                    gameShown = true;
                }

                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                }
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                    }
                }
            }
        }
        else if (strcmp(argv[1], "engine") == 0)
        {
            cout << "--- Engine du jeu ---" << endl;
            engine::Engine ngine;
            cout << "--- engine object created ---" << endl;

            ngine.getState().initializeMapCell();
            cout << "--- state map initialized ---" << endl;

            ngine.getState().initializeCharacters();
            cout << "--- characters initialized ---" << endl;

            //-----------------------------
            sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            //sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32, ngine.getState().getMap().size() * 32 + 32, 32), "map");
            StateLayer layer(ngine.getState(), window);

            layer.initSurfaces(ngine.getState());
            cout << "--- game window initialized ---" << endl;

            StateLayer stateLayer(ngine.getState(), window);
            cout << "--- state layer initialized with the state and the window ---" << endl;

            stateLayer.initSurfaces(ngine.getState());
            cout << "--- state layer's surfaces initialized ---" << endl;

            // Registering observer
            StateLayer *ptr_stateLayer = &stateLayer;
            ngine.getState().registerObserver(ptr_stateLayer);
            bool once = true;
            sf::Music backMusic;
            if (backMusic.openFromFile("res/epic_music.wav"))
            {
                backMusic.setVolume(40);
                backMusic.setLoop(true);
                backMusic.play();
            }
            //turns number to show how it works
            int turns2go = 9;
            while (window.isOpen())
            {
                sf::Event event;

                if (once)
                {
                    stateLayer.draw(window);
                    once = false;
                    cout << "ATENTION!! push some key to trigger a turn..." << endl;
                }

                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::KeyPressed)
                    {

                        cout << "key pressed !" << endl;
                        cout << endl
                             << "#########################################" << endl;
                        cout << "turn number: " << (((-(turns2go - 10)) == 10) ? 0 : -(turns2go - 7)) << endl;
                        cout << "#########################################" << endl
                             << endl;

                        // harcoding now to 3 the chances to move for each round,
                        // to meet quickly in the center of the map and have some fight.
                        ngine.getState().getCharacters()[0]->setCharacterMove(3);
                        ngine.getState().getCharacters()[1]->setCharacterMove(3);

                        int initialXP1 = ngine.getState().getCharacters()[0]->getPosition().getX();
                        int initialYP1 = ngine.getState().getCharacters()[0]->getPosition().getY();

                        int initialXP2 = ngine.getState().getCharacters()[1]->getPosition().getX();
                        int initialYP2 = ngine.getState().getCharacters()[1]->getPosition().getY();

                        int priority = 0;

                        if (turns2go == 9 || turns2go == 7)
                        {

                            cout << "initial character [Player 1] pos " << ngine.getState().getCharacters()[0]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;
                            unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[0]));
                            ngine.addCommand(move(ptr_sc), priority++);
                            Position pos1{initialXP1, ++initialYP1};
                            unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos1));
                            ngine.addCommand(move(ptr_mc1), priority++);

                            Position pos2{initialXP1, ++initialYP1};
                            unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos2));
                            ngine.addCommand(move(ptr_mc2), priority++);

                            Position pos3{initialXP1, ++initialYP1};
                            unique_ptr<engine::Command> ptr_mc3(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos3));
                            ngine.addCommand(move(ptr_mc3), priority++);

                            unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
                            ngine.addCommand(move(ptr_fc), priority++);
                            cout << "Adding commands for this turn FINISHED. Executing..." << endl;

                            ngine.update();
                            turns2go--;
                        }
                        else if (turns2go == 8 || turns2go == 6)
                        {

                            cout << "initial character [Player 2] pos " << ngine.getState().getCharacters()[1]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;
                            unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[1]));
                            ngine.addCommand(move(ptr_sc), priority++);
                            Position pos1{initialXP2, --initialYP2};
                            unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos1));
                            ngine.addCommand(move(ptr_mc1), priority++);

                            Position pos2{initialXP2, --initialYP2};
                            unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos2));
                            ngine.addCommand(move(ptr_mc2), priority++);

                            Position pos3{initialXP2, --initialYP2};
                            unique_ptr<engine::Command> ptr_mc3(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos3));
                            ngine.addCommand(move(ptr_mc3), priority++);

                            unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
                            ngine.addCommand(move(ptr_fc), priority++);
                            cout << "Adding commands for this turn FINISHED. Executing..." << endl;

                            ngine.update();
                            turns2go--;
                        }
                        else if (turns2go == 5 || turns2go == 3 || turns2go == 1)
                        {

                            cout << "initial character [Player 1] pos " << ngine.getState().getCharacters()[0]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;
                            unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[0]));
                            ngine.addCommand(move(ptr_sc), priority++);
                            Position pos1{initialXP1, ++initialYP1};
                            unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos1));
                            ngine.addCommand(move(ptr_mc1), priority++);

                            Position pos2{initialXP1, ++initialYP1};
                            unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos2));
                            ngine.addCommand(move(ptr_mc2), priority++);

                            unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[0], *ngine.getState().getCharacters()[1]));
                            ngine.addCommand(move(ptr_ac1), priority++);

                            unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
                            ngine.addCommand(move(ptr_fc), priority++);
                            cout << "Adding commands for this turn FINISHED. Executing..." << endl;

                            ngine.update();
                            turns2go--;
                        }
                        else if (turns2go == 4 || turns2go == 2)
                        {
                            cout << "initial character [Player 2] pos " << ngine.getState().getCharacters()[1]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;
                            unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[1]));
                            ngine.addCommand(move(ptr_sc), priority++);

                            Position pos1{initialXP2, ++initialYP2};
                            unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos1));
                            ngine.addCommand(move(ptr_mc1), priority++);

                            Position pos2{initialXP2, ++initialYP2};
                            unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos2));
                            ngine.addCommand(move(ptr_mc2), priority++);

                            unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[1], *ngine.getState().getCharacters()[0]));
                            ngine.addCommand(move(ptr_ac1), priority++);

                            unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
                            ngine.addCommand(move(ptr_fc), priority++);
                            cout << "Adding commands for this turn FINISHED. Executing..." << endl;

                            ngine.update();
                            turns2go--;
                        }
                        else if (turns2go == 0)
                        {
                            sleep(2);
                            cout << "No more turns left" << endl;
                            cout << "ENGINE SHOW finished, closing window" << endl;
                            window.close();
                        }
                    }
                }
            }
        }
    }
    return 0;
}
