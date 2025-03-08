/*
 myImGuiDialog standalone dialog app for testing.
 To build, run build_win32.bat on your copy of vcvars32.bat or vcvarsall.bat.
*/

#include <stdio.h>
#include <iostream>
#include <functional>
#include "../Source/myImGuiDialog.h"
#include "../Source/MyParms.h"

class MyClass
{
public:
	MyClass() : parms({ 3, 100, 0 }) {};
	void callbackFunc();
	MyParms* getParms() { return &parms; }
private:
	MyParms parms;
};

void MyClass::callbackFunc() {
	std::cout << "drag/vertices=" << parms.vertices\
		<< "/scale=" << parms.scale\
        << "/angle=" << parms.angle << std::endl;
}

int main(int argc, const char * argv[]) {
	MyClass* cl = new MyClass();
	std::function<void(void)> cbfunc = std::bind(&MyClass::callbackFunc, cl);

	HWND hwnd = NULL;
	int result = myImGuiDialog::runModal(hwnd, cl->getParms(), cbfunc);

	std::cout << "result=" << result << std::endl;
	cbfunc();

	return 0;
}
