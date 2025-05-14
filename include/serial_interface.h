typedef struct {
    int fd; // File descriptor for serial port
    pthread_t reader_thread;
    bool want_exit;
    uint8_t rx_buffer[512];
    size_t rx_len;
    pthread_mutex_t lock;
} SerialInterface;

// Open serial port (returns 0 on success)
int serial_open(SerialInterface* si, const char* dev_path, int baud_rate);

// Close serial port and cleanup
void serial_close(SerialInterface* si);

// Thread function for reading data
void* serial_reader_thread(void* arg);