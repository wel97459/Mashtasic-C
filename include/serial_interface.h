#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>


#include <pb_encode.h>
#include <pb_decode.h>
#include "serial_interface.h"

// Generated Protobuf headers
#include "meshtastic/mesh.pb.h"
#include "meshtastic/admin.pb.h"

#define START1 0x94
#define START2 0xC3
#define HEADER_SIZE 4
#define MAX_PACKET_SIZE 512
#define MAX_LOG_SIZE 256
#define READ_BUFFER_SIZE 1024

typedef struct {
    int fd; // File descriptor for serial port
    pthread_t reader_thread;
    bool want_exit;
    uint8_t* packet_buffer;
    size_t packet_index;
    size_t packet_length;
    uint8_t* log_buffer;
    size_t log_index;
    volatile int running;
    pthread_mutex_t lock;
    void (*packet_handler)(void *);
} SerialInterface;

// Open serial port (returns 0 on success)
int serial_open(SerialInterface* si, const char* dev_path, int baud_rate, void (*cb)(void *));

void serial_write(SerialInterface* si, uint8_t *buffer, size_t len);

int send_to_radio(SerialInterface* si, meshtastic_ToRadio *message);
// Close serial port and cleanup
void serial_close(SerialInterface* si);