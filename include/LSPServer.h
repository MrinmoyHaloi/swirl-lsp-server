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
        void onInitialize(const json &request);
        void onDidChangeContent(const json &request);
        void onCompletion(const json &request);
        void onCompletionResolve(const json &request);
        void onSetTrace(const json &request);
        void onDidOpen(const json &request);
    };
} // namespace LSP