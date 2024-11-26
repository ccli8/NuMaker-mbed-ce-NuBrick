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
#include "NuBrickMaster.h"
#include <cstring>

SingletonPtr<rtos::Mutex> NuBrickMaster::_mutex;

NuBrickMaster::NuBrickMaster(I2C &i2c, int i2c_addr, bool debug)
    : _i2c(i2c), _i2c_addr(i2c_addr), 
        _i2c_buf_pos(_i2c_buf), _i2c_buf_end(_i2c_buf + sizeof (_i2c_buf) / sizeof (_i2c_buf[0])), _i2c_buf_overflow(false),
        _connected(false), _debug(debug), _null_field(0, ""),
        _feature_report_fields(NULL), _num_feature_report_fields(0), 
        _input_report_fields(NULL), _num_input_report_fields(0),
        _output_report_fields(NULL), _num_output_report_fields(0) {
        
    // No lock needed in the constructor

    // Set I2C bus clock to 100K.
    _i2c.frequency(100000);
    
    memset(_i2c_buf, 0x00, sizeof (_i2c_buf));
}

NuBrickMaster::~NuBrickMaster() {
    
    // Remove fields of feature report allocated by subclass
    remove_feature_fields();
    // Remove fields of input report allocated by subclass
    remove_input_fields();
    // Remove fields of output report allocated by subclass
    remove_output_fields();
}
    
bool NuBrickMaster::connect(void) {
    // Support thread-safe
    MutexGuard guard;
    
    if (_connected) {
        return true;
    }
    
    // Get device descriptor
    if (! pull_device_desc()) {
        _connected = false;
        NUBRICK_ERROR_RETURN_FALSE("pull_device_desc() failed\r\n");
    }
    // Get report descriptor
    if (! pull_report_desc()) {
        _connected = false;
        NUBRICK_ERROR_RETURN_FALSE("pull_report_desc() failed\r\n");
    }
    
    _connected = true;
    return true;
}

NuBrickField &NuBrickMaster::operator[](const char *report_field_name) {
    // Support thread-safe
    MutexGuard guard;
    
    if (! report_field_name) {
        NUBRICK_ERROR_RETURN_NULL_FIELD("NULL string not support\r\n");
    }
    
    const char *dot_plus_field_name = strchr(report_field_name, '.');
    if (dot_plus_field_name == NULL) {
        NUBRICK_ERROR_RETURN_NULL_FIELD("%s not support\r\n", report_field_name);
    }
  
    const char *field_name = dot_plus_field_name + 1;
    unsigned report_name_len = dot_plus_field_name - report_field_name;
    
    if (strncmp("feature", report_field_name, report_name_len) == 0) {
        NuBrickField *field = _feature_report_fields;
        NuBrickField *field_end = _feature_report_fields + _num_feature_report_fields;
    
        for (; field != field_end; field ++) {
            const char *field_name_iter = field->_name;
            
            if (strcmp(field_name, field_name_iter) == 0) {
                return *field;
            }
        }
        NUBRICK_ERROR_RETURN_NULL_FIELD("%s not support\r\n", report_field_name);
    }
    else if (strncmp("input", report_field_name, report_name_len) == 0) {
        NuBrickField *field = _input_report_fields;
        NuBrickField *field_end = _input_report_fields + _num_input_report_fields;
    
        for (; field != field_end; field ++) {
            const char *field_name_iter = field->_name;
            
            if (strcmp(field_name, field_name_iter) == 0) {
                return *field;
            }
        }
        NUBRICK_ERROR_RETURN_NULL_FIELD("%s not support\r\n", report_field_name);
        
    }
    else if (strncmp("output", report_field_name, report_name_len) == 0) {
        NuBrickField *field = _output_report_fields;
        NuBrickField *field_end = _output_report_fields + _num_output_report_fields;
    
        for (; field != field_end; field ++) {
            const char *field_name_iter = field->_name;
            
            if (strcmp(field_name, field_name_iter) == 0) {
                return *field;
            }
        }
        NUBRICK_ERROR_RETURN_NULL_FIELD("%s not support\r\n", report_field_name);
    }
    else {
        NUBRICK_ERROR_RETURN_NULL_FIELD("%s not support\r\n", report_field_name);
    }
    
}
    
bool NuBrickMaster::pull_device_desc(void) {
    // Support thread-safe
    MutexGuard guard;
    
    // Send GetDeviceDescriptor command
    nu_set16_le(_i2c_buf, NuBrick_Comm_GetDeviceDesc);    
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, 2, true)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    // Receive device descriptor
    if (_i2c.read(_i2c_addr, (char *) _i2c_buf, NuBrick_DeviceDesc_Len, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.read() failed\r\n");
    }
    
    // Un-serialize device descriptor
    _i2c_buf_pos = _i2c_buf;
    if (! unserialize_device_desc()) {
        NUBRICK_ERROR_RETURN_FALSE("unserialize_device_desc() failed\r\n");
    }
    
    return true;
}

