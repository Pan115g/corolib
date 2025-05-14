#ifndef SERIALPORT_H_    
#define SERIALPORT_H_

#include <cstdint>

namespace corolib
{
    class IoEventHandler;

    class SerialPort
    {
        public:
            constexpr static int32_t Invalid_FileDescriptor = -1;

            explicit SerialPort(IoEventHandler& ioEventHandler, const char* deviceName, const uint32_t baudrate);
            SerialPort(const SerialPort&) = delete;

            ~SerialPort();

            int getFileDescriptor() const { return mFileDescriptor; }
            bool isInitialized() const { return mInitialized; }

        private:
            void setNonBlockingMode();
            void setTerminalAttributes();
            void registerWithEpoll();
            void unregisterWithEpoll();

            bool mInitialized{false};
            uint32_t mBaudrate{0};
            int mFileDescriptor{Invalid_FileDescriptor};
            IoEventHandler& mIoEventHandler;
    };
}
#endif