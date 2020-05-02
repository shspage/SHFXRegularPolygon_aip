/*
 console app for testing
 */

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <functional>
#include "../Source/myImGuiDialog.h"
#include "../Source/MyParms.h"

class MyClass
{
public:
    MyClass() : parms({3, 100.0f, 0}){};
    void callbackFunc();
    MyParms* getParms(){ return &parms; }
private:
    MyParms parms;
};

void MyClass::callbackFunc(){
    std::cout << "vertices=" << parms.vertices << "/scale=" << parms.scale \
        << "/angle=" << parms.angle << std::endl;
}

int main(int argc, const char * argv[]) {
    MyClass* cl = new MyClass();
    std::function<void(void)> cbfunc = std::bind(&MyClass::callbackFunc, cl);
    
    myImGuiDialog::runModal(cl->getParms(), cbfunc);
    
    std::cout << "end" << std::endl;
    return 0;
}