bool NuBrickMaster::pull_report_desc(void) {
    // Support thread-safe
    MutexGuard guard;
    
    // Send GetReportDescriptor command
    nu_set16_le(_i2c_buf, NuBrick_Comm_GetReportDesc);    
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, 2, true)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    // Receive report descriptor
    if (_i2c.read(_i2c_addr, (char *) _i2c_buf, _dev_desc.report_desc_len, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.read() failed\r\n");
    }
    
    // Un-serialize report descriptor
    _i2c_buf_pos = _i2c_buf;
    if (! unserialize_report_desc()) {
        NUBRICK_ERROR_RETURN_FALSE("unserialize_report_desc() failed\r\n");
    }
    
    return true;
}
    
bool NuBrickMaster::pull_input_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    // Send GetInputReport command
    nu_set16_le(_i2c_buf, NuBrick_Comm_GetInputReport);    
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, 2, true)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    // Receive input report
    if (_i2c.read(_i2c_addr, (char *) _i2c_buf, _dev_desc.input_report_len, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.read() failed\r\n");
    }
    
    // Un-serialize input report
    _i2c_buf_pos = _i2c_buf;
    if (! unserialize_input_report()) {
        NUBRICK_ERROR_RETURN_FALSE("unserialize_input_report() failed\r\n");
    }
    
    return true;
}

bool NuBrickMaster::push_output_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    _i2c_buf_pos = _i2c_buf;
    
    // Send SetOutputReport command
    set16_le_next(NuBrick_Comm_SetOutputReport);    
    
    // Serialize output report
    if (! serialize_output_report()) {
        NUBRICK_ERROR_RETURN_FALSE("serialize_output_report() failed\r\n");
    }
    
    // Send Output report
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, _i2c_buf_pos - _i2c_buf, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    return true;
}
    
bool NuBrickMaster::pull_feature_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    // Send GetFeatureReport command
    nu_set16_le(_i2c_buf, NuBrick_Comm_GetFeatureReport);    
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, 2, true)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    // Receive feature report
    if (_i2c.read(_i2c_addr, (char *) _i2c_buf, _dev_desc.getfeat_report_len, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.read() failed\r\n");
    }
    
    // Un-serialize feature report
    _i2c_buf_pos = _i2c_buf;
    if (! unserialize_feature_report()) {
        NUBRICK_ERROR_RETURN_FALSE("unserialize_feature_report() failed\r\n");
    }
    
    return true;
}

bool NuBrickMaster::push_feature_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    _i2c_buf_pos = _i2c_buf;
    
    // Send SetFeatureReport command
    set16_le_next(NuBrick_Comm_SetFeatureReport);    
   
    // Serialize feature report
    if (! serialize_feature_report()) {
        NUBRICK_ERROR_RETURN_FALSE("serialize_feature_report() failed\r\n");
    }
    
    // Send feature report
    if (_i2c.write(_i2c_addr, (char *) _i2c_buf, _i2c_buf_pos - _i2c_buf, false)) {
        NUBRICK_ERROR_RETURN_FALSE("i2c.write() failed\r\n");
    }
    
    return true;
}

bool NuBrickMaster::print_device_desc(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    printf("Device descriptor length\t\t%d\r\n", _dev_desc.dev_desc_len);
    printf("Report descriptor length\t\t%d\r\n", _dev_desc.report_desc_len);
    printf("Input report length\t\t\t%d\r\n", _dev_desc.input_report_len);
    printf("Output report length\t\t\t%d\r\n", _dev_desc.output_report_len);
    printf("Get feature report length\t\t%d\r\n", _dev_desc.getfeat_report_len);
    printf("Set feature report length\t\t%d\r\n", _dev_desc.setfeat_report_len);
    printf("Company ID\t\t\t\t%d\r\n", _dev_desc.cid);
    printf("Device ID\t\t\t\t%d\r\n", _dev_desc.did);
    printf("Product ID\t\t\t\t%d\r\n", _dev_desc.pid);
    printf("Product ID\t\t\t\t%d\r\n", _dev_desc.uid);
    printf("Product ID\t\t\t\t%d\r\n", _dev_desc.ucid);
    
    return true;
}
    
