#include "../Buffer.h"
#include <iostream>

using namespace web;
using namespace std;

int main()
{
    Buffer buf1, buf2;
    buf1.append("I am a student");
    buf2.append("jdw");
    cout << "before" << endl;
    cout << buf1.readableBytes() << endl;
    cout << buf1.writeableBytes() << endl;
    buf1.retrieve(2);
    cout << buf1.prependableBytes() << endl;
    cout << "after" << endl;
    buf1.append(buf2);
    cout << buf1.retrieveAllToStr() << endl;
    return 0;
}