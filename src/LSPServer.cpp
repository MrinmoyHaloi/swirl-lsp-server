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
            // Step 1: Read the "Content-Length" header
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
                // Log an error, but for now we'll just continue
                continue;
            }

            // Step 2: Read the mandatory blank line after the header
            std::string blankLine;
            std::getline(std::cin, blankLine);

            // Step 3: Read the JSON content itself
            std::vector<char> jsonBuffer(contentLength);
            std::cin.read(jsonBuffer.data(), contentLength);
            std::string jsonContent(jsonBuffer.begin(), jsonBuffer.end());

            // Step 4: Parse and process the message
            parseMessage(jsonContent);
        }
    }

    void Server::parseMessage(const std::string &jsonContent) {
        try {
            json request = json::parse(jsonContent);
            if (request.contains("method")) {
                std::string method = request["method"];

                std::cerr << request.dump(4) << std::endl;

                if (method == "initialize") {
                    // Handle the "initialize" request
                    handleInitialize(request);
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
    }

    void Server::handleInitialize(const json &request) {
        // Handle the "initialize" request
        json response = {{"jsonrpc", "2.0"},
                         {"id", request["id"]},
                         {"result",
                          {{"capabilities",
                            {
                                // Advertise the features your server supports
                                {"textDocumentSync", 1}, // 1 = Full sync
                            }}}}};
        sendResponse(response);
    }
} // namespace LSP