bool NuBrickMaster::print_feature_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    print_report(_feature_report_fields, _num_feature_report_fields, "feature report");
    
    return true;
}
    
    
bool NuBrickMaster::print_input_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    print_report(_input_report_fields, _num_input_report_fields, "input report");
    
    return true;
}
    
    
bool NuBrickMaster::print_output_report(void) {
    // Support thread-safe
    MutexGuard guard;
    
    NUBRICK_CHECK_CONNECT();
    
    print_report(_output_report_fields, _num_output_report_fields, "output report");
    
    return true;
}

void NuBrickMaster::add_feature_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name) {
    
    remove_report_fields(_feature_report_fields, _num_feature_report_fields);
    add_report_fields(field_index_name, num_index_name, _feature_report_fields, _num_feature_report_fields);
}

void NuBrickMaster::remove_feature_fields(void) {
    
    remove_report_fields(_feature_report_fields, _num_feature_report_fields);
}

void NuBrickMaster::add_input_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name) {
    
    remove_report_fields(_input_report_fields, _num_input_report_fields);
    add_report_fields(field_index_name, num_index_name, _input_report_fields, _num_input_report_fields);
}

void NuBrickMaster::remove_input_fields(void) {
    
   remove_report_fields(_input_report_fields, _num_input_report_fields);
}

void NuBrickMaster::add_output_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name) {
    
    remove_report_fields(_output_report_fields, _num_output_report_fields);
    add_report_fields(field_index_name, num_index_name, _output_report_fields, _num_output_report_fields);
}

void NuBrickMaster::remove_output_fields(void) {
    
    remove_report_fields(_output_report_fields, _num_output_report_fields);
}

void NuBrickMaster::add_report_fields(const NuBrickField::IndexName *field_index_name, unsigned num_index_name, 
        NuBrickField *&report_fields, unsigned &num_report_fields) {
    
    MBED_ASSERT(report_fields == NULL);
    MBED_ASSERT(num_report_fields == 0);
    
    unsigned i;
    
    num_report_fields = num_index_name;
    void *raw_memory = ::operator new(sizeof (NuBrickField) * num_index_name);
    report_fields = static_cast<NuBrickField *>(raw_memory);
    for (i = 0; i < num_index_name; i ++) {
        NuBrickField *field = report_fields + i;
        new (field) NuBrickField(field_index_name[i].first,  field_index_name[i].second);
    }
}
    
void NuBrickMaster::NuBrickMaster::remove_report_fields(NuBrickField *&report_fields, unsigned &num_report_fields) {
    
    unsigned i;
    
    for (i = 0; i < num_report_fields; i ++) {
        NuBrickField *field = report_fields + i;
        field->~NuBrickField();
    }
    
    ::operator delete((void *) report_fields);
    report_fields = NULL;
    num_report_fields = 0;
}

bool NuBrickMaster::unserialize_device_desc(void) {

    // Device descriptor length
    _dev_desc.dev_desc_len = get16_le_next();
    
    // Report descriptor length
    _dev_desc.report_desc_len = get16_le_next();
    
    // Input report length
    _dev_desc.input_report_len = get16_le_next();
    
    // Output report length
    _dev_desc.output_report_len = get16_le_next();
    
    // Get feature report length
    _dev_desc.getfeat_report_len = get16_le_next();
    
    // Set feature report length
    _dev_desc.setfeat_report_len = get16_le_next();
    
    // CID
    _dev_desc.cid = get16_le_next();
    
    // DID
    _dev_desc.did = get16_le_next();
    
    // PID
    _dev_desc.pid = get16_le_next();
    
    // UID
    _dev_desc.uid = get16_le_next();
    
    // UCID
    _dev_desc.ucid = get16_le_next();
    
    // Reserved 1
    _dev_desc.reserved1 = get16_le_next();
    
    // Reserved 2
    _dev_desc.reserved2 = get16_le_next();
    
    if (_dev_desc.dev_desc_len != NuBrick_DeviceDesc_Len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of device descriptor doesn't match\r\n");
    }
    
    return true;
}


