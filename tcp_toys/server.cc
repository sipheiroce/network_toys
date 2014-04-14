#include <zmq.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <unistd.h>



using namespace std;

void listener(zmq::context_t *ctx, int *count, int *expected) {
    zmq::socket_t control(*ctx, ZMQ_REP);
    control.bind("tcp://127.0.0.1:30001");
    //int timeout = 4000;
    //control.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    while(true) {
        zmq::message_t msg;
        cout << " wait for query" << endl;
        control.recv(&msg);
        cout << " get a query -- count " << *count << endl;
        int old_count = *count;
        zmq::message_t *zmsg = new zmq::message_t(120);
        // now assume the work id range can be place in one char
        int type;
        if(*count == *expected) {
            type = 2;
            *count = 0;
        } else {
            // wait a few time
            int times = 0;
            while(times < 50 && *count != *expected) {
                if(*count == old_count) {
                    ++times; 
                } else {
                    cout << "@@@ old " << old_count << " cur " << *count << endl;
                    old_count = *count;
                }   
                //cout << "@@@ sleep 10ms  -- old " << old_count << " cur " << *count << endl; 
                usleep(5000);
            }
            if(*count == *expected) {
                type = 2;
                *count = 0;
            } else {
                cout <<  " count " << *count  << " doesn't match expected " << *expected << endl;
                type = 3;
                //(*expected) = (*expected) - 1000;
                (*expected) = ((*count)/1000) * 1000;
                if((*expected) < 1000) {
                    (*expected) = 1000;
                }
                *count = 0;
            }
        }
        int i = 0;
        char *buf =  (char *) zmsg->data();
        memcpy(buf, (char *) &i, sizeof(int));
        memcpy(buf + sizeof(int), (char *) &type, sizeof(int));
        memcpy(buf + 2*sizeof(int), (char *) expected, sizeof(int));
        control.send(*zmsg);
        cout << " send response " << type << endl;
        usleep(1000);
    }
}



int main() {

	zmq::context_t ctx(1);
    zmq::socket_t subscriber(ctx, ZMQ_SUB);
    int val = 0; // OK???
    subscriber.setsockopt(ZMQ_RCVHWM, &val, sizeof (val));
    subscriber.connect("tcp://127.0.0.1:31001");
    int timeout = 4000;
    //subscriber.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    int num = 0;
    int real_total = 0;
    int expected = 10000;
    boost::thread lthread(listener, &ctx, &num, &expected);    

    int type = 0;
    while(true) {
        //cout << " wait for message " << endl;
        zmq::message_t rmsg;
        bool ret = subscriber.recv(&rmsg);
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
        real_total++;     
        if(num % expected == 0) {
            cout << " get a msg " << *seq << " total " << num <<  " real total " << real_total << endl;
        }
                 

    } 

    //lthread.join();

	return 0;
}