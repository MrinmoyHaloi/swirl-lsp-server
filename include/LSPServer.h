#pragma once
#include "json.hpp"

using json = nlohmann::json;

namespace LSP {

    class Server {
    public:
        Server();
        ~Server();
        void run();
    private:
        void processRequest(const json &request);
        void sendResponse(const json &response);
        void parseMessage(const std::string &jsonContent);
        void handleInitialize(const json &request);
    };
}