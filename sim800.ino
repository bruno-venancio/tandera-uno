bool get_resp_string(char *resp, int timeOut) {

	_buffer = "";
	long timeOld = millis();

	while (!Serial.available() && !(millis() > timeOld + timeOut)) {
		yield();
	}

	if (Serial.available()) {
		_buffer = Serial.readString();
	}

	////SerialDebug.print("->"); 
	////SerialDebug.println(_buffer);

	if (_buffer.indexOf(resp) != -1) {
		Serial.flush();
		return true;
	} else {
		Serial.flush();
		return false;
	}

}

/*
 *  Le dados na serial e insere na string _buffer
 */

void _readSerial() {

	_buffer = "";
	long timeOld = millis();

	while (!Serial.available() && !(millis() > timeOld + 3000)) {
		yield();
	}

	if (Serial.available()) {
		_buffer = Serial.readString();
	}
}

void _readSerial_10() {

	_buffer = "";
	long timeOld = millis();

	while (!Serial.available() && (millis() < timeOld + 10000)) {
		delay(1);
	}

	if (Serial.available()) {
		_buffer = Serial.readString();
	} else {
		_buffer = "";
		Serial.println("buffer vazio! rotina readserial estourou101010");
	}
}

void _readSerial_6() {

	_buffer = "";
	long timeOld = millis();

	while (!Serial.available() && (millis() < timeOld + 7000)) {
		delay(1);
	}
	if (Serial.available()) {
		_buffer = Serial.readString();
	} else {
		_buffer = "";
		Serial.println("buffer vazio! rotina readserial estourou6666");
	}
}

bool _fecha_mic() {

	Serial.print("AT+CEXTERNTONE=1\r");
	if (!GSM_RESP()) {
		////SerialDebug.println("ERRO FECHA MIC");
		return false;
	} else {
		return true;
	}
}

bool _abre_mic() {

	Serial.print("AT+CEXTERNTONE=0\r");
	if (!GSM_RESP()) {
		////SerialDebug.println("ERRO ABRE MIC");
		return false;
	} else {
		return true;
	}
}

bool GSM_RESP(void) {

	if (get_resp_string("OK", 200)) {
		return true;
	} else {
		return false;
	}

}

void _HANGUP(void) {
	Serial.print("ATH\r");
	if (!GSM_RESP()) {
		////SerialDebug.println("Erro HANGUP");
		delay(1500);
		Serial.print("ATH\r");
		delay(500);
	}
}

void cmds_inicializacao(void) {

	Serial.print("ATZ\r");

	delay(50);
	// Serial.print("ATE0\r");
	delay(50);

	Serial.print("AT+CLTS=1\r");
	delay(50);

	Serial.print("AT+CREG=0\r");
	delay(50);

	Serial.print("AT+CMGF=1\r");
	delay(30);

	Serial.print("AT+CLIP=1\r");
	delay(30);

	//Microfone ganho maximo
	Serial.print("AT+CMIC=0,15\r");
	delay(30);

	//ganho do AUDIO maximo
	Serial.print("ATL9\r");
	delay(30);
	Serial.print("ATM9\r");
	delay(30);

	Serial.print("AT+DDET=1,0,0\r");
	delay(30);
	Serial.print("AT+CMICBIAS=1\r");
	delay(30);
	Serial.print("AT+CAGCSET=1\r");
	delay(30);
	Serial.print("AT+CLVL=100\r");
	delay(30);
	Serial.print("AT+CRSL=100\r");
	delay(30);

	Serial.print("AT\r");
	delay(30);

	Serial.print("AT+ECHO=0,150,254,254,253,1\r");
	delay(30);

	Serial.print("AT+CMGDA=\"DEL ALL\"\r");

	if (!GSM_RESP()) {
		////SerialDebug.println("Erro CMGDA");
	}

	delay(100);

	Serial.print(F("AT+SAPBR=0,1\r"));
	delay(100);
 

}

/*
 *   recebe mensagem SMS
 */

String Recebe_SMS(int index) {
	String buffer = "";
	_buffer_sms = "";
	Serial.print(F("AT+CMGR="));
	Serial.print(index);
	Serial.print("\r");
	buffer = _readData(); //reads the result
	if (buffer.indexOf("CMGR:") != -1) {

		String CMGRvalue = buffer.substring(buffer.indexOf(F("+CMGR:")));
		SMS_numero = CMGRvalue.substring(CMGRvalue.indexOf(F(",")) + 5,
				CMGRvalue.indexOf(F(",")) + 16);
		SMS_numero.trim();
		SMS_msg = CMGRvalue.substring(CMGRvalue.indexOf(F("-")) + 4);
		SMS_msg.trim();
		return SMS_numero;
	} else
		return "";
}

/*
 *   Envia mensagem SMS
 */
bool sendSMS(char *number, char *message) {

	String str = "";
	delay(1000);
	Serial.print("AT+CMGS=\"");  // Comando para enviar SMS
	Serial.print(number);
	Serial.print("\"\r");
	_readSerial();
	str += _buffer;
	//Serial.print("Serie:");
	Serial.print(message);
	Serial.print("\r");
	 
	_readSerial();
	str += _buffer;
	Serial.print((char) 26);

	//_buffer += _readSerial();
	_readSerial_10();
	str += _buffer;
	//expect CMGS:xxx   , onde xxx eh o numero para envio do SMS

	// return str;
	/**/
	if (str.indexOf("+CMGS:") != -1) {

		return true;
	} else {
		return false;
	}
}

