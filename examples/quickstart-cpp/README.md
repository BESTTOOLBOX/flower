---
tags: [quickstart, linear regression, tabular]
dataset: [Synthetic]
framework: [C++]
---

# Flower Clients in C++

In this example you will train a linear model on synthetic data using external
C++ clients and a Python `ServerApp`.

This quickstart uses the Flower `grpc-rere` Fleet API used by recent Flower
versions. The C++ clients connect to a running SuperLink, receive train and
evaluate messages, serialize replies with Flower `RecordDict` payloads, and
push the resulting objects back through the SuperLink object store.

## Acknowledgements

Many thanks to the contributors to this code:

For This Version:

- Jiaxiang Geng (Duke Kunshan University, main contributor)
- Yiyi Lu (Duke Kunshan University, main contributor)
- Lunyu Zhao (Duke Kunshan University, main contributor)
- Bing Luo (Duke Kunshan University, director)

Edge-Intelligence-Lab Fork: https://github.com/Edge-Intelligence-Lab/flower-C--SDK

For Previous Version:
- Lekang Jiang (original author and main contributor)
- Francisco Jose Solis (code re-organization)
- Andreea Zaharia (training algorithm and data generation)

## Install requirements

You'll need Python with `flwr>=1.31.0`, CMake, a C++17 compiler, gRPC C++,
protobuf, `protoc`, `grpc_cpp_plugin`, and OpenSSL.

Install the Python dependencies from this directory:

```bash
python -m pip install -e .
```

## Building the example

This example provides a `CMakeLists.txt` file to configure and build the C++
client.

From `examples/quickstart-cpp` inside a Flower checkout:

```bash
cmake -S . -B build
cmake --build build -j
```

If this directory is built outside the Flower repository, pass the Flower source
tree explicitly:

```bash
cmake -S . -B build -DFLWR_SOURCE_ROOT=/path/to/flower
cmake --build build -j
```

If gRPC/protobuf are installed in a custom prefix:

```bash
export CMAKE_PREFIX_PATH=/path/to/grpc-prefix
export PATH=/path/to/grpc-prefix/bin:$PATH
cmake -S . -B build \
  -DFLWR_SOURCE_ROOT=/path/to/flower \
  -DGRPC_CPP_PLUGIN_EXECUTABLE=/path/to/grpc_cpp_plugin
cmake --build build -j
```

## Flower version compatibility

Verified against Flower `1.37.0`. The C++ client talks to the SuperLink over the
`grpc-rere` Fleet API, which is versioned independently from the Control API.

| Flower | Fleet API (C++ clients) | Control API (`flwr run`) |
| --- | --- | --- |
| 1.31 – 1.36 | `127.0.0.1:9092` | `127.0.0.1:9093` (gRPC) |
| 1.37+ | `127.0.0.1:9092` | `127.0.0.1:<--port>`, HTTP, 8000 by default |

Flower 1.37 moved the Control API to HTTP on the SuperLink's `--host`/`--port`.
It migrates `[tool.flwr.federations]` from `pyproject.toml` into
`~/.flwr/config.toml` on the first `flwr run`, but the migrated address keeps
whatever port `pyproject.toml` had, so the first `flwr run` on 1.37+ fails with
`502 Bad Gateway`. The migration also comments the block out, after which
`~/.flwr/config.toml` is authoritative.

To run on 1.37+, therefore: run `flwr run` once (it migrates and fails), then
edit `~/.flwr/config.toml` to the HTTP port, and run again:

```toml
# ~/.flwr/config.toml
[superlink.local-deployment]
address = "127.0.0.1:8000"
insecure = true
```

Editing `config.toml` *before* the first `flwr run` does not work: the migration
overwrites it. Re-adding the `[tool.flwr.federations]` block to `pyproject.toml`
after migrating also re-triggers the migration, which resets the port again.

Other notes for newer Flower versions:

- `requires-python` must be `>=3.11`; Flower does not support 3.10, and `uv sync`
  fails during dependency resolution if the project claims 3.10 support.
- The protos are generated from `${FLWR_SOURCE_ROOT}/framework/proto` at build
  time, so building inside a Flower checkout always uses that checkout's protos.
- Flower 1.37 added `session_id` to `PushMessagesResponse` and
  `PushObjectRequest`. This client does not send it; the SuperLink handles that
  case explicitly ("Support legacy SuperNodes that do not send a session ID") in
  both the in-memory and SQL core state backends.

## Run the `Flower SuperLink`, the two clients, and the `Flower ServerApp` in separate terminals

```bash
flower-superlink --insecure
```

```bash
build/flwr_client 0 127.0.0.1:9092
```

```bash
build/flwr_client 1 127.0.0.1:9092
```

```bash
flwr run . --stream
```

The `client.py` file only provides a placeholder `ClientApp` entry point for
Flower App metadata. The actual training clients are the external C++
`build/flwr_client` processes.
