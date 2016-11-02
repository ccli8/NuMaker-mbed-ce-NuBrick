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
#include "NuBrickMasterSonar.h"

NuBrickMasterSonar::NuBrickMasterSonar(I2C &i2c, bool debug) :
    NuBrickMaster(i2c, NuBrick_I2CAddr_Sonar, debug) {

    static const NuBrickField::IndexName sonar_feature_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "sleep_period"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "distance_AT")
    };

    static const NuBrickField::IndexName sonar_input_field_index_name_arr[] = {
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex1_Plus1, "distance"),
        NuBrickField::IndexName(NuBrick_ReportDesc_FieldIndex2_Plus1, "over_flag")
    };

    // Add fields of feature report
    add_feature_fields(sonar_feature_field_index_name_arr,
        sizeof (sonar_feature_field_index_name_arr) / sizeof (sonar_feature_field_index_name_arr[0]));
        
    // Add fields of input report
    add_input_fields(sonar_input_field_index_name_arr,
        sizeof (sonar_input_field_index_name_arr) / sizeof (sonar_input_field_index_name_arr[0]));
        
    // Add fields of output report
    
    // No lock needed in the constructor

}
