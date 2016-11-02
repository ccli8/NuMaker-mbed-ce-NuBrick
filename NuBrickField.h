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
#ifndef NUBRICK_FIELD_H
#define NUBRICK_FIELD_H

#include "mbed.h"
#include <utility>

/** An open field of a NuMaker Brick device
 *
 * @Note Synchronization level: Thread safe
 *
 */
class NuBrickField {
    friend class NuBrickMaster;
    
public:
    typedef std::pair<uint16_t, const char *> IndexName;
    
public:
    NuBrickField(uint8_t field_index, const char *name):
        _field_index(field_index),
        _length(0), 
        _minimum(0),
        _maximum(0),
        _value(0),
        _name(name) {
        // Do nothing
    };
    
    /** Get minimum value of the open field of a NuBrick device
     */
    uint16_t get_minimum(void) {
        return _minimum;
    };
    
    /** Get maximum value of the open field of a NuBrick device
     */
    uint16_t get_maximum(void) {
        return _maximum;
    }
    
    /** Get value of the open field of a NuBrick device
     */
    uint16_t get_value(void) {
        return _value;
    }
    
    /** Set value of the open field of a NuBrick device
     */
    void set_value(uint16_t value) {
        _value = value;
    }
    
private:
    uint16_t        _field_index;
    uint16_t        _length;
    uint16_t        _minimum;
    uint16_t        _maximum;
    uint16_t        _value;
    const char *    _name;
};

#endif
