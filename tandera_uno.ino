/*
 Projeto Tandera uno
 Rastreador, bloqueador e botao de panico por controle remoto 433Mhz (HT)com envio SMS

 Microcontrolador: Atmega328P (UNO)em 16MHz
 Programador: Bruno Venancio
 Versao: 1.20
 Email: projeto.tandera.uno@gmail.com
 
 Esse software possui licença do tipo "bemware".
 Se for fazer uso comercial desse projeto,
 considere ajudar alguém.
 Mateus 25:40

 */

#include <TinyGPS++.h>            // http://arduiniana.org/libraries/tinygpsplus/
#include <SoftwareSerial.h>
#include <avr/io.h>
//#include "LowPower.h"
SoftwareSerial SerialGPS(12, -1); // RX , TX

TinyGPSPlus gps;

#include <EEPROM.h>

/*
    __               __
   / /  ___ ________/ /    _____ ________
  / _ \/ _ `/ __/ _  / |/|/ / _ `/ __/ -_)
 /_//_/\_,_/_/  \_,_/|__,__/\_,_/_/  \__/

 */

const int buzzer = 9;
const int ledVm2 = 7;
const int ledVm1 = 7;
const int ledVm3 = 7;
const int pinoPanico = 2;
const int mosfetGPS = 6;

//const int pinoGravar = 2;
static const uint32_t GPSBaud = 4800;

#include "RF433recv.h"   // https://github.com/sebmillet/RF433recv

#define PIN_RFINPUT  3
#define INT_RFINPUT  1

#define ARRAYSZ(a) (sizeof(a) / sizeof(*a))
#define ASSERT_OUTPUT_TO_SERIAL
RF_manager rf(PIN_RFINPUT, INT_RFINPUT);

/*
 _   __
 | | / /__ ________
 | |/ / _ `/ __(_-<
 |___/\_,_/_/ /___/

 */
char tel_mestre[12];
//char tel_sos1[12];

byte ByteGPS;
byte minCounter = 0;
volatile long _longitude;
volatile long _latitude;
char _latMinute[10];
char _lonMinute[10];
char _quadranteNS;
char _quadranteLO;
float _FGlatitude;
float _FGlongitude;
int _vmax = 0;
int _manobrista = 0;
int _alarme = 0;
volatile int _direcao;
volatile int _velocidade;
String _buffer_serial = "";
String _buffer_numero = "";
String _buffer_sms = "";
String _buffer;
int SMS_index;
String _Resp = "";
String SMS_numero;
String SMS_msg;
uint8_t trackInterval = 1;
byte tomorrowY;
byte tomorrowM;
byte tomorrowD;

String APN;
String USER;
String PWD;

struct {
	long EElongitude;
	long EElatitude;
	unsigned int EEdirecao;
	unsigned int EEvelocidade;
} EEline[121], EEatn[5], EEpfinal[1];

/*
    ______
   / __/ /__ ____ ____
  / _// / _ `/ _ `(_-<
 /_/ /_/\_,_/\_, /___/
            /___/
 */
volatile bool _FLAG_CALL = false;
volatile bool _FLAG_SMS = false;
volatile bool _FLAG_PANICO = false;
volatile bool _FLAG_SMS_ERROR = false;
volatile bool _FLAG_ACOBRAR = false;
volatile bool _FLAG_ENABLE_RF = false;

/*
   _____      _
  / ____|    | |
 | (___   ___| |_ _   _ _ __
  \___ \ / _ \ __| | | | '_ \
  ____) |  __/ |_| |_| | |_) |
 |_____/ \___|\__|\__,_| .__/
                       | |
                       |_|
 */
void setup() {

	// pinMode(pinoGravar, INPUT);

	pinMode(buzzer, OUTPUT);
	pinMode(mosfetGPS, OUTPUT);
	pinMode(PIN_RFINPUT, INPUT);
	digitalWrite(buzzer, LOW);
	digitalWrite(mosfetGPS, LOW);

	Serial.begin(115200);
	SerialGPS.begin(9600);

	if (_FLAG_ENABLE_RF) {
		// Registra controle HT
		rf.register_Receiver(RFMOD_TRIBIT_INVERTED, // mod
				10224, // initseq
				0, // lo_prefix
				0, // hi_prefix
				454, // first_lo_ign
				454, // lo_short
				906, // lo_long
				0, // hi_short (0 => take lo_short)
				0, // hi_long  (0 => take lo_long)
				444, // lo_last
				10224, // sep
				28  // nb_bits
				);

		rf.register_callback(callback1, 2000);
		Serial.print(F("Waiting for signal\n"));

		rf.set_opt_wait_free_433(false);
		rf.activate_interrupts_handler();

	}

	// Inicia Módulo SIM800
	cmds_inicializacao();

	beep();
	beep();

	delay(7000);

	attachInterrupt(digitalPinToInterrupt(pinoPanico), handlePanico, FALLING);

}

/*
  _
 | |
 | |     ___   ___  _ __
 | |    / _ \ / _ \| '_ \
 | |___| (_) | (_) | |_) |
 |______\___/ \___/| .__/
                   | |
                   |_|
 */
void loop() {

	if (Serial.available() > 0) {
		serialEvent();

		Serial.flush();
	}

	if (_FLAG_PANICO) {
		//   Serial.println("PANICO ACIONADO!!!");
		_FLAG_PANICO = false;
	}

	verificaSMSCall();

	if (_FLAG_ENABLE_RF) {
		rf.do_events();
	}

	receive_gps();

	if (gps.location.isUpdated()) {
		carrega_coord();
		carrega_dir();
		carrega_vel();
		minCounter++;

		int rec = digitalRead(13);
		digitalWrite(13, !rec);

		if (minCounter < 5)
			return;
		minCounter = 0;

		if (!_FLAG_ENABLE_RF) {
			processaGprs();
		}

	} else if (!gps.location.isValid() || gps.satellites.value() < 3) {

		digitalWrite(buzzer, !digitalRead(buzzer));

	}

	Serial.flush();

	smartDelay(2500);

	if (gps.charsProcessed() < 10)
		Serial.println(F("Nenhum dado GPS chegando: Checar ligacao"));

}
