#include "tools/tool_sense.h"
#include "hardware/sensor.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

esp_err_t tool_sense_execute(const char *input_json, char *output, size_t output_size) {
    mimi_sensor_data_t data;
    if (mimi_sensor_read(&data) == ESP_OK) {
        if (data.has_imu) {
            snprintf(output, output_size, 
                "{\"status\":\"success\",\"temperature\":%.2f,\"accel\":[%.2f,%.2f,%.2f],\"gyro\":[%.2f,%.2f,%.2f]}", 
                data.temperature, data.accel_x, data.accel_y, data.accel_z, data.gyro_x, data.gyro_y, data.gyro_z);
        } else if (data.has_env) {
            snprintf(output, output_size, 
                "{\"status\":\"success\",\"temperature\":%.2f,\"humidity\":%.2f,\"unit\":\"Celsius\"}", 
                data.temperature, data.humidity);
        } else {
             snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"No sensor data\"}");
        }
    } else {
        snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Sensor read failed\"}");
    }
    return ESP_OK;
}
