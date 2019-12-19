#include "main.hpp"

using namespace std;

int main(int argc, char** argv){
	try{
		bz::Buzz buzzer;
		uint8_t res;
		buzzer.guirlande_start();

		for(int i=0; i<50; i++){
			buzzer.updateStatus();
			res = buzzer.get_Pressed_Player();
			if(res>0){
				buzzer.guirlande_stop();
				buzzer.setLight(res);

                pressSpace();
				
				buzzer.updateStatus(50);
				buzzer.guirlande_start();
			}
		}
	}catch(const char* msg){
		cerr << msg << endl;
	}
	return 0;
}

#ifdef WINDOWS
void pressSpace(){
    const int KEYEVENT_KEYUP = 0x02;
	keybd_event(VK_SPACE,0,0,0);
	keybd_event(VK_SPACE,0,KEYEVENT_KEYUP,0);
}
#else
void pressSpace(){
    xdo_t * x = xdo_new(NULL);
    xdo_send_keysequence_window(x, CURRENTWINDOW, "k", 0);
}
#endif