#include <zmq.hpp>
#include <iostream>
#include <unistd.h>


using namespace std;

int main() {

	zmq::context_t ctx(1);
    zmq::socket_t sock(ctx, ZMQ_REQ);
    int val = 0; // OK???
    sock.setsockopt(ZMQ_SNDHWM, &val, sizeof (val));
    sock.connect("tcp://127.0.0.1:30001");
    usleep(1000000);

    int retry_idx = 0;
    for(int i = 0; i<1000; ++i) {
        zmq::message_t *zmsg = new zmq::message_t(200000);
        // now assume the work id range can be place in one char
        char *buf =  (char *) zmsg->data();
        memcpy(buf, (char *) &i, sizeof(int));
        sock.send(*zmsg);
        //cout << "send msg " << (i+1) << endl;
        //usleep(100);
        zmq::message_t rmsg;
        bool ret = sock.recv(&rmsg);  

    } 

	return 0;
}