//#include <name.h>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/interest.hpp>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <vector>
#include "options.hpp"
#include "ndncatchunks.hpp"
#include "consumer.hpp"
#include <chrono>
#include <thread>
#include <time.h>
#include <vector>
#include "strtk.hpp"

using namespace boost::interprocess;
using namespace ndn;
int main ()
{
 message_queue::remove("message_queue");
// ndn::chunks::ndnChunks nchunks;
    while(true)
    {
   try{
//struct timespec tim,tim2;
//tim.tv_sec = 0;
//tim.tv_nsec = 1;
//nanosleep(&tim, NULL);


 std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
      message_queue mq(open_only ,"message_queue") ;

      size_t const max_msg_size = 0x100;

      std::string s;
      s.resize(max_msg_size);
      size_t msg_size;
      unsigned msg_prio;
      mq.receive(&s[0], s.size(), msg_size, msg_prio);
      s.resize(msg_size);
if(s.size() <=1)
continue;

//ndn::Name newPrefix(s);
//ndn::Interest intrest(newPrefix);
//uint64_t lastSegment =  intrest.getName()[-1].toSegment();
//std::cout << "LLLL " << lastSegment << std::endl;
std::cout << "Name: "<< s << std::endl;
     ndn::chunks::ndnChunks nchunks;
      nchunks.startChunk(s);
  /*std::string newInterestName = intrest.getName()[0].toUri() + "/" + intrest.getName()[1].toUri();// +"/" +data.getName()[2].toUri();
int i=5;
//for(int i=5; i<=lastSegment ; ++i)
do{
Name newPrefix(newInterestName);
Interest newInterest(Name(newPrefix).appendSegment(i));
std::string newName = newInterest.getName().toUri();
//std::cout << newName << std::endl;
nchunks.startChunk(newName);
// std::this_thread::sleep_for(std::chrono::nanoseconds(100));
i = i + 25;
//sleep(1);
}while(i<=lastSegment);
  
*/

   }
   catch(interprocess_exception &ex){
 message_queue::remove("message_queue");
   }

   message_queue::remove("message_queue");
    }
    return 0;
}
