#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Ping a host and return the latency
 * @param host Domain or IP
 * @return Latency in ms, or -1 on failure
 */
int network_ping(const char *host);

/**
 * @brief Perform a WiFi scan and fill a buffer with results
 * @param buf Output buffer
 * @param buf_size Size of buffer
 * @return Number of APs found
 */
int network_wifi_scan(char *buf, size_t buf_size);

/**
 * @brief Get detailed IP information
 * @param buf Output buffer
 * @param buf_size Size of buffer
 * @return esp_err_t
 */
esp_err_t network_get_ip_info(char *buf, size_t buf_size);

/**
 * @brief Force NTP time synchronization
 * @return esp_err_t
 */
esp_err_t network_ntp_sync(void);

#endif // NETWORK_UTILS_H
