#include <vector>
#include <map>
#include <functional>
#include <span>
#include <iostream>

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udp_socket.hpp"

// compile command is g++-10 poller.cc -o poller_sum -std=c++2a
// test method
// strace ./poller_sum
// or ./poller_sum 
// in other console window
//  echo "aa" > /dev/udp/127.0.0.1/4000
//  echo "aa" > /dev/udp/127.0.0.1/4001

class poller {
  public :
    using worker = std::function<void (std::span<char> data)>;
    void add(int fd, worker w) {
       fds_.push_back({fd, POLLIN, 0});
       cbs_.emplace(fd, std::move(w));
    } 
    void wait() {
      auto r = poll (fds_.data(), fds_.size(), -1);
      for( auto& e: fds_) {
        if (e.revents & POLLIN) {
          char buffer[1500];
          auto len = ::read(e.fd, buffer, sizeof(buffer));
          cbs_[e.fd](std::span(buffer).first(len));
        }
      }
    }
    
  private:
    std::vector<pollfd> fds_;
    std::map<int, worker> cbs_;
};

int main()
{
  auto port4000 = udp_socket("127.0.0.1", 4000);
  auto port4001 = udp_socket("127.0.0.1", 4001);
  uint64_t packets = 0, sum = 0;
  bool done = false;
  
  auto calc_sum = [&](std::span<char> data) {
    ++packets;
    for( auto c: data) {sum += c;}
  };   
  auto print_and_exit = [&](std::span<char>) {
    std::cout << "packets = " << packets << " sum = " << sum <<"\n";
    done = true;
  };
  poller poll;
  poll.add (port4000.fd(), calc_sum);
  poll.add (port4001.fd(), print_and_exit);
  while (!done) {
    poll.wait();
  }  
	return 1;
}
