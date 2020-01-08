#include "ServicesManager.h"
#include "ServiceException.h"
#include <iostream>
using namespace std;
using namespace server;

void ServicesManager::registerService(unique_ptr<AbstractService> service)
{
    services.push_back(std::move(service));
}

AbstractService *ServicesManager::findService(string url)
{
    for (auto &service : services)
    {
        const string &pattern(service->getPattern());
        if (url.find(pattern) != 0 || (url.size() > pattern.size() && url[pattern.size()] != '/'))
            continue;
        return service.get();
    }
    return nullptr;
}

HttpStatus ServicesManager::queryService(string &out, string &in, string url, string method)
{
    AbstractService *service = findService(url);
    if (!service) throw ServiceException(HttpStatus::NOT_FOUND, "Service " + url + " not found");

    const string &pattern(service->getPattern());
    int id = -1; // not found

    // validation and id assignment if possible
    if (url.size() > pattern.size())
    {
        string end = url.substr(pattern.size());
        if (end[0] != '/' || end.substr(1).empty()) throw ServiceException(HttpStatus::BAD_REQUEST, "Bad url (expected: <servicename>/<id>)");
        end = end.substr(1);
        try
        {
            size_t pos = 0;
            id = stoi(end, &pos);
            if (pos != end.size())
                throw ServiceException(HttpStatus::BAD_REQUEST, "Bad url: '" + end + "' isn't a correct id");
        }
        catch (...) {throw ServiceException(HttpStatus::BAD_REQUEST, "Bad url: '" + end + "' isn't a correct id");}
    }
    // HTTP METHODS
    if (method == "GET")
    {
        Json::Value jsonOut;
        HttpStatus status = service->get(jsonOut, id);
        out = jsonOut.toStyledString();
        return status;
    }
    else if (method == "PUT")
    {
        Json::Reader jsonReader;
        Json::Value jsonIn;
        if (!jsonReader.parse(in, jsonIn))
            throw ServiceException(HttpStatus::BAD_REQUEST, "Invalid data: " + jsonReader.getFormattedErrorMessages());
        return service->put(jsonIn, id);
    }
    else if (method == "POST")
    {
        Json::Reader jsonReader;
        Json::Value jsonIn;
        if (!jsonReader.parse(in, jsonIn))
            throw ServiceException(HttpStatus::BAD_REQUEST, "Invalid data: " + jsonReader.getFormattedErrorMessages());
        Json::Value jsonOut;
        HttpStatus status = service->post(jsonOut, jsonIn);
        out = jsonOut.toStyledString();
        return status;
    }

    else if (method == "DELETE")
    {
        return service->remove(id);
    }
    throw ServiceException(HttpStatus::BAD_REQUEST, "Method " + method + " invalide");
}
