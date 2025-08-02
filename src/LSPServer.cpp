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
                             {"textDocumentSync", 2}, // 2 = Incremental sync
                             {"completionProvider",
                              {{"resolveProvider", true},
                               {"triggerCharacters", {".", "@"}}}}}}}}};
        sendResponse(response);
    }

    void Server::onDidOpen(const json &request) {
        // Handle the "didOpen" notification
        std::cerr << "[Did Open] " << request["params"]["textDocument"]["uri"]
                  << std::endl;
        // Here you would typically load the document into your model
    }

    void Server::onDidChangeContent(const json &request) {
        // Handle the "didChangeContent" notification
        std::cerr << "[Did Change Content] \""
                  << request["params"]["contentChanges"][0]["text"].dump(4)
                  << "\"" << std::endl;
        // Here you would typically update your document model
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
} // namespace LSP