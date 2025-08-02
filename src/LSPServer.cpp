#include "LSPServer.h"
#include <iostream>

namespace LSP {

    Server::Server() {
        std::cerr << "LSP Server initialized." << std::endl;
    }

    Server::~Server() {
        // std::cerr << "LSP Server shutting down." << std::endl;
    }

    void Server::run() {
        // Start the server and listen for incoming requests

        while (true) {
            // Read the "Content-Length" header
            std::string contentLengthHeader;
            std::getline(std::cin, contentLengthHeader);
            if (contentLengthHeader.empty()) {
                // End of stream or client closed connection
                break;
            }

            int contentLength = 0;
            try {
                contentLength = std::stoi(contentLengthHeader.substr(16));
            } catch (const std::exception &e) {
                // Log an error
                continue;
            }

            // Read the mandatory blank line after the header
            std::string blankLine;
            std::getline(std::cin, blankLine);

            // Read the JSON content itself
            std::vector<char> jsonBuffer(contentLength);
            std::cin.read(jsonBuffer.data(), contentLength);
            std::string jsonContent(jsonBuffer.begin(), jsonBuffer.end());

            // Parse and process the message
            parseMessage(jsonContent);
        }
    }

    void Server::parseMessage(const std::string &jsonContent) {
        try {
            json request = json::parse(jsonContent);
            if (request.contains("method")) {
                std::string method = request["method"];

                std::cerr << "[Received Request] " << request.dump(4)
                          << std::endl;

                if (method == "initialize") {
                    // Handle the "initialize" request
                    onInitialize(request);
                } else if (method == "textDocument/didChange") {
                    // Handle the "didChangeContent" notification
                    onDidChangeContent(request);
                } else if (method == "textDocument/didOpen") {
                    onDidOpen(request);
                } else if (method == "textDocument/didSave") {
                    // Handle the "didSave" notification
                    std::cerr << "[Did Save] "
                              << request["params"]["textDocument"]["uri"]
                              << std::endl;
                    // Here you would typically save the document state
                } else if (method == "textDocument/completion") {
                    // Handle the "completion" request
                    onCompletion(request);
                } else if (method == "completionItem/resolve") {
                    // Handle the "completionResolve" request
                    onCompletionResolve(request);
                } else if (method == "$/setTrace") {
                    // Handle the "setTrace" request
                    onSetTrace(request);
                } else {
                    // Handle other methods or send an error response
                    json errorResponse = {
                        {"jsonrpc", "2.0"},
                        {"id", 1},
                        {"result", nullptr},
                        {"error",
                         {{"code", -32601}, // Method not found
                          {"message", "Method not found: " + method}}}};
                    sendResponse(errorResponse);
                }
            }
        } catch (const json::parse_error &e) {
            // Log JSON parsing error
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }

    void Server::sendResponse(const json &response) {
        std::string responseStr = response.dump();
        std::cout << "Content-Length: " << responseStr.size() << "\r\n\r\n"
                  << responseStr;
        std::cout.flush();
        std::cerr << "[Sent Response] " << response.dump(4) << std::endl;
    }

    void Server::onInitialize(const json &request) {
        // Handle the "initialize" request
        json response = {{"jsonrpc", "2.0"},
                         {"id", request["id"]},
                         {"result",
                          {{"capabilities",
                            {// Advertise the features your server supports
                             {"textDocumentSync", 1}, // 1 = Full sync
                             {"completionProvider",
                              {{"resolveProvider", true},
                               {"triggerCharacters", {".", "@"}}}}}}}}};
        sendResponse(response);
    }

    void Server::onDidOpen(const json &request) {
        // Handle the "didOpen" notification
        if (request.contains("params") &&
            request["params"].contains("textDocument")) {
            std::string uri = request["params"]["textDocument"]["uri"];
            std::string text = request["params"]["textDocument"]["text"];
            int version = request["params"]["textDocument"]["version"];

            // Store the document
            storeDocument(uri, text, version);

            std::cerr << "[Did Open] " << uri << " (length: " << text.length()
                      << ")" << std::endl;
        }
    }

    void Server::onDidChangeContent(const json &request) {
        // Handle the "didChangeContent" notification
        if (request.contains("params")) {
            std::string uri = request["params"]["textDocument"]["uri"];
            int version = request["params"]["textDocument"]["version"];

            // Handle incremental changes
            if (request["params"].contains("contentChanges")) {
                auto changes = request["params"]["contentChanges"];

                for (const auto &change : changes) {
                    if (change.contains("text") && !change.contains("range")) {
                        // Full document update
                        std::string newText = change["text"];
                        updateDocument(uri, newText, version);
                    } else if (change.contains("range") &&
                               change.contains("text")) {
                        // Incremental update - you'd need to implement
                        // range-based updates
                        std::cerr << "[Incremental Change] Not implemented yet"
                                  << std::endl;
                    }
                }
            }

            std::cerr << "[Did Change Content] " << uri
                      << " (version: " << version << ")" << std::endl;
        }
    }
    void Server::onCompletion(const json &request) {
        // Handle the "completion" request
        json response = {
            {"jsonrpc", "2.0"},
            {"id", request["id"]},
            {"result",
             {{"isIncomplete", false},
              {"items",
               {{{"label", "Hello"},
                 {"kind", 1}, // Text
                 {"sortText", "0001"},
                 {"detail", "Hello from swirl C++ lsp server"},
                 {"documentation", "Text completion from swirl LSP server"}},
                {{"label", "from"},
                 {"kind", 2}, // Method
                 {"sortText", "0002"},
                 {"detail", "from swirl C++ lsp server"},
                 {"documentation", "Method completion from swirl LSP server"}},
                {{"label", "swirl"},
                 {"kind", 3}, // Function
                 {"sortText", "0003"},
                 {"detail", "swirl C++ lsp server"},
                 {"documentation",
                  "Function completion from swirl LSP server"}},
                {{"label", "C++"},
                 {"kind", 6}, // Class
                 {"sortText", "0004"},
                 {"detail", "C++ lsp server"},
                 {"documentation", "Class completion from swirl LSP server"}},
                {{"label", "lsp"},
                 {"kind", 7}, // Interface
                 {"sortText", "0005"},
                 {"detail", "lsp server"},
                 {"documentation",
                  "Interface completion from swirl LSP server"}},
                {{"label", "server"},
                 {"kind", 9}, // Module
                 {"sortText", "0006"},
                 {"detail", "server"},
                 {"documentation",
                  "Module completion from swirl LSP server"}}}}}}};
        sendResponse(response);
    }
    void Server::onCompletionResolve(const json &request) {
        // Handle the "completionResolve" request
        json result = request["params"];
        if (result.contains("sortText")) {
            result.erase("sortText");
        }

        json response = {
            {"jsonrpc", "2.0"}, {"id", request["id"]}, {"result", result}};
        sendResponse(response);
    }
    void Server::onSetTrace(const json &request) {
        // Handle the "setTrace" notification
        std::string traceValue = request["params"]["value"];
        std::cerr << "[Set Trace] " << traceValue << std::endl;
    }

    void Server::storeDocument(const std::string &uri,
                               const std::string &content, int version) {
        documents[uri] = content;
        documentVersions[uri] = version;
        std::cerr << "[Document Stored] " << uri << " (version: " << version
                  << ")" << std::endl;
    }

    void Server::updateDocument(const std::string &uri,
                                const std::string &newContent, int version) {
        if (hasDocument(uri)) {
            documents[uri] = newContent;
            documentVersions[uri] = version;
            std::cerr << "[Document Updated] " << uri
                      << " (version: " << version << ")" << std::endl;
        }
    }

    void Server::removeDocument(const std::string &uri) {
        documents.erase(uri);
        documentVersions.erase(uri);
        std::cerr << "[Document Removed] " << uri << std::endl;
    }

    std::string Server::getDocument(const std::string &uri) {
        auto it = documents.find(uri);
        return (it != documents.end()) ? it->second : "";
    }

    bool Server::hasDocument(const std::string &uri) {
        return documents.find(uri) != documents.end();
    }
} // namespace LSP