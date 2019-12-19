#include "info_device.h"

void list_all_devices(){
	struct libusb_context* ctx;
	if(libusb_init(&ctx) < 0){
		fprintf(stderr, "Init Error\n");
		return;
	}

	struct libusb_device** devs;
	int nbr = libusb_get_device_list(ctx, &devs);
	if(nbr < 0){
		fprintf(stderr, "Get Device List Error\n");
		return;
	}

	for(int i=0; i<nbr; i++)
		device_info(devs[i]);

	libusb_free_device_list(devs, 1);
	libusb_exit(ctx);
}

char* printbites(uint8_t b){
	char* res = (char*)malloc(9*sizeof(char));
	res[8]='\0';
	for(int i=7; i>=0; i--){
		res[i] = (b&1?'1':'0');
		b = b>>1; 
	}
	return res;
}

void device_info(libusb_device* dev){
	struct libusb_device_descriptor desc;

	if (libusb_get_device_descriptor(dev, &desc) < 0) {
		fprintf(stderr, "Get descriptor error\n");
		return;
	}


	if(desc.idVendor == 1356 && desc.idProduct == 4096)
		printf("############BUZZER############\n\n");
	else
		printf("##############################\n\n");

	
	printf("Number of possible configurations: %d\n", desc.bNumConfigurations);
	printf("Device Class: %d\n", desc.bDeviceClass);

	printf("VendorID: %#04x\n", desc.idVendor);
	printf("ProductID: %#04x\n\n", desc.idProduct);
	
	struct libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	
	printf("Interfaces: %d\n\n", config->bNumInterfaces);
	printf("--------------------\n\n");


	const struct libusb_interface *inter;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_endpoint_descriptor *epdesc;
	char* tmp;

	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		printf("Number of alternate settings: %d\n\n", inter->num_altsetting);
		printf("..........\n\n");

		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];

			printf("Interface Number: %d\n", interdesc->bInterfaceNumber);
			printf("Number of endpoints: %d\n\n", interdesc->bNumEndpoints);

			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];

				printf("bLength: %d\n",epdesc->bLength);
				printf("bDescriptorType: %d (Should always be 5)\n",epdesc->bDescriptorType);
				tmp = printbites(epdesc->bEndpointAddress); printf("bEndpointAddress: %s\n",tmp); free(tmp);
				tmp = printbites(epdesc->bmAttributes); printf("bmAttributes: %s\n",tmp); free(tmp);
				printf("wMaxPacketSize: %d\n",epdesc->wMaxPacketSize);
				printf("bInterval: %d\n",epdesc->bInterval);
			}

			if( j != inter->num_altsetting-1) printf("..........\n\n");
		}

		if( i != config->bNumInterfaces-1) printf("--------------------\n\n");
	}
	
	printf("\n\n##############################\n");
	libusb_free_config_descriptor(config);
}