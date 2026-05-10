#include "hardware/bluetooth_utils.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* NimBLE includes */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "cJSON.h"

static const char *TAG = "bt_utils";
static bool s_initialized = false;
static SemaphoreHandle_t s_scan_sem = NULL;
static SemaphoreHandle_t s_sync_sem = NULL;

typedef struct {
    char name[32];
    char addr[18];
    int rssi;
} ble_device_info_t;

#define MAX_SCAN_RESULTS 20
static ble_device_info_t s_scan_results[MAX_SCAN_RESULTS];
static int s_num_results = 0;

esp_err_t bluetooth_advertise_start(void) {
    if (!s_initialized) return ESP_ERR_INVALID_STATE;

    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *device_name = "MimiClaw-S3";

    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    int rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGE(TAG, "Error starting advertising; rc=%d", rc);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Started BLE Advertising as %s", device_name);
    return ESP_OK;
}

esp_err_t bluetooth_advertise_stop(void) {
    if (!s_initialized) return ESP_ERR_INVALID_STATE;
    
    int rc = ble_gap_adv_stop();
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGE(TAG, "Error stopping advertising; rc=%d", rc);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Stopped BLE Advertising");
    return ESP_OK;
}

static void ble_app_on_sync(void) {
    ESP_LOGI(TAG, "NimBLE Host Synced");
    
    // Auto-start advertising so devices can find Mimi
    bluetooth_advertise_start();

    if (s_sync_sem) {
        xSemaphoreGive(s_sync_sem);
    }
}

static void host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

esp_err_t bluetooth_init(void) {
    if (s_initialized) return ESP_OK;

    if (s_sync_sem == NULL) {
        s_sync_sem = xSemaphoreCreateBinary();
    } else {
        xSemaphoreTake(s_sync_sem, 0); /* Ensure it's empty */
    }

    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d", ret);
        return ret;
    }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = ble_app_on_sync;

    ble_svc_gap_device_name_set("MimiClaw-S3");
    ble_svc_gap_init();

    nimble_port_freertos_init(host_task);

    if (s_scan_sem == NULL) {
        s_scan_sem = xSemaphoreCreateBinary();
    }

    /* Wait for host to sync (max 5 seconds) */
    ESP_LOGI(TAG, "Waiting for NimBLE sync...");
    if (xSemaphoreTake(s_sync_sem, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGE(TAG, "NimBLE sync timeout!");
        return ESP_ERR_TIMEOUT;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Bluetooth NimBLE stack initialized and synced");
    return ESP_OK;
}

esp_err_t bluetooth_deinit(void) {
    if (!s_initialized) return ESP_OK;

    int rc = nimble_port_stop();
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to stop nimble port %d", rc);
        return ESP_FAIL;
    }

    nimble_port_deinit();
    s_initialized = false;
    ESP_LOGI(TAG, "Bluetooth NimBLE stack deinitialized");
    return ESP_OK;
}

bool bluetooth_is_enabled(void) {
    return s_initialized;
}

static int ble_scan_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
    case BLE_GAP_EVENT_DISC: {
        struct ble_hs_adv_fields fields;
        int rc = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        if (rc != 0) return 0;

        if (s_num_results < MAX_SCAN_RESULTS) {
            /* Check if we already have this address */
            char addr_str[18];
            snprintf(addr_str, sizeof(addr_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                     event->disc.addr.val[5], event->disc.addr.val[4], event->disc.addr.val[3],
                     event->disc.addr.val[2], event->disc.addr.val[1], event->disc.addr.val[0]);

            bool found = false;
            for (int i = 0; i < s_num_results; i++) {
                if (strcmp(s_scan_results[i].addr, addr_str) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                strncpy(s_scan_results[s_num_results].addr, addr_str, 18);
                if (fields.name_len > 0) {
                    int len = fields.name_len > 31 ? 31 : fields.name_len;
                    memcpy(s_scan_results[s_num_results].name, fields.name, len);
                    s_scan_results[s_num_results].name[len] = '\0';
                } else {
                    strcpy(s_scan_results[s_num_results].name, "Unknown");
                }
                s_scan_results[s_num_results].rssi = event->disc.rssi;
                s_num_results++;
            }
        }
        return 0;
    }
    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "Scan complete");
        xSemaphoreGive(s_scan_sem);
        return 0;
    default:
        return 0;
    }
}

esp_err_t bluetooth_ble_scan(int duration_ms, char *results_json, size_t max_len) {
    if (!s_initialized) {
        esp_err_t err = bluetooth_init();
        if (err != ESP_OK) return err;
    }

    s_num_results = 0;
    memset(s_scan_results, 0, sizeof(s_scan_results));

    struct ble_gap_disc_params disc_params;
    disc_params.filter_duplicates = 1;
    disc_params.passive = 0;
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    int rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, duration_ms, &disc_params, ble_scan_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error initiating GAP discovery; rc=%d", rc);
        return ESP_FAIL;
    }

    /* Wait for scan to complete */
    if (xSemaphoreTake(s_scan_sem, pdMS_TO_TICKS(duration_ms + 500)) != pdTRUE) {
        ESP_LOGW(TAG, "Scan timeout");
    }

    /* Format results to JSON */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON *arr = cJSON_CreateArray();
    for (int i = 0; i < s_num_results; i++) {
        cJSON *dev = cJSON_CreateObject();
        cJSON_AddStringToObject(dev, "name", s_scan_results[i].name);
        cJSON_AddStringToObject(dev, "address", s_scan_results[i].addr);
        cJSON_AddNumberToObject(dev, "rssi", s_scan_results[i].rssi);
        cJSON_AddItemToArray(arr, dev);
    }
    cJSON_AddItemToObject(root, "devices", arr);
    cJSON_AddNumberToObject(root, "count", s_num_results);

    char *json_str = cJSON_PrintUnformatted(root);
    strncpy(results_json, json_str, max_len);
    free(json_str);
    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t bluetooth_get_info(char *info_json, size_t max_len) {
    uint8_t addr[6];
    int rc = -1;

    if (s_initialized) {
        rc = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, addr, NULL);
        if (rc != 0) {
            rc = ble_hs_id_copy_addr(BLE_ADDR_RANDOM, addr, NULL);
        }
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "initialized", s_initialized);
    cJSON_AddStringToObject(root, "stack", "NimBLE");
    
    if (rc == 0) {
        char addr_str[18];
        snprintf(addr_str, sizeof(addr_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                 addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
        cJSON_AddStringToObject(root, "address", addr_str);
    }
    
    char *json_str = cJSON_PrintUnformatted(root);
    strncpy(info_json, json_str, max_len);
    free(json_str);
    cJSON_Delete(root);
    return ESP_OK;
}
