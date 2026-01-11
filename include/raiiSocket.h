#pragma once
#include <types.h>
#include <cerrno>
#include <set>
#include <logger.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <type_traits>
#include <types.h>
#include <unistd.h>
// library for a socket read write

namespace ff
{
    // forward declarations
    class clientRaiiSocket;
    class serverRaiiSocket;

    class raiiSocket
    {
        friend serverRaiiSocket;
        friend clientRaiiSocket;
        private:
            const int fileDesc;
            int fileDescOther;
            const u16 port;
            bool closed;

            void sockErrLogThrow(std::string operation, bool close = false);

            void operationSuccess(std::string operation);

            typedef std::remove_reference_t<decltype(errno)> errnoType;
            bool opErrShouldClose(std::string operation, 
                    std::set<errnoType> yesSet,
                    std::set<errnoType> noSet) const;

            bool bindShouldClose() const;

            bool acceptShouldClose() const;

            bool connectShouldClose() const;

        public:
            raiiSocket(const u16 port);

            // no copies
            raiiSocket(raiiSocket& other) = delete;
            raiiSocket& operator=(raiiSocket& other) = delete;

            // move construct no move assign
            raiiSocket(raiiSocket&& other) = default;
            raiiSocket& operator=(raiiSocket&& other) = delete;

            ~raiiSocket();

            virtual std::string read();
            
            void write(std::string msg);

            void close();
    };

    class serverRaiiSocket : public ff::raiiSocket {
        public:
            using ff::raiiSocket::raiiSocket;
            void bind();

            void listen(int maxCons);
            
            void accept();

            virtual std::string read() override;
    };

    class clientRaiiSocket : public ff::raiiSocket {
        public:
            using ff::raiiSocket::raiiSocket;
            void connect();
    };
}
