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
#ifndef NUBRICK_MASTER_H
#define NUBRICK_MASTER_H

#include "mbed.h"
#include "mbed_debug.h"
#include "NuBrickField.h"
#include "nubrick_prot.h"
#include "targets/TARGET_NUVOTON/nu_bitutil.h"

/** Print error message and return null field
 *
 *  @note For internal use
 */
#define NUBRICK_ERROR_RETURN_NULL_FIELD(...)                \
    do {                                                    \
        debug_if(_debug, __VA_ARGS__);                      \
        return _null_field;                                 \
    } while (0);

/** Print error message and return false
 *
 *  @note For internal use
 */
#define NUBRICK_ERROR_RETURN_FALSE(...)                     \
    do {                                                    \
        debug_if(_debug, __VA_ARGS__);                      \
        return false;                                       \
    } while (0);

/** Check if NuMaker Brick I2C slave module has connected
 *
 *  @note For internal use
 */
#define NUBRICK_CHECK_CONNECT()                                                                     \
    do {                                                                                            \
        if (! _connected) {                                                                         \
            NUBRICK_ERROR_RETURN_FALSE("NuMaker Brick I2C slave module not connected yet!!!\r\n");  \
        }                                                                                           \
    } while (0);
    
/** Check if I2C buffer is overflow on getN calls
 *
 *  @note For internal use
 */
#define NUBRICK_CHECK_GETN_NEXT(N)                                      \
    do {                                                                \
        if ((_i2c_buf_pos + N) > _i2c_buf_end) {                        \
            error("%s:%s: I2C buffer overflow", __FILE__, __func__);    \
            return 0;                                                   \
        }                                                               \
    } while (0);

/** Check if I2C buffer is overflow on setN calls
 *
 *  @note For internal use
 */
#define NUBRICK_CHECK_SETN_NEXT(N)                                  \
    do {                                                            \
        if ((_i2c_buf_pos + N) > _i2c_buf_end) {                    \
            error("%s: I2C buffer overflow", __func__);             \
        }                                                           \
    } while (0);
    
/** A NuMaker Brick I2C master, used for communicating with NuMaker Brick I2C slave modules
 *
 * @note Synchronization level: Thread safe
 *
 */
class NuBrickMaster {

public:

    /** Create an I2C interface, connected to the specified pins
     *
     *  @param i2c I2C object
     *  @param address 8-bit I2C slave address [ addr | 0 ]
     */
    NuBrickMaster(I2C &i2c, int i2c_addr, bool debug);

    virtual ~NuBrickMaster();
    
    /** Connect to the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     *
     *  @note On success, device descriptor and report descriptor will be fetched.
     */
    bool connect(void);
    
    /** Is the NuBrick I2C slave module connected?
     *
     *  @return true if success, false if failure
     */
    bool connected(void) {
        return _connected;
    }
    
    /** Get one field of the NuBrick I2C slave module in "report.field" format, e.g. "feature.sleep_period". 
     *
     *  @return non-NULL if success, NULL if failure
     *
     *  @note See subclasses for supported report.field names
     *  @note If passed name is incorrect or not supported, a null object is returned.
     *        Operations on this null object will do nothing.
     *
     */
    NuBrickField &operator[](const char *report_field_name);
    
    /** Pull device descriptor from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool pull_device_desc(void);
    
    /** Pull report descriptor from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool pull_report_desc(void);
    
    /** Pull input report from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool pull_input_report(void);

    /** Push output report to the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool push_output_report(void);
    
    /** Pull feature report from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool pull_feature_report(void);
    
    /** Push feature report to the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool push_feature_report(void);
    
    /** Print device descriptor
     */
    bool print_device_desc(void);
    
    /** Print feature report
     */
    bool print_feature_report(void);
    
    /** Print input report
     */
    bool print_input_report(void);
    
    /** Print output report
     */
    bool print_output_report(void);
    
protected:
    I2C &                               _i2c;
    int                                 _i2c_addr;
    uint8_t                             _i2c_buf[80];
    uint8_t *                           _i2c_buf_pos;
    uint8_t * const                     _i2c_buf_end;
    bool                                _i2c_buf_overflow;
    bool                                _connected;
    bool                                _debug;
    NuBrick_Device_Descriptor           _dev_desc;
    NuBrickField                        _null_field;
    NuBrickField *                      _feature_report_fields;
    unsigned                            _num_feature_report_fields;
    NuBrickField *                      _input_report_fields;
    unsigned                            _num_input_report_fields;
    NuBrickField *                      _output_report_fields;
    unsigned                            _num_output_report_fields;
    
    /** Using RAII idiom for mutex lock/unlock
     */
    class MutexGuard {
    public:
        MutexGuard() {
            _mutex->lock();
        }
        
        ~MutexGuard() {
            _mutex->unlock();
        }
    };
    
    static SingletonPtr<rtos::Mutex>  _mutex;
    
    /** Add fields of feature report
     */
    void add_feature_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name);
    
    /** Remove fields of feature report
     */
    void remove_feature_fields(void);
    
    /** Add fields of input report
     */
    void add_input_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name);
    
    /** Remove fields of input report
     */
    void remove_input_fields(void);
    
    /** Add fields of output report
     */
    void add_output_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name);
    
    /** Remove fields of output report
     */
    void remove_output_fields(void);
    
    /** Add fields of specific report
     */
    void add_report_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name, 
        NuBrickField *&report_fields, unsigned &num_report_fields);
    
    /** Remove fields of specific report
     */
    void remove_report_fields(NuBrickField *&report_fields, unsigned &num_report_fields);
    
    /** Un-serialize device descriptor from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    bool unserialize_device_desc(void);
    
    /** Un-serialize report descriptor from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    virtual bool unserialize_report_desc(void);
    
    /** Un-serialize input report from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    virtual bool unserialize_input_report(void);
    
    /** Serialize output report to the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    virtual bool serialize_output_report(void);
    
    /** Un-serialize feature report from the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    virtual bool unserialize_feature_report(void);
    
    /** Serialize feature report to the NuBrick I2C slave module
     *
     *  @return true if success, false if failure
     */
    virtual bool serialize_feature_report(void);
    
    /** Un-serialize field from report descriptor
     */
    bool unserialize_field_from_report_desc(NuBrickField *field);
    
    /** Un-serialize field from report
     */
    bool unserialize_field_from_report(NuBrickField *field);
    
    /** Serialize field to report
     */
    bool serialize_field_to_report(const NuBrickField *field);
    
    /** Un-serialize from little-endian stream to uint8_t type and advance stream position
     */
    uint8_t get8_next(void);
    
    /** Serialize from uint8_t type to little-endian stream and advance stream position
     */
    void set8_next(uint8_t val);
    
    /** Un-serialize from little-endian stream to uint16_t type and advance stream position
     */
    uint16_t get16_le_next(void);
    
    /** Serialize from uint16_t type to little-endian stream and advance stream position
     */
    void set16_le_next(uint16_t val);
    
    /** Un-serialize from big-endian stream to uint16_t type and advance stream position
     */
    uint16_t get16_be_next(void);
    
    /** Print specified report
     *
     *  @param name report name
     */
    void print_report(NuBrickField *fields, unsigned num_fields, const char *report_name);
};

#endif
