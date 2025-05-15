#include "serial_interface.h"

void handle_log(char* message) {
    //printf("LOG: %s\n", message);
}

void* serial_reader_thread(void* arg) {
    SerialInterface* si = (SerialInterface*)arg;
    char byte;
    size_t bytes_read;
    size_t packet_length = 0;
    size_t bytes_needed = 0;

    enum State { SEARCH_START1, SEARCH_START2, READ_HEADER, READ_PAYLOAD };
    enum State state = SEARCH_START1;

    while (si->running) {

        bytes_read = read(si->fd, &byte, 1);
        if (bytes_read <= 0) continue;
        
        switch (state) {
            case SEARCH_START1:
                if (byte == START1) {
                    printf("Start1\n");
                    state = SEARCH_START2;
                    si->packet_index = 0;
                    si->packet_buffer[si->packet_index++] = byte;
                } else {
                    // Handle log messages
                    if (byte == '\r') continue;
                    if (byte == '\n') {
                        si->log_buffer[si->log_index] = '\0';
                        handle_log((char*)si->log_buffer);
                        si->log_index = 0;
                    } else {
                        if (si->log_index < MAX_LOG_SIZE-1) {
                            si->log_buffer[si->log_index++] = byte;
                        }
                    }
                }
                break;

            case SEARCH_START2:
                si->packet_buffer[si->packet_index++] = byte;
                if (byte == START2) {
                    state = READ_HEADER;
                    printf("Start2\n");
                } else {
                    state = SEARCH_START1;
                }
                break;

            case READ_HEADER:
                si->packet_buffer[si->packet_index++] = byte;
                if (si->packet_index == HEADER_SIZE) {
                    si->packet_length = (si->packet_buffer[2] << 8) | si->packet_buffer[3];
                    if (si->packet_length > MAX_PACKET_SIZE) {
                        state = SEARCH_START1;
                        break;
                    }
                    bytes_needed = si->packet_length;
                    state = READ_PAYLOAD;
                }
                break;

            case READ_PAYLOAD:
                si->packet_buffer[si->packet_index++] = byte;
                if (--bytes_needed == 0) {
                    // Full packet received
                    si->packet_handler(si);
                    state = SEARCH_START1;
                }
                break;
        }
    }
    return NULL;
}

int serial_open(SerialInterface* si, const char* dev_path, int baud_rate, void (*ph)(void *)) {
    si->fd = open(dev_path, O_RDWR | O_NOCTTY);
    if (si->fd < 0) return -1;

    struct termios tty;
    tcgetattr(si->fd, &tty);
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);
    tty.c_cflag &= ~HUPCL; // Disable hangup-on-close
    tcsetattr(si->fd, TCSANOW, &tty);
    si->running = 1;
    
    si->packet_index = 0;
    si->packet_length = 0;
    si->packet_buffer = (uint8_t *)malloc(sizeof(uint8_t)*MAX_PACKET_SIZE);
    si->log_index = 0;
    si->log_buffer = (uint8_t *)malloc(sizeof(uint8_t)*MAX_LOG_SIZE);

    si->packet_handler = ph;
    pthread_create(&si->reader_thread, NULL, serial_reader_thread, si);

    uint8_t buff[32];
    for(size_t i = 0; i < 32; i++)
        buff[i] = START2;

    serial_write(si, buff, 32);

    return 0;
}

void serial_write(SerialInterface* si, uint8_t *buffer, size_t len)
{
    write(si->fd, buffer, len);
    fsync(si->fd);
    sleep(0.1);
}

void serial_close(SerialInterface* si) {
    si->running = 0;
    pthread_join(si->reader_thread, NULL);
    free(si->packet_buffer);
    free(si->log_buffer);
    close(si->fd);
}

int send_to_radio(SerialInterface* si, meshtastic_ToRadio *message) {
    // Serialize the protobuf message
    uint8_t * buffer = (uint8_t*) malloc(meshtastic_ToRadio_size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, meshtastic_ToRadio_size);
    bool status = pb_encode(&stream, meshtastic_ToRadio_fields, &message);
    size_t msg_len = stream.bytes_written;
    /* Then just check for any errors.. */
    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        free(buffer);
        return -2;
    }
    // Create protocol header
    uint8_t header[HEADER_SIZE] = {
        START1,
        START2,
        (uint8_t)((msg_len >> 8) & 0xFF), // High byte
        (uint8_t)(msg_len & 0xFF)         // Low byte
    };


    printf("Sending header: ");
    for (int i = 0; i < HEADER_SIZE; i++) {
        printf("%02x ", header[i]);
    }
    printf("\nPayload: ");
    for (size_t i = 0; i < msg_len; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");


    // Write header and payload
    ssize_t bytes_written = write(si->fd, header, HEADER_SIZE);
    if (bytes_written != HEADER_SIZE) {
        printf("Header write failed\n");
        free(buffer);
        return -3;
    }

    bytes_written = write(si->fd, buffer, msg_len);
    if (bytes_written != (ssize_t)msg_len) {
        printf("Payload write failed\n");
        free(buffer);
        return -4;
    }

    // // Ensure data is fully transmitted
    // if (tcdrain(si->fd != 0) {
    //     printf("Flush failed\n");
    //     free(buffer);
    //     return -5;
    // }

    free(buffer);
    return 0;
}