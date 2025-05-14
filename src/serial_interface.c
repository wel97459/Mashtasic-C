#include <termios.h>
#include <pthread.h>

int serial_open(SerialInterface* si, const char* dev_path, int baud_rate) {
    si->fd = open(dev_path, O_RDWR | O_NOCTTY);
    if (si->fd < 0) return -1;

    struct termios tty;
    tcgetattr(si->fd, &tty);
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);
    tty.c_cflag &= ~HUPCL; // Disable hangup-on-close
    tcsetattr(si->fd, TCSANOW, &tty);

    pthread_create(&si->reader_thread, NULL, serial_reader_thread, si);
    return 0;
}

void* serial_reader_thread(void* arg) {
    SerialInterface* si = (SerialInterface*)arg;
    uint8_t buf[128];
    while (!si->want_exit) {
        ssize_t n = read(si->fd, buf, sizeof(buf));
        if (n > 0) {
            pthread_mutex_lock(&si->lock);
            memcpy(si->rx_buffer + si->rx_len, buf, n);
            si->rx_len += n;
            pthread_mutex_unlock(&si->lock);
        }
    }
    return NULL;
}