String _readData() {

	uint16_t timeout = 0;
	while (!Serial.available() && timeout < 10000) {
		delay(10);
		timeout++;
	}
	if (Serial.available()) {
		String output = Serial.readString();

		return output;
	}
}

/*
 *  Qualidade do sinal GSM
 */
//  0-31 | 0-> baixo | 31 - forte | 99 -> desconhecido
unsigned int qualidade_sinal() {

	Serial.print(F("AT+CSQ\r"));
	_readSerial();

	if ((_buffer.indexOf(F("+CSQ:"))) != -1) {
		return _buffer.substring(_buffer.indexOf(F("+CSQ: ")) + 6,
				_buffer.indexOf(F(","))).toInt();
	} else {
		return 99;
	}
}

/*
 *  Envia SMS com reenvio do mesmo a cobrar, 
 *  em caso de falta de creditos, afinal... quem nunca ne?
 */

void envia_sms(char *TEL_RESP, char *frase) {
	if (sendSMS(TEL_RESP, frase)) {
		_readSerial_10;
		if (_buffer == "") {

			delay(3000);

		} else if (_buffer.indexOf(F("ERROR")) != -1) {
			_FLAG_SMS_ERROR = true;

			delay(4500);
			String StrAcobrar = "9090";
			StrAcobrar += TEL_RESP;
			StrAcobrar.toCharArray(TEL_RESP, 16);
			sendSMS(TEL_RESP, frase);
			delay(4000);
			return;

		} else {
			_FLAG_SMS_ERROR = false;

			delay(2000);
		}
	} else {
		_FLAG_SMS_ERROR = true;

		delay(6000);
		String StrAcobrar = "9090";
		StrAcobrar += TEL_RESP;
		StrAcobrar.toCharArray(TEL_RESP, 16);
		sendSMS(TEL_RESP, frase);
		delay(4000);
	}
}

void call_send_mapa(String Numero) {
	char TEL_RESP[16];
	if (Numero.charAt(0) == '0') {
		Numero.setCharAt(0, ' ');
		Numero.trim();
	}
	Numero.toCharArray(TEL_RESP, 13);
	String Message = "";
	char Slatitude[10];
	char Slongitude[10];

	// Mostra data
	int gpsDia = gps.date.day();
	int gpsMes = gps.date.month();
	Message.concat("Data: ");
	Message.concat(gpsDia);
	Message.concat("/");
	Message.concat(gpsMes);
	Message.concat("\n");
	Message.concat("Horario: ");
	// Mostra horas
	int gpsHour = gps.time.hour();
	gpsHour -= 3;  // GMT -3 para Brasil
	if (gpsHour < 0)
		gpsHour = gpsHour + 24;
	// Mostra minutes
	char minutesStr[3];
	int gpsMinute = gps.time.minute();
	sprintf(minutesStr, "%02d", gpsMinute);
	Message.concat(gpsHour);
	Message.concat(":");
	Message.concat(minutesStr);
	Message.concat("\n");
	Message.concat("Velocidade: ");

	Message.concat(_velocidade);
	Message.concat("Km/h \n");
	Message.concat("Direção: ");
	if (_velocidade < 5) {
		Message.concat("Parado");
	} else {
		Message.concat(retCardeal());
	}
	Message.concat("\n");
	Message.concat("Localizacao:");
	Message.concat("\n");

	Message.concat("http://maps.google.com.br/maps?q=");
	dtostrf(_FGlatitude, 4, 5, Slatitude);  //4 is mininum width, 6 is precision
	Message.concat(Slatitude);
	Message.concat(",");
	dtostrf(_FGlongitude, 4, 5, Slongitude);
	Message.concat(Slongitude);
	char frase[Message.length()];
	Message.toCharArray(frase, Message.length());

	if (sendSMS(TEL_RESP, frase)) {
		_readSerial_10;
		if (_buffer == "") {

			delay(3000);

		} else if (_buffer.indexOf(F("ERROR")) != -1) {
			_FLAG_SMS_ERROR = true;

			delay(4500);
			String StrAcobrar = "9090";
			StrAcobrar += TEL_RESP;
			StrAcobrar.toCharArray(TEL_RESP, 16);
			sendSMS(TEL_RESP, frase);
			delay(4000);
			return;

		} else {
			_FLAG_SMS_ERROR = false;

			delay(2000);
		}
	} else {
		_FLAG_SMS_ERROR = true;

		delay(6000);
		String StrAcobrar = "9090";
		StrAcobrar += TEL_RESP;
		StrAcobrar.toCharArray(TEL_RESP, 16);
		sendSMS(TEL_RESP, frase);
		delay(4000);
	}
	return;
}

// OPERATOR NAME +
String getOperadora() {
	Serial.print(F("AT+COPS?\r"));
	_readSerial();

	if (_buffer.indexOf(F(",")) == -1) {
		return "NOT CONNECTED";
	} else {
		return _buffer.substring(_buffer.indexOf(F(",\"")) + 2,
				_buffer.lastIndexOf(F("\"")));
	}
}

String getModuleIMEI() {
	Serial.print(F("AT+CGSN\r"));
	_readSerial();

	String saida = _buffer.substring(8, _buffer.indexOf(F("OK")));
	saida.trim();
	return saida;
}
