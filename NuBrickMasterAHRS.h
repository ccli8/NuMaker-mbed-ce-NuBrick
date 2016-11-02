/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NUBRICK_MASTER_AHRS_H
#define NUBRICK_MASTER_AHRS_H

#include "mbed.h"
#include "NuBrickMaster.h"


/** A NuMaker Brick I2C master, used for communicating with NuMaker Brick I2C slave module AHRS
 *
 * @Note Synchronization level: Thread safe
 *
 * @details Support fields for access through [] operator:
 *          - feature.sleep_period
 *          - feature.pre_vibration_AT
 *          - input.vibration
 *          - input.over_flag
 */
class NuBrickMasterAHRS : public NuBrickMaster {

public:

    /** Create an I2C interface, connected to the specified pins
     *
     *  @param i2c I2C object
     */
    NuBrickMasterAHRS(I2C &i2c, bool debug);

    virtual ~NuBrickMasterAHRS() {
        // Do nothing
    }
};

#endif
