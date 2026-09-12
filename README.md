# AgentForge C++

A C++ project for learning how to build an AI agent harness, starting with a local language model connection.

## Current Features

- Interactive terminal interface.
- Local model responses through Ollama.
- HTTP requests using libcurl.
- JSON serialization and parsing using nlohmann/json.
- Empty-input validation and an `exit` command.
- Connection and request timeouts.
- Request error handling that keeps the terminal session running.

The current version sends each task independently. Conversation history, workspace tools, and an autonomous tool execution loop are planned.

## Requirements

- macOS with Xcode or Command Line Tools
- Git
- CMake
- A C++20 compiler
- libcurl development files
- nlohmann/json
- Ollama

Development has been tested on a Mac with an Apple A18 Pro chip and 8 GB of memory.

## Build on macOS

Install the dependencies with Homebrew:

```bash
brew install cmake nlohmann-json ollama
```

The development setup uses libcurl provided by the installed Apple developer tools.

Clone and build:

```bash
git clone https://github.com/bonigo1998/agentforge-cpp.git
cd agentforge-cpp
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix nlohmann-json)"
cmake --build build
```

## Run

Start Ollama in one terminal with cloud features disabled:

```bash
OLLAMA_NO_CLOUD=1 ollama serve
```

Leave that terminal open.

In a second terminal, download the model:

```bash
ollama pull qwen3:1.7b
```

From the project folder, start AgentForge:

```bash
./build/agentforge
```

Enter a task such as:

```text
Explain what a C++ class is in two short sentences.
```

Type `exit` to quit AgentForge.

Model inference runs locally and requires no API key or paid API usage. Internet access is needed for the initial software and model downloads.

## Current Configuration

- Model: `qwen3:1.7b`
- Endpoint: `http://127.0.0.1:11434/api/chat`
- Context window: 2,048 tokens
- Maximum generated response: 128 tokens
- Connection timeout: 5 seconds
- Total request timeout: 180 seconds

Responses that reach the output limit are marked as shortened.

## Roadmap

- [x] Create a C++20 project with CMake.
- [x] Build an interactive command loop.
- [x] Separate the interface and model client.
- [x] Connect to a local model.
- [ ] Add conversation history.
- [ ] Define and validate tool requests.
- [ ] Implement workspace file tools.
- [ ] Add an agent tool execution loop with step limits.
- [ ] Add automated tests and execution logs.

## Contributing

Focused improvements and bug reports are welcome. Pull requests should explain the change and how it was verified.
