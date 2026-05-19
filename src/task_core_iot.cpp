#include "task_core_iot.h"

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

constexpr uint32_t TELEMETRY_INTERVAL_MS = 10000U;
uint32_t lastTelemetrySend = 0U;

void coreiot_task(void *pvParameters)
{
    if (xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY) == pdTRUE) {
        while (1)
        {
            if (!tb.connected())
            {
                if (tb.connect(CORE_IOT_SERVER.c_str(), CORE_IOT_TOKEN.c_str(), CORE_IOT_PORT.toInt()))
                {
                    Serial.println("CoreIOT: Connected");
                }
            }

            tb.loop();

            // Gửi telemetry mỗi 10 giây
            if (tb.connected() && (millis() - lastTelemetrySend >= TELEMETRY_INTERVAL_MS))
            {
                tb.sendTelemetryData("temperature", glob_temperature);
                tb.sendTelemetryData("humidity", glob_humidity);
                lastTelemetrySend = millis();
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}