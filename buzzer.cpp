#include "buzzer.hpp"

using namespace bz;


//##Constructer & Destructeur
Buzz::Buzz(){
	if(libusb_init(&_ctx) < 0)
		std::cerr << "Init Error" << std::endl;

	//Is device connected?
	//Enough rights?
	_handle = libusb_open_device_with_vid_pid(_ctx, VENDOR_ID, PRODUCT_ID);
	if(_handle==NULL)
		std::cerr << "Open Device Error" << std::endl << "Device not connected? Not enough permission?" << std::endl;


	if(libusb_kernel_driver_active(_handle, 0) == 1)
		libusb_detach_kernel_driver(_handle, 0);

	if(libusb_claim_interface(_handle, 0) < 0)
		std::cerr << "Claim Interface Error" << std::endl;

	_state = (unsigned char*)calloc(0, 5*sizeof(unsigned char));
	if(_state == NULL)
		std::cerr << "Allocation failed" << std::endl;

	_is_synchronous = 1;
	_guirlande=0;
}

Buzz::~Buzz(){
	this->synchronous();
	free(_state);
	libusb_release_interface(_handle, 0);
	libusb_close(_handle);
	libusb_exit(_ctx);
}

//Recuperation des donnees
void Buzz::asynchronous(){
	if(!_is_synchronous)
		return;

	libusb_fill_interrupt_transfer(_transfer, _handle, END_POINT, _state, 5, &new_data, this, 0);
	
	if(libusb_submit_transfer(_transfer) < 0)
		std::cerr << "Submit transfer Error" << std::endl;

	_is_synchronous = 0;
}

void Buzz::synchronous(){
	if(_is_synchronous)
		return;
	
	libusb_cancel_transfer(_transfer);

	_is_synchronous = 1;
}

void Buzz::updateStatus(int timeout){
	if(_is_synchronous)
		libusb_interrupt_transfer(_handle, END_POINT, _state, 5, NULL, timeout);
}

void bz::new_data(libusb_transfer* transfer){
	Buzz* buzz = (Buzz*)transfer->user_data;
	if(transfer->status == LIBUSB_TRANSFER_CANCELLED)
		return;

	if(transfer->status < 0)
		std::cerr << "Transfer Error " << std::endl;

	libusb_fill_interrupt_transfer(transfer, buzz->_handle, END_POINT, buzz->_state, 5, &new_data, buzz, 0);
	
	if(libusb_submit_transfer(buzz->_transfer) < 0)
		std::cerr << "Submit transfer Error" << std::endl;
}

//##Affichage de leds
void Buzz::setLight(uint8_t players)const{
	unsigned char* lights = (unsigned char*)malloc(7*sizeof(unsigned char));
	int i=0;
	lights[i++] = OFF;

	lights[i++] = (players&0b0001?ON:OFF);
	lights[i++] = (players&0b0010?ON:OFF);
	lights[i++] = (players&0b0100?ON:OFF);
	lights[i++] = (players&0b1000?ON:OFF);

	lights[i++] = OFF;
	lights[i++] = OFF;
	
	libusb_control_transfer(_handle, REQUEST_TYPE, REQUEST, W_VALUE, W_INDEX, lights, 7, 50);

	free(lights);
}

void Buzz::guirlande()const{
	uint8_t i=8;
	while(_guirlande){
		i = i << 1;
		if(i>8) i=1;
		setLight(i);
		usleep(200000);
	}
}

void bz::thread_start(const Buzz* buzzer){
	buzzer->guirlande();
}

void Buzz::guirlande_start(){
	guirlande_stop();
	_guirlande = 1;
	_go = std::thread(&thread_start,this);
}
void Buzz::guirlande_stop(){
	if(_guirlande){
		_guirlande=0;
		_go.join();
	}
}

//##Interpretation de donnees
uint8_t Buzz::get_Pressed_Player() const{
	return ((_state[2]&0x1F) ? 0x01:0) |
		(((_state[2]&0xE0) | (_state[3]&0x03)) ? 0x02:0) |
		(((_state[3]&0x7C) ? 0x04:0)) |
		((((_state[3]&0x80) | (_state[4]&0xF)) ? 0x08:0));
}

uint8_t Buzz::getController(int player) const{
	switch(player){
		case 1:
			return 	((_state[2]&0x01)?0x01:0)|
				((_state[2]&0x10)?0x02:0)|
				((_state[2]&0x08)?0x04:0)|
				((_state[2]&0x04)?0x08:0)|
				((_state[2]&0x02)?0x10:0);
		case 2:
			return (_state[2]&0x20)|
				((_state[3]&0x02)?0x02:0)|
				((_state[3]&0x01)?0x04:0)|
				((_state[2]&0x80)?0x08:0)|
				((_state[2]&0x40)?0x10:0);
		case 3:
			return (_state[3]&0x04)|
				((_state[3]&0x40)?0x02:0)|
				((_state[3]&0x20)?0x04:0)|
				((_state[3]&0x10)?0x08:0)|
				((_state[3]&0x08)?0x10:0);
		case 4:
			return (_state[3]&0x80)|
				((_state[4]&0x08)?0x02:0)|
				((_state[4]&0x04)?0x04:0)|
				((_state[4]&0x02)?0x08:0)|
				((_state[4]&0x01)?0x10:0);

		default:
			return 0;
	}
}

uint8_t Buzz::getBuzz() const{
	return ((_state[2]&0x01)?0x01:0) | ((_state[2]&0x20)?0x02:0) | ((_state[3]&0x04)?0x04:0) | ((_state[3]&0x80)?0x08:0);
}

std::ostream& bz::operator<<(std::ostream& cout, const Buzz& buzz){
	cout << " " << (buzz._state[2]&0x01?'#':'_') << " | " << (buzz._state[2]&0x20?'#':'_') << " | " << (buzz._state[3]&0x04?'#':'_') << " | " << (buzz._state[3]&0x80?'#':'_') << std::endl;
	cout << " " << (buzz._state[2]&0x10?'#':'_') << " | " << (buzz._state[3]&0x02?'#':'_') << " | " << (buzz._state[3]&0x40?'#':'_') << " | " << (buzz._state[4]&0x08?'#':'_') << std::endl;
	cout << " " << (buzz._state[2]&0x08?'#':'_') << " | " << (buzz._state[3]&0x01?'#':'_') << " | " << (buzz._state[3]&0x20?'#':'_') << " | " << (buzz._state[4]&0x04?'#':'_') << std::endl;
	cout << " " << (buzz._state[2]&0x04?'#':'_') << " | " << (buzz._state[2]&0x80?'#':'_') << " | " << (buzz._state[3]&0x10?'#':'_') << " | " << (buzz._state[4]&0x02?'#':'_') << std::endl;
	cout << " " << (buzz._state[2]&0x02?'#':'_') << " | " << (buzz._state[2]&0x40?'#':'_') << " | " << (buzz._state[3]&0x08?'#':'_') << " | " << (buzz._state[4]&0x01?'#':'_') << std::endl;

	return cout;
}