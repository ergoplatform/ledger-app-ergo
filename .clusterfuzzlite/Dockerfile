# TO-DO: use latest, once ravamped IO supported by fuzzing tests
FROM ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-lite:4.7.0 AS LITE_BUILDER

# Base image with clang toolchain
FROM gcr.io/oss-fuzz-base/base-builder:v1

# Copy the project's source code.
COPY . $SRC/ledger-app-ergo
COPY --from=LITE_BUILDER /opt/ledger-secure-sdk $SRC/ledger-app-ergo/BOLOS_SDK

# Working directory for build.sh
WORKDIR $SRC/ledger-app-ergo

# Copy build.sh into $SRC dir.
COPY ./.clusterfuzzlite/build.sh $SRC/