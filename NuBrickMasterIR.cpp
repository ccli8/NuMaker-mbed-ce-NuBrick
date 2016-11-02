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
#include "NuBrickMasterIR.h"

NuBrickMasterIR::NuBrickMasterIR(I2C &i2c, bool debug) :
    NuBrickMaster(i2c, NuBrick_I2CAddr_IR, debug) {

    static const NuBrickField::IndexName ir_feature_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "sleep_period"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "num_learned_data"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex3_Plus1, "using_data_type"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex4_Plus1, "index_orig_data_to_send"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex5_Plus1, "index_learned_data_to_send")
    };

    static const NuBrickField::IndexName ir_input_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "received_data_flag")
    };

    static const NuBrickField::IndexName ir_output_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "send_IR_flag"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "learn_IR_flag")
    };

    // Add fields of feature report
    add_feature_fields(ir_feature_field_index_name_arr,
        sizeof (ir_feature_field_index_name_arr) / sizeof (ir_feature_field_index_name_arr[0]));
        
    // Add fields of input report
    add_input_fields(ir_input_field_index_name_arr,
        sizeof (ir_input_field_index_name_arr) / sizeof (ir_input_field_index_name_arr[0]));
        
    // Add fields of output report
    add_output_fields(ir_output_field_index_name_arr,
        sizeof (ir_output_field_index_name_arr) / sizeof (ir_output_field_index_name_arr[0]));
    
    // No lock needed in the constructor

}
