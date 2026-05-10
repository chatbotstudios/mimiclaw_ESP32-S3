#include "hardware/network_utils.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"
#include "proxy/http_proxy.h"
#include <string.h>

static const char *TAG = "net_utils";

/* --- Ping Logic --- */
static int s_last_ping_ms = -1;

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args) {
  uint32_t elapsed_time;
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time,
                       sizeof(elapsed_time));
  s_last_ping_ms = (int)elapsed_time;
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args) {
  s_last_ping_ms = -1;
}

int network_ping(const char *host) {
  s_last_ping_ms = -1;

  ip_addr_t target_addr;
  struct hostent *hp = gethostbyname(host);
  if (!hp)
    return -1;

  // Direct assignment to avoid -Werror=address from ip_addr_set_ip4_u32 macro
  target_addr.u_addr.ip4.addr = *(uint32_t *)(hp->h_addr);
  target_addr.type = IPADDR_TYPE_V4;

  esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
  ping_config.target_addr = target_addr;
  ping_config.count = 1;

  esp_ping_callbacks_t cbs = {.on_ping_success = cmd_ping_on_ping_success,
                              .on_ping_timeout = cmd_ping_on_ping_timeout,
                              .cb_args = NULL};

  esp_ping_handle_t ping_hdl;
  esp_ping_new_session(&ping_config, &cbs, &ping_hdl);
  esp_ping_start(ping_hdl);

  /* Block for a moment to get result */
  int timeout = 2000;
  while (s_last_ping_ms == -1 && timeout > 0) {
    vTaskDelay(pdMS_TO_TICKS(50));
    timeout -= 50;
  }

  esp_ping_stop(ping_hdl);
  esp_ping_delete_session(ping_hdl);

  return s_last_ping_ms;
}

/* --- WiFi Scan --- */
int network_wifi_scan(char *buf, size_t buf_size) {
  uint16_t number = 10;
  wifi_ap_record_t ap_info[10];
  uint16_t ap_count = 0;

  esp_wifi_scan_start(NULL, true);
  esp_wifi_scan_get_ap_records(&number, ap_info);
  esp_wifi_scan_get_ap_num(&ap_count);

  size_t off = 0;
  off += snprintf(buf + off, buf_size - off, "Found %d APs:\n", ap_count);
  for (int i = 0; i < number && i < ap_count; i++) {
    off +=
        snprintf(buf + off, buf_size - off, "- %-16s | RSSI: %d dBm | Ch: %d\n",
                 ap_info[i].ssid, ap_info[i].rssi, ap_info[i].primary);
  }
  return ap_count;
}

/* --- IP Info --- */
esp_err_t network_get_ip_info(char *buf, size_t buf_size) {
  esp_netif_ip_info_t ip_info;
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (!netif)
    return ESP_ERR_NOT_FOUND;

  esp_netif_get_ip_info(netif, &ip_info);

  char local_ip[16], gw[16], mask[16];
  esp_ip4addr_ntoa(&ip_info.ip, local_ip, 16);
  esp_ip4addr_ntoa(&ip_info.gw, gw, 16);
  esp_ip4addr_ntoa(&ip_info.netmask, mask, 16);

  snprintf(buf, buf_size,
           "Local IP: %s\nGateway:  %s\nMask:     %s\nPublic IP: (fetching...)",
           local_ip, gw, mask);

  return ESP_OK;
}

/* --- NTP Sync --- */
esp_err_t network_ntp_sync(void) {
  ESP_LOGI(TAG, "Starting NTP sync...");
  esp_sntp_stop();
  esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org");
  esp_sntp_init();

  /* Wait for time to be set */
  int retry = 0;
  const int max_retry = 10;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET &&
         ++retry < max_retry) {
    ESP_LOGI(TAG, "Waiting for NTP sync... (%d/%d)", retry, max_retry);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }

  if (retry < max_retry) {
    ESP_LOGI(TAG, "NTP Sync successful");
    return ESP_OK;
  } else {
    ESP_LOGE(TAG, "NTP Sync failed");
    return ESP_FAIL;
  }
}
