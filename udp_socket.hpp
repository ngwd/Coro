#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string>

void die(const char* s)
{
  perror(s);
  exit(1);
}      
class udp_socket {
  public:
    explicit udp_socket(std::string ip_addr, unsigned short port) {
      int fd, i; 
      char buf[512];
      if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
         die("socket");
      }
      struct sockaddr_in me = {0};
      me.sin_family = AF_INET;
      me.sin_port = htons(port);
      // me.sin_port = port;  // wrong
      // me.sin_addr.s_addr = htonl(INADDR_ANY);
			me.sin_addr.s_addr = INADDR_ANY;
			if (bind(fd, (struct sockaddr *)&me, sizeof(struct sockaddr_in)) == -1) {
        die("bind");
      }
      _fd = fd;
		}; 
   const int fd() const noexcept {return _fd;} 
 private:
   int _fd; 
};
