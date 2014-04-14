#include <zmq.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <unistd.h>


using namespace std;


int main() {

	zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, ZMQ_REP);
    int val = 0; // OK???
    sock.setsockopt(ZMQ_RCVHWM, &val, sizeof (val));
    sock.bind("tcp://127.0.0.1:30001");
    //int timeout = 4000;
    //sock.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));


    int type = 0;
    int num = 0;
    while(true) {
        //cout << " wait for message " << endl;
        zmq::message_t rmsg;
        bool ret = sock.recv(&rmsg);
        if(!ret) {
            cout << " no data " << endl;            
            //break;
        } else {
           //usleep(100);

        }
        char *buf =  (char *) rmsg.data();
        char * p = buf;
        int * seq = (int *) p;
            
        num++;  
        
        //cout << " get a msg " << *seq << " size " << strlen(buf)  << " total " << num << endl;
        

        zmq::message_t *zmsg = new zmq::message_t(80);
        // now assume the work id range can be place in one char
        sock.send(*zmsg);                 

    } 

	return 0;
}