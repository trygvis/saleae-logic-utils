#include "common.h"

using namespace std;
using namespace Logic;

Log log("to_event");
ByteInStream* in;
EventOutStream* out;

bool logic_parse_args(int argc, char* argv[]) {
    return true;
}

void logic_work() {
    in = new ByteInStream(cin);
    out = new EventOutStream(cout);

    log.info("Sample rate: %s", in->sampleRate()->text);

    uint8_t value = in->read();
    uint8_t last = value;
    while(in->isOk()) {
        if(value == last) {
            value = in->read();
            continue;
        }

        char buf[1000];
        printf("change: time=%s, last=%02x, value=%02x\n", in->currentTime().to_str(buf, sizeof(buf)), last, value);

        last = value;
        value = in->read();
    }
}

void logic_close() {
    delete in;
    delete out;
}
