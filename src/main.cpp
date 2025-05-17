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

void testing(){
    meshtastic_ToRadio msg = meshtastic_ToRadio_init_default;
    msg.
    uint8_t * buffer = (uint8_t*) malloc(1024);
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, 1024);
    bool status = pb_encode(&stream, meshtastic_ToRadio_fields, &msg);
    size_t msg_len = stream.bytes_written;

    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));


    printf("msg_len: %lu, Payload: \n", msg_len);
    for (size_t i = 0; i < msg_len; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
}

int main() {
    SerialInterface iface;
    
    // if(serial_open(&iface, "/dev/ttyUSB0", B115200, &packet_received) != 0) {
    //     fprintf(stderr, "Failed to initialize serial interface\n");
    //     return 1;
    // }


    testing();

    // while(1) {
    //     sleep(1);
    // }

    // serial_close(&iface);
    return 0;
}
