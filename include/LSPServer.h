#pragma once
#include "json.hpp"
#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace LSP {

    class Server {
      public:
        Server();
        ~Server();
        void run();

      private:
        // Document manger: URI: content
        std::unordered_map<std::string, std::string> documents;

        // Track document version
        std::unordered_map<std::string, int> documentVersions;

        void storeDocument(const std::string &uri, const std::string &content,
                           int version = 0);
        void updateDocument(const std::string &uri,
                            const std::string &newContent, int version);
        void removeDocument(const std::string &uri);
        std::string getDocument(const std::string &uri);
        bool hasDocument(const std::string &uri);

        // helper functions
        void processRequest(const json &request);
        void sendResponse(const json &response);
        void parseMessage(const std::string &jsonContent);

        // Request handlers
        void onInitialize(const json &request);
        void onDidOpen(const json &request);
        void onDidChangeContent(const json &request);
        void onCompletion(const json &request);
        void onCompletionResolve(const json &request);
        void onSetTrace(const json &request);

        void validateDocument(const std::string &uri);
        std::pair<int, int> calculatePosition(const std::string &content,
                                                  size_t offset);
    };
} // namespace LSP