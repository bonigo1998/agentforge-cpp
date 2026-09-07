# AgentForge C++

A lightweight AI agent harness built in C++ to explore how language models use tools and complete tasks.

## Overview

An AI model can propose actions, but it needs software around it to execute those actions. That software is an **agent harness**.

AgentForge will manage the interaction between a user, an AI model, and a set of tools. It will receive a task, send it to the model, execute permitted tool requests, and return the results until the task finishes or a configured limit is reached.

## Project Status

🚧 Early development. The features below are planned and are not yet implemented.

This project is being built incrementally as a hands-on learning exercise in C++, AI agents, testing, and open-source development.

## Planned Features

- **Terminal interface** — Submit tasks and view responses.
- **Model integration** — Connect to a language model through an API.
- **Tool execution** — Let the model request supported actions.
- **Workspace boundaries** — Restrict file tools to a designated directory.
- **Execution limits** — Cap agent steps and tool usage.
- **Logging** — Record actions, tool results, and errors.
- **Tests** — Verify tool behavior and agent control flow.

Initial tools will focus on reading and writing workspace files. Arbitrary shell execution is outside the initial scope.

## Planned Architecture

```text
User task
    |
    v
Agent loop <------> Model API
    |
    v
Tool dispatcher
    |
    v
Workspace tools
    |
    v
Tool results returned to the agent loop
```

The harness will validate tool requests before execution and stop when the model produces a final response or an execution limit is reached.

## Technology

- **Language:** C++
- **Build system:** CMake
- **Initial development platform:** macOS
- **Editor:** Cursor
- **Version control:** Git and GitHub

API client and testing dependencies will be selected during implementation.

## Roadmap

- [ ] Set up a minimal C++ project with CMake.
- [ ] Build a terminal interface.
- [ ] Add a model API client.
- [ ] Define tool request and result formats.
- [ ] Implement workspace file tools.
- [ ] Build the agent loop.
- [ ] Add execution limits and error handling.
- [ ] Add logs and automated tests.
- [ ] Document setup and example tasks.

## Getting Started

Build and run instructions will be added when the first executable is available.

## Credentials and Safety

API keys must stay outside source control. Examples will use placeholder values.

Workspace checks and execution limits are planned safeguards, not a complete security sandbox. Run development experiments only with disposable, non-sensitive files.

## Contributing

Feedback, bug reports, and focused contributions are welcome.

For larger changes, please open an issue first to discuss the approach. Pull requests should explain the change and include relevant verification.

## Author

[bonigo1998](https://github.com/bonigo1998)