bool NuBrickMaster::unserialize_report_desc(void) {
    
    // Report descriptor length
    uint16_t report_desc_len = get16_le_next();
    if (report_desc_len != _dev_desc.report_desc_len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of report descriptor doesn't match\r\n");
    }
    
    uint16_t desc_type;
    NuBrickField *field = NULL;
    NuBrickField *field_end = NULL;
    
    // Check no feature report descriptor
    if (! _num_feature_report_fields) {
        return true;
    }
    
    // Feature report descriptor type
    desc_type = get16_be_next();
    if (desc_type != NuBrick_DescType_FeatureReport) {
        NUBRICK_ERROR_RETURN_FALSE("Expect feature report descriptor type %d, but %d received\r\n", NuBrick_DescType_FeatureReport, desc_type);
    }
    
    // Un-serialize feature report fields from report descriptor
    field = _feature_report_fields;
    field_end = _feature_report_fields + _num_feature_report_fields;
    for (; field != field_end; field ++) {
        if (! unserialize_field_from_report_desc(field)) {
            NUBRICK_ERROR_RETURN_FALSE("unserialize_field_from_report_desc() failed\r\n");
        }
    }

    // Check no input report descriptor
    if (! _num_input_report_fields) {
        return true;
    }
    
    // Input report descriptor type
    desc_type = get16_be_next();
    if (desc_type != NuBrick_DescType_InputReport) {
        NUBRICK_ERROR_RETURN_FALSE("Expect input report descriptor type %d, but %d received\r\n", NuBrick_DescType_InputReport, desc_type);
    }
    
    // Un-serialize input report fields from report descriptor
    field = _input_report_fields;
    field_end = _input_report_fields + _num_input_report_fields;
    for (; field != field_end; field ++) {
        if (! unserialize_field_from_report_desc(field)) {
            NUBRICK_ERROR_RETURN_FALSE("unserialize_field_from_report_desc() failed\r\n");
        }
    }
    
    // Check no output report descriptor
    if (! _num_output_report_fields) {
        return true;
    }
    
    // Output report descriptor type
    desc_type = get16_be_next();
    if (desc_type != NuBrick_DescType_OutputReport) {
        NUBRICK_ERROR_RETURN_FALSE("Expect output report descriptor type %d, but %d received\r\n", NuBrick_DescType_OutputReport, desc_type);
    }
    
    // Un-serialize output report fields from report descriptor
    field = _output_report_fields;
    field_end = _output_report_fields + _num_output_report_fields;
    for (; field != field_end; field ++) {
        if (! unserialize_field_from_report_desc(field)) {
            NUBRICK_ERROR_RETURN_FALSE("unserialize_field_from_report_desc() failed\r\n");
        }
    }
    
    return true;
}
    
bool NuBrickMaster::unserialize_input_report(void) {
    
    // Input report length
    uint16_t report_len = get16_le_next();
    if (report_len != _dev_desc.input_report_len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of input report doesn't match\r\n");
    }

    // Un-serialize fields from input report
    NuBrickField *field = _input_report_fields;
    NuBrickField *field_end = _input_report_fields + _num_input_report_fields;
    for (; field != field_end; field ++) {
        if (! unserialize_field_from_report(field)) {
            NUBRICK_ERROR_RETURN_FALSE("unserialize_field_from_report() failed\r\n");
        }
    }
    
    return true;
}
    
bool NuBrickMaster::serialize_output_report(void) {
    
    uint8_t *i2c_buf_beg = _i2c_buf_pos;
    
    // Output report length
    set16_le_next(_dev_desc.output_report_len);

    // Serialize fields to output report
    NuBrickField *field = _output_report_fields;
    NuBrickField *field_end = _output_report_fields + _num_output_report_fields;
    for (; field != field_end; field ++) {
        if (! serialize_field_to_report(field)) {
            NUBRICK_ERROR_RETURN_FALSE("serialize_field_to_report() failed\r\n");
        }
    }
    
    if ((_i2c_buf_pos - i2c_buf_beg) != _dev_desc.output_report_len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of output report doesn't match\r\n");
    }
    
    return true;
}
    
bool NuBrickMaster::unserialize_feature_report(void) {
    
    // Feature report length
    uint16_t report_len = get16_le_next();
    if (report_len != _dev_desc.getfeat_report_len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of feature report doesn't match\r\n");
    }
    
    // Un-serialize fields from feature report
    NuBrickField *field = _feature_report_fields;
    NuBrickField *field_end = _feature_report_fields + _num_feature_report_fields;
    for (; field != field_end; field ++) {
        if (! unserialize_field_from_report(field)) {
            NUBRICK_ERROR_RETURN_FALSE("unserialize_field_from_report() failed\r\n");
        }
    }
    
    return true;
}
    
bool NuBrickMaster::serialize_feature_report(void) {
    
    uint8_t *i2c_buf_beg = _i2c_buf_pos;
    
    // Feature report length
    set16_le_next(_dev_desc.setfeat_report_len);

    // Serialize fields to feature report
    NuBrickField *field = _feature_report_fields;
    NuBrickField *field_end = _feature_report_fields + _num_feature_report_fields;
    for (; field != field_end; field ++) {
        if (! serialize_field_to_report(field)) {
            NUBRICK_ERROR_RETURN_FALSE("serialize_field_to_report() failed\r\n");
        }
    }
    
    if ((_i2c_buf_pos - i2c_buf_beg) != _dev_desc.setfeat_report_len) {
        NUBRICK_ERROR_RETURN_FALSE("Length of set feature report doesn't match\r\n");
    }
    
    return true;
}

