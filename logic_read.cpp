#include "LogicInterface.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <map>
#include <pthread.h>
#include <unistd.h>

using namespace std;

Log log("logic_read", true);
ByteOutStream* out;

LogicInterface* logicInterface;
int sampleRateHz = 0;

void OnConnect(unsigned int logic_id, void* user_data);
void OnDisconnect(unsigned int logic_id, void* user_data);
void OnReadData( unsigned int logic_id, unsigned char* data, unsigned int data_length, void* user_data );
//void OnWriteData( unsigned int logic_id, unsigned char* data, unsigned int data_length, void* user_data );
void OnError(unsigned int logic_id, void* user_data);

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

unsigned int logicId;

int usage(char* me, char* msg) {
    log.error("usage: %s -r <sample rate>", me);
    log.error("");
    log.error(" Sample rate has to be one of");
    for(int i = 0; i < SampleRate::count(); i++) {
        log.error("  % 9d (%s)", SampleRate::get(i).value, SampleRate::get(i).text.c_str());
    }

    if(msg) {
        log.error("");
        log.error("%s", msg);
    }
    return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
    char c;
    SampleRate* sampleRate = NULL;
    while ((c = getopt (argc, argv, "r:")) != -1) {
        switch(c) {
            case 'r': // Sample rate
                sampleRate = SampleRate::parse(optarg);
                break;
            case '?':
                log.error("woot? got '?'");
                return usage(argv[0], NULL);
        }
    }

    if(sampleRate == NULL) {
        return usage(argv[0], "Invalid sample rate, has to be one of the allowed values.");
    }

    log.info("Sample rate: %d", sampleRate->text.c_str());

    out = new ByteOutStream(sampleRate, cerr);

    log.info("Connecting to Logic...");
	//Register for our callbacks.
    logicInterface = new LogicInterface();
	logicInterface->RegisterOnConnect( OnConnect, 0 );
	logicInterface->RegisterOnDisconnect( OnDisconnect, 0 );
	logicInterface->RegisterOnReadData( OnReadData, 0 );
	logicInterface->RegisterOnError( OnError, 0 );

	logicInterface->BeginConnect();

    pthread_mutex_lock(&condition_mutex);
    pthread_cond_wait(&condition_cond, &condition_mutex);
    pthread_mutex_unlock(&condition_mutex);

    log.info("Connected! Logic id=", logicId);

    logicInterface->ReadStart( logicId );

    sleep(10);

    logicInterface->Stop( logicId );

    // TODO: Clean up on failure
    delete logicInterface;
    delete out;
	return 0;
}

void OnConnect(unsigned int logic_id, void* user_data)
{
	//note that you could use user_data to pass the 'this' pointer to a static member of a class;  user_data is set when registering the callbacks.
	log.info("Logic connected! (id=%d)", logic_id);
	logicInterface->SetSampleRateHz( logic_id, sampleRateHz );

    logicId = logic_id;

    pthread_mutex_lock(&condition_mutex);
    pthread_cond_signal(&condition_cond);
    pthread_mutex_unlock(&condition_mutex);
}

void OnDisconnect(unsigned int logic_id, void* user_data)
{
	log.info("Logic disonnected! (id=%d)", logic_id);
}

void OnReadData( unsigned int logic_id, unsigned char* data, unsigned int data_length, void* user_data )
{
	log.info("Got %d  bytes, starting with 0x%02x", data_length, (int)*data);

    /*
    int file = STDERR_FILENO;

	log.info("Got %d  bytes, starting with 0x%02x", data_length, (int)*data);

    ssize_t size;
    unsigned int left = data_length;
    while(left > 0) {
        size = write(file, data, left);

        if(size == -1) {
            log.error("Error while writing to output stream.");
        }
        fsync(file);
        left -= size;
    }

    */
	delete[] data;
}

void OnError(unsigned int logic_id, void* user_data)
{
	//Logic reports an error if something fails while reading or writing.  Usually it means that it couldn't keep up with the requested data rate, or Logic was disconnected.
	log.error("Logic Reported an Error!");
}

