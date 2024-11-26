# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(nubrick STATIC EXCLUDE_FROM_ALL)

target_include_directories(nubrick
    PUBLIC
        .
)

target_sources(nubrick
    PRIVATE
        NuBrickMaster.cpp
        NuBrickMasterAHRS.cpp
        NuBrickMasterBuzzer.cpp
        NuBrickMasterGas.cpp
        NuBrickMasterIR.cpp
        NuBrickMasterKeys.cpp
        NuBrickMasterLED.cpp
        NuBrickMasterSonar.cpp
        NuBrickMasterTemp.cpp
)

target_link_libraries(nubrick PUBLIC mbed-core-flags)
