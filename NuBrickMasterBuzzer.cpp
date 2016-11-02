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
#include "NuBrickMasterBuzzer.h"

NuBrickMasterBuzzer::NuBrickMasterBuzzer(I2C &i2c, bool debug) :
    NuBrickMaster(i2c, NuBrick_I2CAddr_Buzzer, debug) {

    static const NuBrickField::IndexName buzzer_feature_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "sleep_period"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "volume"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex3_Plus1, "tone"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex4_Plus1, "song"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex5_Plus1, "period"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex6_Plus1, "duty"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex7_Plus1, "latency")
    };

    static const NuBrickField::IndexName buzzer_input_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "execute_flag")
    };

    static const NuBrickField::IndexName buzzer_output_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "start_flag"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "stop_flag")
    };

    // Add fields of feature report
    add_feature_fields(buzzer_feature_field_index_name_arr,
        sizeof (buzzer_feature_field_index_name_arr) / sizeof (buzzer_feature_field_index_name_arr[0]));
        
    // Add fields of input report
    add_input_fields(buzzer_input_field_index_name_arr,
        sizeof (buzzer_input_field_index_name_arr) / sizeof (buzzer_input_field_index_name_arr[0]));
        
    // Add fields of output report
    add_output_fields(buzzer_output_field_index_name_arr,
        sizeof (buzzer_output_field_index_name_arr) / sizeof (buzzer_output_field_index_name_arr[0]));
    
    // No lock needed in the constructor

}
