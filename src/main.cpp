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
                {
                    meshtastic_DeviceUIConfig packet = meshtastic_DeviceUIConfig_init_zero;
                    status = pb_decode(&stream, meshtastic_DeviceUIConfig_fields, &packet);
                    if (status){
                        printf("Screen_timeout:0x%04X\n", packet.screen_timeout);
                    }
                }
            break;
            case meshtastic_FromRadio_config_tag:
                printf("config_tag:\n");
            break;
            case meshtastic_FromRadio_moduleConfig_tag:
                printf("moduleConfig_tag \n");
            break; 
            default:
                printf("Not Supported.\n");
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
