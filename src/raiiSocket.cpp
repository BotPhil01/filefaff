#include <cassert>
#include <ext/stdio_filebuf.h>
#include <raiiSocket.h>
#include <cerrno>
#include <stdexcept>
#include <logger.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <types.h>
#include <unistd.h>

namespace
{
    [[nodiscard]] const int createRaiiSocket()
    {
        const int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            LOG << "raiiSocket failed to create with errno" << errno << "\n";
            throw std::runtime_error("raiiSocket failed to create");
        }

        LOG << "Create raiiSocket success " << fd << "\n";
        return fd;
    }
}

namespace ff
{
    void raiiSocket::sockErrLogThrow(std::string operation, bool shouldClose)
    {
        LOG << "raiiSocket " << fileDesc << " failed to " << operation << " errno " << errno << "\n";
        if (shouldClose)
        {
            ::close(fileDesc);
        }
        throw std::runtime_error("raiiSocket failed to perform operation");
    }

    void raiiSocket::operationSuccess(std::string operation) {
        LOG << "raiiSocket " << operation << " success on " << fileDesc << "\n";
    }
    typedef std::remove_reference_t<decltype(errno)> errnoType;
    bool raiiSocket::opErrShouldClose(std::string operation, 
            std::set<errnoType> yesSet,
            std::set<errnoType> noSet) const 
    {
        if (yesSet.contains(errno)) 
        {
            LOG << operation << "close determination success close=true"
                << fileDesc << " " << errno << "\n";
            return true;
        }

        if (noSet.contains(errno)) 
        {
            LOG << operation << " close determination success close=false "
                << fileDesc << " " << errno << "\n";
            return false;
        }

        LOG << operation << "close determination failed " << fileDesc << " "
            << errno << "\n";
        throw new std::runtime_error("Errno not in available lists");
    }

    bool raiiSocket::bindShouldClose() const 
    {
        const std::initializer_list<errnoType> yesErrnos{
            EACCES, EADDRINUSE, EACCES, EADDRNOTAVAIL,
                EFAULT, ELOOP, ENAMETOOLONG, ENOENT, ENOMEM,
                ENOTDIR, EROFS};
        const std::initializer_list<errnoType> noErrnos{
            EBADF, EINVAL, ENOTSOCK
        };

        return opErrShouldClose("bind", yesErrnos, noErrnos);
    }

    bool raiiSocket::acceptShouldClose() const
    {
        typedef std::remove_reference_t<decltype(errno)> errnoType;
        const std::initializer_list<errnoType> yesClose{
            EAGAIN, EWOULDBLOCK, ECONNABORTED, EFAULT, EINTR, EINVAL,
                EMFILE, ENFILE, ENOBUFS, ENOMEM, EPERM, EPROTO
        };
        const std::initializer_list<errnoType> noClose{
            EBADF, ENOTSOCK, EOPNOTSUPP, ENOSR, ESOCKTNOSUPPORT,
                EPROTONOSUPPORT, ETIMEDOUT, ERESTART
        };
        return opErrShouldClose("accept", yesClose, noClose);
    }

    bool raiiSocket::connectShouldClose() const
    {
        typedef std::remove_reference_t<decltype(errno)> errnoType;
        const std::initializer_list<errnoType> yesClose{
            EACCES, EPERM, EADDRNOTAVAIL, EAFNOSUPPORT, EAGAIN, 
                EALREADY, ECONNREFUSED, EFAULT, EINPROGRESS, EINTR, 
                ENETUNREACH, EPROTOTYPE, ETIMEDOUT
        };
        const std::initializer_list<errnoType> noClose{
            EADDRINUSE, EBADF, EISCONN, ENOTSOCK, 
        };
        return opErrShouldClose("accept", yesClose, noClose);
    }

    raiiSocket::raiiSocket(const u16 port) :
        port(port), fileDesc(createRaiiSocket()), fileDescOther(-1)
    {
        LOG << "raiiSocket constructor called for " << fileDesc << " on port "
            << port << "\n";
    }

    raiiSocket::~raiiSocket()
    {
        LOG << "raiiSocket destructor called for " << fileDesc << " on port " 
            << port << "\n";
        close();
    }

    std::string raiiSocket::read()
    {
        std::string buf(raiiSocket::READBUFSIZE, 0);
        const ssize_t ret = ::read(fileDesc, &buf[0], buf.size());
        if (ret == -1) {
            // TODO fix later
            opErrShouldClose("read", {}, {});
        }
        operationSuccess("read");
        return buf;
    }

    void raiiSocket::write(std::string msg) {
        const ssize_t ret = ::write(fileDesc, &msg[0], msg.size());
        if (ret == -1) {
            opErrShouldClose("write", {}, {});
        }
        operationSuccess("write");
        return;
    }

    void raiiSocket::close()
    {
        if (closed) {
            LOG << "raiiSocket already closed " << fileDesc << "\n";
            return;
        }

        const int ret = ::close(fileDesc);
        if (ret == -1)
        {
            sockErrLogThrow("close");
        }
        closed = true;
        operationSuccess("close");
    }

    void serverRaiiSocket::bind() 
    {
        sockaddr_in address{ 
            .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr = static_cast<struct in_addr>(INADDR_ANY)
        };
        if (::bind(fileDesc, (struct sockaddr *)&address, sizeof(address))) 
        {
            sockErrLogThrow("bind", bindShouldClose());
        }
        operationSuccess("bind");
    }

    void serverRaiiSocket::listen(int maxCons) 
    {
        const int ret = ::listen(fileDesc, maxCons);
        if (ret) 
        {
            sockErrLogThrow("listen", errno == EOPNOTSUPP);
        }
        operationSuccess("listen");
    }
    void serverRaiiSocket::accept() 
    {
        const int ret = ::accept(fileDesc, nullptr, nullptr);
        if (ret == -1) 
        {
            sockErrLogThrow("accept", acceptShouldClose());
        }
        fileDescOther = ret;
        operationSuccess("accept");
    }

    std::string serverRaiiSocket::read()
    {
        if (fileDescOther == -1) {
            throw std::runtime_error("fileDescOther not initialised");
        }
        std::string buf(raiiSocket::READBUFSIZE, 0);
        const ssize_t ret = ::read(fileDescOther, &buf[0], buf.size());
        if (ret == -1) {
            // TODO fix later
            opErrShouldClose("read", {}, {});
        }
        return buf;
    }

    void clientRaiiSocket::connect() 
    {
        const sockaddr_in address{ 
            .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr = static_cast<struct in_addr>(INADDR_ANY)
        };
        const int ret = ::connect(fileDesc, (struct sockaddr*) &address,
                sizeof(address));
        if (ret) {
            sockErrLogThrow("connect", connectShouldClose());
        }
        operationSuccess("connect");
    }
}