bool NuBrickMaster::unserialize_field_from_report_desc(NuBrickField *field) {
    // Number/length of the field
    uint8_t field_index = get8_next();
    if (field_index != field->_field_index) {
        NUBRICK_ERROR_RETURN_FALSE("Expect field index %d, but %d received\r\n", field->_field_index, field_index);
    }
    
    // Length of the field
    field->_length = get8_next();
    if (field->_length != 1 && field->_length != 2) {
        NUBRICK_ERROR_RETURN_FALSE("Expect field length 1/2, but %d received\r\n", field->_length);
    }
    
    // Minimum of the field
    uint8_t min = get8_next();
    switch (min) {
        case NuBrick_ReportDesc_Min_Plus1:
            field->_minimum = get8_next();
            break;
        
        case NuBrick_ReportDesc_Min_Plus2:
            field->_minimum = get16_le_next();
            break;
            
        default:
            NUBRICK_ERROR_RETURN_FALSE("Expect field minimum %d/%d, but %d received\r\n", NuBrick_ReportDesc_Min_Plus1, NuBrick_ReportDesc_Min_Plus2, min);
    }
    
    // Maximum of the field
    uint8_t max = get8_next();
    switch (max) {
        case NuBrick_ReportDesc_Max_Plus1:
            field->_maximum = get8_next();
            break;
        
        case NuBrick_ReportDesc_Max_Plus2:
            field->_maximum = get16_le_next();
            break;
            
        default:
            NUBRICK_ERROR_RETURN_FALSE("Expect field maximum %d/%d, but %d received\r\n", NuBrick_ReportDesc_Max_Plus1, NuBrick_ReportDesc_Max_Plus2, max);
    }
    
    return true;
}

bool NuBrickMaster::unserialize_field_from_report(NuBrickField *field) {
    // Value of the field
    switch (field->_length) {
        case 1:
            field->_value = get8_next();
            break;
            
        case 2:
            field->_value = get16_le_next();
            break;
            
        default:
            NUBRICK_ERROR_RETURN_FALSE("Expect field length 1/2, but %d received\r\n", field->_length);
    }
    
    return true;
}

bool NuBrickMaster::serialize_field_to_report(const NuBrickField *field) {
    // Value of the field
    switch (field->_length) {
        case 1:
            set8_next(field->_value);
            break;
            
        case 2:
            set16_le_next(field->_value);
            break;
            
        default:
            NUBRICK_ERROR_RETURN_FALSE("Expect field length 1/2, but %d received\r\n", field->_length);
    }
    
    return true;
}

uint8_t NuBrickMaster::get8_next(void) {
    NUBRICK_CHECK_GETN_NEXT(1);
    
    uint8_t val = *_i2c_buf_pos ++;
    return val;
}
    
void NuBrickMaster::set8_next(uint8_t val) {
    NUBRICK_CHECK_SETN_NEXT(1);
    
    *_i2c_buf_pos ++ = val;
}
    
uint16_t NuBrickMaster::get16_le_next(void) {
    NUBRICK_CHECK_GETN_NEXT(2);
    
    uint16_t val = nu_get16_le(_i2c_buf_pos);
    _i2c_buf_pos += 2;
    return val;
}
    
void NuBrickMaster::set16_le_next(uint16_t val) {
    NUBRICK_CHECK_SETN_NEXT(2);
    
    nu_set16_le(_i2c_buf_pos, val);
    _i2c_buf_pos += 2;
}
    
uint16_t NuBrickMaster::get16_be_next(void) {
    NUBRICK_CHECK_GETN_NEXT(2);
    
    uint16_t val = nu_get16_be(_i2c_buf_pos);
    _i2c_buf_pos += 2;
    return val;
}
    
void NuBrickMaster::print_report(NuBrickField *fields, unsigned num_fields, const char *report_name) {
    
    printf("Number of fields of %s\t%d\r\n", report_name, num_fields);
    
    unsigned i;
    for (i = 0; i < num_fields; i ++) {
        NuBrickField *field = fields + i;
        
        printf("Name\t\t%s\r\n", field->_name);
        printf("Length\t\t%d\r\n", field->_length);
        printf("Value\t\t%d\r\n", field->_value);
        printf("Minimum\t\t%d\r\n", field->_minimum);
        printf("Maximum\t\t%d\r\n", field->_maximum);
        printf("\r\n");
    }
}
