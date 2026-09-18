#include "ollama_client.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

using nlohmann::json;

std::size_t collect_response(char* data, std::size_t size,
                             std::size_t count, void* userdata) noexcept {
    const std::size_t bytes = size * count;

    try {
        static_cast<std::string*>(userdata)->append(data, bytes);
        return bytes;
    } catch (...) {
        return 0;
    }
}

void check_curl(CURLcode code) {
    if (code != CURLE_OK) {
        throw std::runtime_error(
            std::string("HTTP request failed: ") + curl_easy_strerror(code));
    }
}

json make_request(const json& messages) {
    return {
        {"model", "qwen3:1.7b"},
        {"messages", messages},
        {"stream", false},
        {"think", false},
        {"options", {
            {"num_ctx", 2048},
            {"num_predict", 128}
        }}
    };
}

json send_request(const json& request) {
    const std::string body = request.dump();
    std::string response;

    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(
        curl_slist_append(nullptr, "Content-Type: application/json"),
        &curl_slist_free_all);

    if (!headers) {
        throw std::runtime_error("Could not create HTTP headers.");
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(
        curl_easy_init(), &curl_easy_cleanup);

    if (!curl) {
        throw std::runtime_error("Could not create HTTP client.");
    }

    check_curl(curl_easy_setopt(curl.get(), CURLOPT_URL,
                               "http://127.0.0.1:11434/api/chat"));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_NOPROXY, "*"));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get()));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, body.c_str()));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION,
                               &collect_response));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA,
                               static_cast<void*>(&response)));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5L));
    check_curl(curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 180L));

    check_curl(curl_easy_perform(curl.get()));

    long status = 0;
    check_curl(curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status));

    if (status != 200) {
        throw std::runtime_error(
            "Ollama returned HTTP " + std::to_string(status) + ": " +
            response);
    }

    const json result = json::parse(response);

    if (!result.value("done", false)) {
        throw std::runtime_error("Ollama returned an incomplete response.");
    }

    return result;
}

}  // namespace

std::string OllamaClient::chat(
    const std::vector<ChatMessage>& messages) const {
    json json_messages = json::array();

    for (const auto& message : messages) {
        json_messages.push_back({
            {"role", message.role},
            {"content", message.content}
        });
    }

    const json result = send_request(make_request(json_messages));

    std::string answer =
        result.at("message").at("content").get<std::string>();

    if (answer.empty()) {
        throw std::runtime_error("Ollama returned an empty answer.");
    }

    if (result.value("done_reason", "") == "length") {
        answer += "\n[Response stopped at the output limit.]";
    }

    return answer;
}

ModelResponse OllamaClient::chat_with_tools(
    const json& messages) const {
    if (!messages.is_array()) {
        throw std::invalid_argument("Messages must be a JSON array.");
    }

    const json parameters = {
        {"type", "object"},
        {"required", json::array({"path"})},
        {"properties", {{"path", {
            {"type", "string"},
            {"description", "Workspace-relative file path"}
        }}}},
        {"additionalProperties", false}
    };

    const json file_tool = {
        {"type", "function"},
        {"function", {
            {"name", "read_file"},
            {"description", "Read a text file in the project workspace"},
            {"parameters", parameters}
        }}
    };

    json request = make_request(messages);
    request["tools"] = json::array({file_tool});

    const json result = send_request(request);
    const json& message = result.at("message");

    ModelResponse reply;
    reply.content = message.at("content").get<std::string>();

    if (message.contains("tool_calls") &&
        !message.at("tool_calls").is_null()) {
        if (!message.at("tool_calls").is_array()) {
            throw std::runtime_error("Ollama returned invalid tool calls.");
        }

        reply.tool_calls = message.at("tool_calls");
    }

    if (reply.content.empty() && reply.tool_calls.empty()) {
        throw std::runtime_error("Ollama returned an empty response.");
    }

    if (result.value("done_reason", "") == "length") {
        if (!reply.tool_calls.empty()) {
            throw std::runtime_error(
                "Ollama stopped while generating a tool call.");
        }

        reply.content += "\n[Response stopped at the output limit.]";
    }

    return reply;
}