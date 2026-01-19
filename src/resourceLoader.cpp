#include <filesystem>
#include <string>
#include <fstream>
#include <cassert>
#include <logger.h>
#include <sys/stat.h>
namespace ff::http {

    enum fileStatus{
        NOEXIST = 2,
        NOAUTH = 1,
        GOOD = 0
    };
    
    [[nodiscard("use the resource please")]]
    std::string getResource(const std::string p) {
        const std::string path = "." + p;

        LOG << "reading from resource " << path << "\n";
        std::ifstream stream(path);
        if (!stream.is_open()) {
            struct stat stmp;
            if(stat(path.c_str(), &stmp)) {
                throw std::runtime_error("File does not exist");
            }
            throw std::domain_error("Invalid auth for file");
        }

        LOG << "file open " << stream.is_open() <<
            "goodbit " << stream.goodbit << "\n";

        const uintmax_t size = std::filesystem::file_size(path);
        std::string buf(size, '\0');
        stream.read(&buf[0], buf.size());

        LOG << "read into buffer " << buf << "\n";
        return buf;
    }
}
