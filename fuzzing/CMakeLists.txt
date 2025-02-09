cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(ergo_fuzzers C)

set(CMAKE_C_STANDARD 11)

if (NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Fuzzer needs to be built with Clang")
endif()

if (NOT DEFINED BOLOS_SDK)
  message(FATAL_ERROR "BOLOS_SDK environment variable not found.")
endif()

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()


# compatible with ClusterFuzzLite
if (NOT DEFINED ENV{LIB_FUZZING_ENGINE})
    add_compile_options(-fsanitize=address,fuzzer-no-link)
    add_link_options(-fsanitize=address,fuzzer)
else()
    add_link_options($ENV{LIB_FUZZING_ENGINE})
endif()

add_compile_options(-g)


set(SDK_PATH ${BOLOS_SDK})
set(UX_PATH ${SDK_PATH}/lib_ux)
set(ERGO_PATH ..)

set(UX_SOURCE
    ${UX_PATH}/src/ux_flow_engine.c
    ${UX_PATH}/src/ux_layout_bb.c
    ${UX_PATH}/src/ux_layout_bn.c
    ${UX_PATH}/src/ux_layout_bnn.c
    ${UX_PATH}/src/ux_layout_bnnn.c
    ${UX_PATH}/src/ux_layout_nn.c
    ${UX_PATH}/src/ux_layout_paging.c
    ${UX_PATH}/src/ux_layout_paging_compute.c
    ${UX_PATH}/src/ux_layout_pbb.c
    ${UX_PATH}/src/ux_layout_pb.c
    ${UX_PATH}/src/ux_layout_pn.c
    ${UX_PATH}/src/ux_layout_pnn.c
    ${UX_PATH}/src/ux_layout_utils.c
    ${UX_PATH}/src/ux_stack.c
)

set(ERGO_SOURCE
    ${ERGO_PATH}/src/commands/attestinput/ainpt_handler.c
    ${ERGO_PATH}/src/commands/attestinput/ainpt_response.c
    ${ERGO_PATH}/src/commands/attestinput/ainpt_ui_bagl.c
    ${ERGO_PATH}/src/commands/deriveaddress/da_handler.c
    ${ERGO_PATH}/src/commands/deriveaddress/da_response.c
    ${ERGO_PATH}/src/commands/deriveaddress/da_ui_bagl.c
    ${ERGO_PATH}/src/commands/extpubkey/epk_handler.c
    ${ERGO_PATH}/src/commands/extpubkey/epk_response.c
    ${ERGO_PATH}/src/commands/extpubkey/epk_ui_bagl.c
    ${ERGO_PATH}/src/commands/signtx/operations/stx_op_p2pk.c
    ${ERGO_PATH}/src/commands/signtx/stx_amounts.c
    ${ERGO_PATH}/src/commands/signtx/stx_handler.c
    ${ERGO_PATH}/src/commands/signtx/stx_output.c
    ${ERGO_PATH}/src/commands/signtx/stx_response.c
    ${ERGO_PATH}/src/commands/signtx/stx_ui_bagl.c
    ${ERGO_PATH}/src/commands/signtx/stx_ui_common.c
    ${ERGO_PATH}/src/common/bip32_ext.c
    ${ERGO_PATH}/src/common/buffer_ext.c
    ${ERGO_PATH}/src/common/gve.c
    ${ERGO_PATH}/src/common/rwbuffer.c
    ${ERGO_PATH}/src/common/bip32_ext.c
    ${ERGO_PATH}/src/helpers/blake2b.c
    ${ERGO_PATH}/src/helpers/crypto.c
    ${ERGO_PATH}/src/helpers/input_frame.c
    ${ERGO_PATH}/src/ergo/address.c
    ${ERGO_PATH}/src/ergo/ergo_tree.c
    ${ERGO_PATH}/src/ergo/schnorr.c
    ${ERGO_PATH}/src/ergo/tx_ser_box.c
    ${ERGO_PATH}/src/ergo/tx_ser_full.c
    ${ERGO_PATH}/src/ergo/tx_ser_input.c
    ${ERGO_PATH}/src/ergo/tx_ser_table.c
    ${ERGO_PATH}/src/ui/display.c
    ${ERGO_PATH}/src/ui/ui_dynamic_flow.c
    ${ERGO_PATH}/src/ui/ui_approve_reject.c
    ${ERGO_PATH}/src/ui/ui_application_id_bagl.c
    ${ERGO_PATH}/src/ui/ui_bip32_path_bagl.c
    ${ERGO_PATH}/src/ui/ui_sign_reject.c
    ${ERGO_PATH}/src/ui/ui_main_bagl.c
    ${ERGO_PATH}/src/ui/ui_menu_bagl.c
    ${ERGO_PATH}/src/context.c
    ${BOLOS_SDK}/lib_standard_app/base58.c
    ${BOLOS_SDK}/lib_standard_app/read.c
    ${BOLOS_SDK}/lib_standard_app/write.c
    ${BOLOS_SDK}/lib_standard_app/format.c
)

file(GLOB SHIMS_SRC ${ERGO_PATH}/unit-tests/utils/*.c)

include_directories(
    ${BOLOS_SDK}/include
    ${BOLOS_SDK}/target/nanox/include
    ${BOLOS_SDK}/lib_cxng/include
    ${BOLOS_SDK}/lib_bagl/include
    ${BOLOS_SDK}/lib_ux/include
    ${BOLOS_SDK}/lib_standard_app
    
    ${ERGO_PATH}/src
    ${ERGO_PATH}/src/commands
    ${ERGO_PATH}/src/commands/attestinput
    ${ERGO_PATH}/src/commands/deriveaddress
    ${ERGO_PATH}/src/commands/extpubkey
    ${ERGO_PATH}/src/commands/signtx
    ${ERGO_PATH}/src/common
    ${ERGO_PATH}/src/ergo
    ${ERGO_PATH}/src/helpers
    ${ERGO_PATH}/src/ui
    ./include
)
#include_directories(BEFORE SYSTEM ${ERGO_PATH}/unit-tests/utils)

add_compile_definitions(
    FUZZING
    HAVE_BAGL
    BAGL_WIDTH=128
    BAGL_HEIGHT=64
    HAVE_UX_FLOW

    APPNAME=\"Ergo\"
    APPVERSION=\"1.0.0\"
    MAJOR_VERSION=1
    MINOR_VERSION=0
    PATCH_VERSION=0

    IO_HID_EP_LENGTH=64
    IO_SEPROXYHAL_BUFFER_SIZE_B=300
    OS_IO_SEPROXYHAL

    HAVE_ECC
    HAVE_CRC
    HAVE_BLAKE2
    HAVE_ECC_WEIERSTRASS
    HAVE_SECP256K1_CURVE
    HAVE_SECP256R1_CURVE
    HAVE_ECC_TWISTED_EDWARDS
    HAVE_ED25519_CURVE
    HAVE_ECDSA
    HAVE_EDDSA
    HAVE_HASH
    HAVE_HMAC
    HAVE_SHA224
    HAVE_SHA256
    HAVE_SHA3
    HAVE_RNG
    HAVE_MATH
  )

add_compile_options(
    -Wno-format -Wno-pointer-to-int-cast -Wno-constant-conversion -Wno-tautological-constant-out-of-range-compare
)

set(SOURCE
    ${UX_SOURCE}
    ${SHIMS_SRC}
    ${ERGO_SOURCE}
    ./src/utils/os_mocks.c
    ./src/utils/glyphs.c
    ./src/utils/crc32.c
)


add_library(ergo ${SOURCE})

set(harnesses
    ainpt_harness
    da_harness
    epk_harness
    stx_harness
)

foreach(harness IN LISTS harnesses)
    add_executable(${harness}
        ./src/${harness}.c
    )
    target_link_libraries(${harness} PUBLIC ergo)
endforeach()
