#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>

#include "serial_interface.h"

// Generated Protobuf headers
#include "meshtastic/device_ui.pb.h"

void packet_received(void* arg) {
    SerialInterface* si = (SerialInterface*)arg;
    //printf("Raw: %s\n", si->packet_buffer+HEADER_SIZE);
    meshtastic_FromRadio fromRadio = meshtastic_FromRadio_init_zero;
    // /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(si->packet_buffer+HEADER_SIZE, si->packet_length);
    /* Now we are ready to decode the message. */
    bool status = pb_decode(&stream, meshtastic_FromRadio_fields, &fromRadio);

    if (status)
    {
        printf("Received packet ID: 0x%08x, which_payload_variant: %u\n", fromRadio.id, fromRadio.which_payload_variant);
        switch(fromRadio.which_payload_variant)
        {
            case meshtastic_FromRadio_deviceuiConfig_tag:
                printf("Device UI Config:\n");
                printf("Version: 0x%08X\n", fromRadio.deviceuiConfig.version);
                printf("Screen_timeout:0x%04X\n", fromRadio.deviceuiConfig.screen_timeout);     
            break;

            case meshtastic_FromRadio_config_tag:
                printf("Config Tag: ");
                switch(fromRadio.config.which_payload_variant){
                    case meshtastic_Config_device_tag:
                        printf("Device configuration\n");
                        break;
                    case meshtastic_Config_position_tag:
                        printf("Position configuration\n");
                        break;
                    case meshtastic_Config_power_tag:
                        printf("Power configuration\n");
                        break;
                    case meshtastic_Config_network_tag:
                        printf("Network configuration\n");
                        break;
                    case meshtastic_Config_display_tag:
                        printf("Display configuration\n");
                        break;
                    case meshtastic_Config_lora_tag:
                        printf("LoRa configuration\n");
                        break;
                    case meshtastic_Config_bluetooth_tag:
                        printf("Bluetooth configuration\n");
                        break;
                    case meshtastic_Config_security_tag:
                        printf("Security configuration\n");
                        break;
                    case meshtastic_Config_sessionkey_tag:
                        printf("Session key configuration\n");
                        break;
                    case meshtastic_Config_device_ui_tag:
                        printf("Device UI configuration\n");
                        break;
                    default:
                        printf("Not Supported.\n");
                }
            break;

            case meshtastic_FromRadio_moduleConfig_tag:
                printf("Module Config Tag: ");
                switch(fromRadio.moduleConfig.which_payload_variant) {
                    case meshtastic_ModuleConfig_mqtt_tag:
                        printf("MQTT\n");
                        break;
                    case meshtastic_ModuleConfig_serial_tag:
                        printf("Serial\n");
                        break;
                    case meshtastic_ModuleConfig_external_notification_tag:
                        printf("External Notification\n");
                        break;
                    case meshtastic_ModuleConfig_store_forward_tag:
                        printf("Store & Forward\n");
                        break;
                    case meshtastic_ModuleConfig_range_test_tag:
                        printf("Range Test\n");
                        break;
                    case meshtastic_ModuleConfig_telemetry_tag:
                        printf("Telemetry\n");
                        break;
                    case meshtastic_ModuleConfig_canned_message_tag:
                        printf("Canned Message\n");
                        break;
                    case meshtastic_ModuleConfig_audio_tag:
                        printf("Audio\n");
                        break;
                    case meshtastic_ModuleConfig_remote_hardware_tag:
                        printf("Remote Hardware\n");
                        break;
                    case meshtastic_ModuleConfig_neighbor_info_tag:
                        printf("Neighbor Info\n");
                        break;
                    case meshtastic_ModuleConfig_ambient_lighting_tag:
                        printf("Ambient Lighting\n");
                        break;
                    case meshtastic_ModuleConfig_detection_sensor_tag:
                        printf("Detection Sensor\n");
                        break;
                    case meshtastic_ModuleConfig_paxcounter_tag:
                        printf("Pax Counter\n");
                        break;
                    default:
                        printf("Unknow Type\n");
                }
                break; 
            default:
                printf("Not Supported.\n");
        }
    }else{
        printf("failed packet.\n");
            printf("Payload:\n");
            size_t i;
            for (i = 0; i < si->packet_length+HEADER_SIZE; i++) {
                printf("%02x ", *(si->packet_buffer+i));
                if((i % 16) == 15) {
                    printf("\n");
                }
            }
            printf("\nRaw:\n");
            for (i = 0; i < si->packet_length+HEADER_SIZE; i++) {
                printf("%c", *(si->packet_buffer+i));
            }
    }
}

int main() {
    SerialInterface iface;
    
    if(serial_open(&iface, "/dev/ttyUSB0", B115200, &packet_received) != 0) {
        fprintf(stderr, "Failed to initialize serial interface\n");
        return 1;
    }


    while(1) {
        sleep(1);
    }

    serial_close(&iface);
    return 0;
}
