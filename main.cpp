#include <iostream>
#include "wav.h"

int main() {
    wav music1(1,8000);
    music1.decode("../examples/Lem2400.enc");
//    music1.upload("../output.wav");
    music1.save("../output2.wav");
    return 0;
}
