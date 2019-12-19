#pragma once
#include <ostream>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <libusb.h>

namespace bz{
//Product
static const uint16_t VENDOR_ID = 0x054C;
static const uint16_t PRODUCT_ID = 0x1000;

//Get data
static const unsigned char END_POINT = 0x81;

//Set lights
static const uint8_t REQUEST_TYPE = 0x21;
static const uint8_t REQUEST = 0x09;
static const uint16_t W_VALUE = 0x0200;
static const uint16_t W_INDEX = 0x0000;

static const unsigned char ON = 0xFF;
static const unsigned char OFF = 0x00;


class Buzz{
	private:
		unsigned char* _state;

		libusb_device_handle* _handle;
		libusb_context* _ctx;
		libusb_transfer* _transfer;

		int _is_synchronous;
		int _guirlande;
		void guirlande()const;
		std::thread _go;

	public:
		Buzz();
		~Buzz();

		/* Passe en mode synchrone ou asynchrone.
		*  En mode asynchrone, le status est mis a jour a chauqe pression d'un bouton.
		*  En mode syncrhone, la fonction updateStatus doit etre explicitement appellee.*/
		void asynchronous();
		void synchronous();

		/* En mode synchrone, met a jour le status des buzzer.
		*  Si le timeout = 0, la fonction bloque jusqu'a ce qu'un buzzer soit presse.*/
		void updateStatus(int timeout=0);

		/* Retourne quel joueur a un bouton presse.
		*  Sous une forme binaire (4bit).
		*  2⁰ -> J1	2¹ -> J2	2² -> J3	2⁴ -> J8*/
		uint8_t get_Pressed_Player() const;

		/* Retourne quel joueur a le buzz presse.
		*  Sous une forme binaire (4bit).
		*  2⁰ -> J1	2¹ -> J2	2² -> J3	2⁴ -> J8*/
		uint8_t getBuzz() const;

		/* Retourne l'etat de la manette du joueur [1..4].
		*  Sous forme binaire (5bit).
		*  2⁰ -> Buzz	2¹ -> bleu	2² -> orange	2³ -> vert	2⁴ -> jaune.*/
		uint8_t getController(int player) const;

		/* Permet d'allumer les buzzers.
		*  Si (player & 2⁰) alors manette 1 sera ON
		*  etc.*/
		void setLight(uint8_t player) const;

		/* Lance ou arrete un thread qui allume en boucle les manettes:
		*  1 puis 2 puis 3 puis 4*/
		void guirlande_start();
		void guirlande_stop();

		friend std::ostream& operator<<(std::ostream& cout, const Buzz& buzz);
		friend void thread_start(const Buzz*);
		friend void new_data(libusb_transfer* transfer);
};

std::ostream& operator<<(std::ostream& cout, const Buzz& buzz);

/*NE PAS UTILISER*/
void new_data(libusb_transfer* transfer);

/*NE PAS UTILISER*/
void thread_start(const Buzz*);
}