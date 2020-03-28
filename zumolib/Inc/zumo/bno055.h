/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: bno055.h
 Pfad:  zumolib/inc/zumo

 Erstellt:    20.09.2019
 Autor:       Tobias Ellermeyer
*/

#ifndef BNO055_H_
#define BNO055_H_

#define BNO_DEBUG

#ifdef __cplusplus
  extern "C" {
#endif
// #define FREERTOS_ENABLED true

#include "bno055_defines.h"

enum bno055_system_status_t {
  BNO055_SYSTEM_STATUS_IDLE = 0x00,
  BNO055_SYSTEM_STATUS_SYSTEM_ERROR = 0x01,
  BNO055_SYSTEM_STATUS_INITIALIZING_PERIPHERALS = 0x02,
  BNO055_SYSTEM_STATUS_SYSTEM_INITIALIZATION = 0x03,
  BNO055_SYSTEM_STATUS_EXECUTING_SELF_TEST = 0x04,
  BNO055_SYSTEM_STATUS_FUSION_ALGO_RUNNING = 0x05,
  BNO055_SYSTEM_STATUS_FUSION_ALOG_NOT_RUNNING = 0x06
};

enum bno055_opmode_t {  // BNO-55 operation modes
  BNO055_OPERATION_MODE_CONFIG = 0x00,
  // Sensor Mode
  BNO055_OPERATION_MODE_ACCONLY,
  BNO055_OPERATION_MODE_MAGONLY,
  BNO055_OPERATION_MODE_GYRONLY,
  BNO055_OPERATION_MODE_ACCMAG,
  BNO055_OPERATION_MODE_ACCGYRO,
  BNO055_OPERATION_MODE_MAGGYRO,
  BNO055_OPERATION_MODE_AMG,  // 0x07
                              // Fusion Mode
  BNO055_OPERATION_MODE_IMU,
  BNO055_OPERATION_MODE_COMPASS,
  BNO055_OPERATION_MODE_M4G,
  BNO055_OPERATION_MODE_NDOF_FMC_OFF,
  BNO055_OPERATION_MODE_NDOF  // 0x0C
};

typedef struct {
  uint8_t mcuState;
  uint8_t gyrState;
  uint8_t magState;
  uint8_t accState;
} bno055_self_test_result_t;

typedef struct {
  uint8_t sys;
  uint8_t gyro;
  uint8_t mag;
  uint8_t accel;
} bno055_calibration_t;

typedef struct {
  double x;
  double y;
  double z;
} bno055_vector_t;

typedef enum {
  BNO055_VECTOR_ACCELEROMETER = 0x08,  // Default: m/s²
  BNO055_VECTOR_MAGNETOMETER = 0x0E,   // Default: uT
  BNO055_VECTOR_GYROSCOPE = 0x14,      // Default: rad/s
  BNO055_VECTOR_EULER = 0x1A,          // Default: degrees
  BNO055_VECTOR_LINEARACCEL = 0x28,    // Default: m/s²
  BNO055_VECTOR_GRAVITY = 0x2E         // Default: m/s²
} bno055_vector_type_t;

enum bno055_system_error_t {
  BNO055_SYSTEM_ERROR_NO_ERROR = 0x00,
  BNO055_SYSTEM_ERROR_PERIPHERAL_INITIALIZATION_ERROR = 0x01,
  BNO055_SYSTEM_ERROR_SYSTEM_INITIALIZATION_ERROR = 0x02,
  BNO055_SYSTEM_ERROR_SELF_TEST_FAILED = 0x03,
  BNO055_SYSTEM_ERROR_REG_MAP_VAL_OUT_OF_RANGE = 0x04,
  BNO055_SYSTEM_ERROR_REG_MAP_ADDR_OUT_OF_RANGE = 0x05,
  BNO055_SYSTEM_ERROR_REG_MAP_WRITE_ERROR = 0x06,
  BNO055_SYSTEM_ERROR_LOW_PWR_MODE_NOT_AVAILABLE_FOR_SELECTED_OPR_MODE = 0x07,
  BNO055_SYSTEM_ERROR_ACCEL_PWR_MODE_NOT_AVAILABLE = 0x08,
  BNO055_SYSTEM_ERROR_FUSION_ALGO_CONF_ERROR = 0x09,
  BNO055_SYSTEM_ERROR_SENSOR_CONF_ERROR = 0x0A
};

/**
 * @brief  Low Level I2C Funktion
 */
void bno055_writeData(uint8_t reg, uint8_t data);
/**
 * @brief  Low Level I2C Funktion
 */
void bno055_readData(uint8_t reg, uint8_t *data, uint8_t len);
/**
 * @brief  Low Level I2C Funktion
 */
void bno055_delay(int time);

/**
 * @brief  Den BNO055-Chip per Software zurücksetzen
 */
void bno055_reset();


void bno055_setOperationMode(uint8_t mode);
void bno055_setOperationModeConfig();
void bno055_setOperationModeNDOF();
void bno055_enableExternalCrystal();
void bno055_disableExternalCrystal();

/**
 * @brief  Den BNO055 initialisieren
 *
 * **Muss** aufgerufen werden, bevor andere Funktionen genutzt werden
 *
 * @return
 *
 *    | 0   | ok     |
 *    | <>0 | Fehler |
 */
uint8_t bno055_setup();

int8_t bno055_getTemp();

uint8_t bno055_getBootloaderRevision();
uint8_t bno055_getSystemStatus();
uint8_t bno055_getSystemError();
int16_t bno055_getSWRevision();

bno055_self_test_result_t bno055_getSelfTestResult();
bno055_calibration_t bno055_getCalibration();

/**
 * @brief  Die Werte des Beschleunigungsmessers holen
 *
 */
bno055_vector_t bno055_getVectorAccelerometer();

/**
 * @brief  Die Werte des Magnetometers holen
 *
 */
bno055_vector_t bno055_getVectorMagnetometer();

/**
 * @brief  Die Werte des Drehratensensors holen
 *
 */
bno055_vector_t bno055_getVectorGyroscope();

/**
 * @brief  Die per Sensor-Fusion zurückgerechneten Raumwinkel holen
 *
 */
bno055_vector_t bno055_getVectorEuler();

bno055_vector_t bno055_getVectorLinearAccel();
bno055_vector_t bno055_getVectorGravity();

#ifdef __cplusplus
  }
#endif
#endif  // BNO055_H_
