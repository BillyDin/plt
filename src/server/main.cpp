#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <map>
#include <memory>
#include <unistd.h>
#include <microhttpd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include "server.h"
#include "state.h"
#include "render.h"
#include "engine.h"
#include "ai.h"
#define SOCKET_ERROR -1

using namespace server;
using namespace std;
using namespace state;
using namespace render;
using namespace engine;
using namespace ai;

class Request
{
public:
    struct MHD_PostProcessor *pp = nullptr;
    string data;
    ~Request()
    {
        if (pp)
            MHD_destroy_post_processor(pp);
    }
};

static int post_iterator(void *cls,
                         enum MHD_ValueKind kind,
                         const char *key,
                         const char *filename,
                         const char *content_type,
                         const char *transfer_encoding,
                         const char *data, uint64_t off, size_t size)
{
    return MHD_NO;
}

// based on https://www.gnu.org/software/libmicrohttpd/
static int handler(void *cls,
             struct MHD_Connection *connection,
             const char *url,
             const char *method,
             const char *version,
             const char *upload_data, size_t *upload_data_size, void **ptr)
{
    Request *request = (Request *)*ptr;

    if (!request)
    {
        request = new Request();
        if (!request)
        {
            return MHD_NO;
        }
        *ptr = request;
        if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 || strcmp(method, MHD_HTTP_METHOD_PUT) == 0)
        {
            request->pp = MHD_create_post_processor(connection, 1024, &post_iterator, request);
            if (!request->pp)
            {
                cerr << "Failed to setup post processor for " << url << endl;
                return MHD_NO;
            }
        }
        return MHD_YES;
    }

    if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 || strcmp(method, MHD_HTTP_METHOD_PUT) == 0)
    {
        MHD_post_process(request->pp, upload_data, *upload_data_size);
        if (*upload_data_size != 0)
        {
            request->data = upload_data;
            *upload_data_size = 0;
            return MHD_YES;
        }
    }

    HttpStatus status;
    string response;
    try
    {
        ServicesManager *manager = (ServicesManager *)cls;
        status = manager->queryService(response, request->data, url, method);
    }
    catch (ServiceException &e)
    {
        status = e.status();
        response += "\n";
    }
    catch (exception &e)
    {
        status = HttpStatus::SERVER_ERROR;
        response = e.what();
        response += "\n";
    }
    catch (...)
    {
        status = HttpStatus::SERVER_ERROR;
        response = "Unknown exception\n";
    }

    struct MHD_Response *mhd_response;
    mhd_response = MHD_create_response_from_buffer(response.size(), (void *)response.c_str(), MHD_RESPMEM_MUST_COPY);
    if (strcmp(method, MHD_HTTP_METHOD_GET) == 0)
    {
        MHD_add_response_header(mhd_response, "Content-Type", "application/json; charset=utf-8");
    }
    int ret = MHD_queue_response(connection, status, mhd_response);
    MHD_destroy_response(mhd_response);
    return ret;
}

static void request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe)
{
    Request *request = (Request *)*con_cls;
    if (request)
    {
        delete request;
        *con_cls = nullptr;
    }
}

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "hello") == 0)
            cout << "Bonjour " << ((argv[2]) ? argv[2] : "tout le monde") << endl;

        else if (strcmp(argv[1], "record") == 0)
        {

            std::string commands_file = "res/replay.txt";

            engine::Engine ngine{"game"};

            ngine.setEnableRecord(true);
            ngine.getState().initializeMapCell();
            ngine.getState().initializeCharacters();

            HeuristicAI hai1{ngine, 1};
            HeuristicAI hai2{ngine, 2};

            cout << "\t\t--- Record ---" << endl;
            cout << "On enregistre 12 tours de jeu dans le fichier " << commands_file << endl;
            cout << "Deux IA heuristiques jouent l'une contre l'autre" << endl;
            sleep(2);
            cout << "--> Début de l'enregistrement <--" << endl;

            // On simule 12 tours
            while (ngine.getState().getTurn() != 13)
            {
                // hai1 player1
                if (ngine.getState().getTurn() % 2 != 0)
                {
                    hai1.run(ngine);
                }
                //hai2 player2
                else
                {
                    hai2.run(ngine);
                }
            }
            cout << "--> Fin de l'enregistrement <--" << endl;

            cout << "--> Debut de l'enregistrement dans le fichier <--" << endl;
            // Ouverture du fichier en ecriture en effacant son contenu à l'ouverture
            std::ofstream written_file(commands_file, ios::out | ios::trunc);
            if (written_file)
            {

                Json::Value root = ngine.getRecord();
                cout << root << endl;

                // Ecriture dans le fichier du tableau de commandes de cette partie
                written_file << root;

                // Fermeture du fichier
                written_file.close();

                cout << "--> Fin de l'enregistrement dans le fichier <--" << endl;
            }
            else
            {
                cerr << "Impossible d'ouvrir le fichier des commandes enregistrées pour l'ecriture" << endl;
            }
        }
        else if (strcmp(argv[1], "listen") == 0)
        {
            try
            {
                Game game;
                ServicesManager servicesManager;

                std::unique_ptr<AbstractService> ptr_versionService(new VersionService());
                servicesManager.registerService(move(ptr_versionService));

                std::unique_ptr<AbstractService> ptr_playerService(new PlayerService(ref(game)));
                servicesManager.registerService(move(ptr_playerService));

                std::unique_ptr<AbstractService> ptr_gameService(new GameService(ref(game)));
                servicesManager.registerService(move(ptr_gameService));

                std::unique_ptr<AbstractService> ptr_commandService(new CommandsService(game.getEngine()));
                servicesManager.registerService(move(ptr_commandService));

                struct MHD_Daemon *d;
                if (argc != 2)
                {
                    printf("%s PORT\n", argv[0]);
                    return 1;
                }

                d = MHD_start_daemon( // MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL,
                    MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
                    // MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG | MHD_USE_POLL,
                    // MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,
                    80,
                    NULL, NULL,
                    &handler, (void *)&servicesManager,
                    MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
                    MHD_OPTION_END);

                if (d == NULL)
                    return 1;
                cout << "server is listening in port 80..." << endl << "press any button to stop the server" << endl;
                (void)getc(stdin);
                MHD_stop_daemon(d);
            }
            catch (exception &e)
            {
                cerr << "Exception: " << e.what() << endl;
            }
        }
    }
    return 0;
}
