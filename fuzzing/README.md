# Fuzzing tests

## Fuzzing

Fuzzing allows us to test how a program behaves when provided with invalid, unexpected, or random data as input.

In the case of `ledger-app-ergo` we want to test the code that is responsible for handling attest input, derive address, ext pubkey and sign tx APDU handlers.

If the application crashes, or a [sanitizer](https://github.com/google/sanitizers) detects any kind of
access violation, the fuzzing process is stopped, a report regarding the vulnerability is shown,
and the input that triggered the bug is written to disk under the name `crash-*`.
The vulnerable input file created can be passed as an argument to the fuzzer to triage the issue.

## Manual usage based on Ledger container

### Preparation

The fuzzer can run from the docker `ledger-app-builder-legacy`. You can download it from the `ghcr.io` docker repository:

```console
sudo docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

You can then enter this development environment by executing the following command from the repository root directory:

```console
sudo docker run --rm -ti --user "$(id -u):$(id -g)" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

### Compilation

Once in the container, go into the `fuzzing` folder to compile the fuzzer:

```console
cd fuzzing

# cmake initialization
cmake -DBOLOS_SDK=/opt/ledger-secure-sdk -DCMAKE_C_COMPILER=/usr/bin/clang -Bbuild -H.

# Fuzzer compilation
make -C build
```

### Run

```console
./build/ainpt_harness
./build/da_harness
./build/epk_harness
./build/stx_harness
```

## Notes

For more context regarding fuzzing check out the app-boilerplate fuzzing [README.md](https://github.com/LedgerHQ/app-boilerplate/blob/master/fuzzing/README.md)