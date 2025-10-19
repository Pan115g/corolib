#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <errno.h>

int configureSerialPort(int fd) {
    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    // Set baud rate
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // 8N1 Mode
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 data bits

    // No flow control
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cflag |= CREAD | CLOCAL;  // Turn on READ & ignore ctrl lines
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Turn off software flow ctrl
    tty.c_oflag &= ~OPOST;                          // Raw output

    // Set read timeout
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return 0;
}

int test_pseudo_serial() {
    const char* portname = "/tmp/ttyV0";  // Replace with your actual port

    int serial_fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd < 0) {
        perror("open");
        return 1;
    }

    // Configure the serial port
    if (configureSerialPort(serial_fd) != 0) {
        close(serial_fd);
        return 1;
    }

    std::cout << "Waiting for data on " << portname << "...\n";

    char buf[1025];
    while (true) {
        int num_bytes = read(serial_fd, buf, sizeof(buf) - 1);
        if (num_bytes > 0) {
            buf[num_bytes] = '\0';  // Null-terminate the buffer
            std::cout << "Received: " << buf << std::flush;
        } else if (num_bytes < 0 && errno != EAGAIN) {
            perror("read");
            break;
        }
        // Otherwise, it's a non-blocking read with no data.
    }

    close(serial_fd);
    return 0;
}