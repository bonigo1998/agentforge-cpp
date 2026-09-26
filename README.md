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
- Read-only workspace file access through `/read <path>`.
- File questions through `/askfile <path> <question>`.
- Workspace boundary checks that reject absolute paths and paths outside the project.


The current version keeps recent conversation history, supports read-only workspace commands, and can execute model-requested file reads through a bounded agent loop.

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

## Workspace Commands

Read a file directly in the terminal:

```text
/read README.md
```

Ask the local model a question using a file:

```text
/askfile README.md Summarize this project in one sentence.
```

Use `/clear` to remove conversation history and `exit` to quit.

File paths must be relative to the directory where AgentForge starts. Absolute paths and paths that escape the workspace are rejected.

## Agent Tool Loop

Ask the model to use a workspace file:

```text
Use read_file to read src/chat_message.hpp. List the fields in ChatMessage.
```

When the model requests `read_file`, AgentForge validates the arguments, attempts the file read, and returns the result to the model.

The terminal prints `Using read_file tool...` when a file-read attempt starts.

Current limits:

- At most three model requests and two file-read attempts per task.
- Exactly one tool call is accepted per model response.
- Unsupported tools and invalid arguments are rejected.
- File contents larger than 4,000 bytes are replaced with an error message.
- File paths must stay inside the workspace.

Tool selection depends on the model. A correct-looking answer alone does not prove that a file was read. Use `/read` for direct file contents or `/askfile` to supply file context explicitly.

## Current Configuration

- Model: `qwen3:1.7b`
- Endpoint: `http://127.0.0.1:11434/api/chat`
- Context window: 2,048 tokens
- Maximum generated response: 128 tokens
- Connection timeout: 5 seconds
- Total request timeout: 180 seconds

Responses that reach the output limit are marked as shortened.

## Testing

From the project directory, configure, build, and run the automated tests:

```bash
cmake -S . -B build -DBUILD_TESTING=ON &&
cmake --build build &&
ctest --test-dir build --output-on-failure --no-tests=error
```

The tests cover:

- Slash-command parsing.
- Workspace file reads and missing-file errors.
- Rejection of absolute paths and paths that escape the workspace.
- Symlinks that stay inside or point outside the workspace.

These tests run without Ollama or a model download. They do not test model responses or the complete agent loop.

GitHub Actions also builds the project and runs both test suites on Ubuntu for pull requests targeting `main` and pushes to `main`.


## Roadmap

- [x] Create a C++20 project with CMake.
- [x] Build an interactive command loop.
- [x] Separate the interface and model client.
- [x] Connect to a local model.
- [x] Add conversation history.
- [x] Define and validate tool requests.
- [x] Implement workspace file tools.
- [x] Add an agent tool execution loop with step limits.
- [x] Add automated tests and GitHub Actions.
- [ ] Add execution logs.

## Contributing

Focused improvements and bug reports are welcome. Pull requests should explain the change and how it was verified.