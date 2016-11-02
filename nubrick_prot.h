/* mbed Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
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

#ifndef NUBRICK_PROT_H
#define NUBRICK_PROT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Predefined I2C addresses for NuMaker Brick I2C slave devices
 */
enum NuBrick_I2CAddr {
    NuBrick_I2CAddr_Buzzer          = 0x15 << 1,
    NuBrick_I2CAddr_LED             = 0x16 << 1,
    NuBrick_I2CAddr_AHRS            = 0x17 << 1,
    NuBrick_I2CAddr_Sonar           = 0x18 << 1,
    NuBrick_I2CAddr_Temp            = 0x19 << 1,
    NuBrick_I2CAddr_Gas             = 0x1A << 1,
    NuBrick_I2CAddr_IR              = 0x1B << 1,
    NuBrick_I2CAddr_Key             = 0x1C << 1,
    NuBrick_I2CAddr_Reserved9       = 0x1D << 1,
    NuBrick_I2CAddr_Reserved10      = 0x1E << 1,
    NuBrick_I2CAddr_Reserved11      = 0x1F << 1,
    NuBrick_I2CAddr_Reserved12      = 0x20 << 1,
    NuBrick_I2CAddr_Reserved13      = 0x21 << 1,
    NuBrick_I2CAddr_Reserved14      = 0x22 << 1,
};

/** NuMaker Brick protocol: command code
 */
enum NuBrick_Comm {
    NuBrick_Comm_None               = 0,
    NuBrick_Comm_GetDeviceDesc      = 1,    // Get device descriptor
    NuBrick_Comm_GetReportDesc      = 2,    // Get report descriptor (feature/input/output)
    NuBrick_Comm_GetInputReport     = 3,    // Get input report
    NuBrick_Comm_SetOutputReport    = 4,    // Set output report
    NuBrick_Comm_GetFeatureReport   = 5,    // Get feature report
    NuBrick_Comm_SetFeatureReport   = 6,    // Set feature report
};

/** NuMaker Brick protocol: descriptor type
 */
enum NuBrick_DescType {
    NuBrick_DescType_FeatureReport  = 257,
    NuBrick_DescType_InputReport    = 258,
    NuBrick_DescType_OutputReport   = 259,
};

/** NuMaker Brick protocol: report descriptor
 */
enum NuBrick_ReportDesc {
    NuBrick_ReportDesc_Min_Plus1   = 9,
    NuBrick_ReportDesc_Min_Plus2   = 10,
    NuBrick_ReportDesc_Max_Plus1   = 13,
    NuBrick_ReportDesc_Max_Plus2   = 14,
    
    NuBrick_ReportDesc_FieldIndex1_Plus1    = 5,
    NuBrick_ReportDesc_FieldIndex2_Plus1    = 17,
    NuBrick_ReportDesc_FieldIndex3_Plus1    = 29,
    NuBrick_ReportDesc_FieldIndex4_Plus1    = 41,
    NuBrick_ReportDesc_FieldIndex5_Plus1    = 53,
    NuBrick_ReportDesc_FieldIndex6_Plus1    = 65,
    NuBrick_ReportDesc_FieldIndex7_Plus1    = 77,
    NuBrick_ReportDesc_FieldIndex8_Plus1    = 89,
    NuBrick_ReportDesc_FieldIndex9_Plus1    = 101,
    NuBrick_ReportDesc_FieldIndex10_Plus1   = 113,
};

enum {
    NuBrick_DeviceDesc_Len          = 26,   // Device descriptor length
};

struct NuBrick_Device_Descriptor {
    uint16_t dev_desc_len;                  // Device descriptor length
    uint16_t report_desc_len;               // Report descriptor length
    uint16_t input_report_len;              // Input report length
    uint16_t output_report_len;             // Output report length
    uint16_t getfeat_report_len;            // Get feature report length
    uint16_t setfeat_report_len;            // Set feature report length
    uint16_t cid;                           // Company ID
    uint16_t did;                           // Device ID
    uint16_t pid;                           // Product ID
    uint16_t uid;                           // Product ID
    uint16_t ucid;                          // Product ID
    uint16_t reserved1;                     // Reserved 1
    uint16_t reserved2;                     // Reserved 2
};

    
#ifdef __cplusplus
}
#endif

#endif
