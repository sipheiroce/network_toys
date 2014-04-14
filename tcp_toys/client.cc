#include <zmq.hpp>
#include <iostream>
#include <unistd.h>


using namespace std;

int main() {

	zmq::context_t ctx(1);
    zmq::socket_t publisher(ctx, ZMQ_PUB);
    publisher.bind("tcp://127.0.0.1:31001");
    int val = 0; // OK???
    publisher.setsockopt(ZMQ_SNDHWM, &val, sizeof (val));

    zmq::socket_t control(ctx, ZMQ_REQ);
    control.connect("tcp://127.0.0.1:30001");

    usleep(1000000);

    int type = 0;
    int expected = 10000;
    int retry_idx = 0;
    for(int i = 0; i<10000; ++i) {
    	type = 0;
        zmq::message_t *zmsg = new zmq::message_t(120);
        // now assume the work id range can be place in one char
        char *buf =  (char *) zmsg->data();
        memcpy(buf, (char *) &i, sizeof(int));
        memcpy(buf + sizeof(int), (char *) &type, sizeof(int));
        publisher.send(*zmsg);
        cout << "send msg " << (i+1) << endl;
        //usleep(100);

        if( (i+1) % expected == 0 && i != 0) {
            retry_idx = i+1 - expected;
        	type = 1;
        	zmq::message_t *zmsg1 = new zmq::message_t(120);
        	// now assume the work id range can be place in one char
	        char *buf1 =  (char *) zmsg1->data();
	        memcpy(buf1, (char *) &i, sizeof(int));
	        memcpy(buf1 + sizeof(int), (char *) &type, sizeof(int));
	        control.send(*zmsg1);
            cout << "send query " << endl;

            zmq::message_t rmsg;
            cout << "wait for reply  " << endl;
            control.recv(&rmsg);

	        
	        // need to get things out of message
	        char *buf =  (char *) rmsg.data();
            char * p = buf + sizeof(int);
	        int * type_ptr = (int *) p;
            p = buf + 2*sizeof(int);
            int * new_expected = (int *) p;
	        if(*type_ptr == 2) {
	        	//ok
	        	cout << " OK,  continue " << endl;
                retry_idx = i + 1;
	        } else {
                cout << " get response " << *type_ptr << endl;
                cout << " retransmit @ " << retry_idx << " with expected " << *new_expected << endl;
                expected = *new_expected;
                i = retry_idx-1;
	        }

	        
        }

    } 

	return 0;
}