#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>


#include "serial_interface.h"

void packet_received(void* arg) {
    SerialInterface* si = (SerialInterface*)arg;
    printf("Raw: %s\n", si->packet_buffer+HEADER_SIZE);
    meshtastic_MeshPacket packet = meshtastic_MeshPacket_init_zero;
    // /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(si->packet_buffer+HEADER_SIZE, si->packet_length);
    /* Now we are ready to decode the message. */
    bool status = pb_decode(&stream, meshtastic_MeshPacket_fields, &packet);
    if (status)
    {
        printf("Received packet from: 0x%08x\n", packet.from);
        //printf("Payload: %.*s\n", (int)packet.payload->size, (char *)packet.payload->data);
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
