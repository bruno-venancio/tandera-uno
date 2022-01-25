void processaGprs(void) {
	gprsInit("claro.com.br", "claro", "claro");
	// gprsInit("vivo.com.br","vivo","vivo");
	gprsConnect();
	conectaServidor();
	delay(1000);
	conecta_orange1();
	delay(3000);
	conecta_orange2();
	delay(1000);
}

void gprsInit(String apn, String user, String password) {
	APN = apn;
	USER = user;
	PWD = password;
}

// Conexao GPRS
bool gprsConnect() {

	  Serial.print(F("AT+CGATT=1\r"));
	  delay(500);

	  Serial.print(F("AT+CGDCONT=1,\"IP\",\""));
	  Serial.print(APN);
	  Serial.print("\"\r");
	_readSerial();
	if (_buffer.indexOf(F("OK")) != -1) {
		delay(200);
		Serial.print(F("AT+CSTT=\""));
		Serial.print(APN);
		Serial.print("\",\"");
		Serial.print(USER);
		Serial.print("\",\"");
		Serial.print(PWD);
		Serial.print(F("\"\r"));
		_readSerial();
		if (_buffer.indexOf(F("OK")) != -1) {
			_buffer = "";
			return true;
		}

	} else {
		_buffer = "";
		return false;
	}

}

bool conectaServidor(void) {
	Serial.print(F("AT+CIICR\r"));
	// _readSerial();
	delay(800);
	Serial.print(F("AT+CIFSR\r"));
	delay(800);
	Serial.print(F("AT+CIPHEAD=1\r"));
	delay(200);

	Serial.print(F("AT+CIPSTART=\"TCP\",\"193.193.165.37\",\"26731\"\r"));

	delay(2500);

}

bool conecta_orange1(void) {

	limpa_serial();
	Serial.print(F("AT+CIPSEND\r"));
	_readSerial();
	if (_buffer.indexOf("ERROR") != -1) {
		delay(1000);
		Serial.print(F("AT+CIPSEND\r"));
	}
	delay(1000);
	envia_cabecalho();
	delay(1000);
	_buffer = "";
}

bool conecta_orange2(void) {

	Serial.print(F("AT+CIPSEND\r"));
	_readSerial();
	if (_buffer.indexOf("ERROR") != -1) {
		delay(1000);
		Serial.print(F("AT+CIPSEND\r"));
	}
	delay(2500);
	envia_string_gprs(gps.date, gps.time);
	_buffer = "";
	return true;

}

// Check is GPRS connected?
bool gprsIsConnected() {
	Serial.print(F("AT+SAPBR=2,1\r"));
	_readSerial();
	if (_buffer.indexOf(F("ERROR")) != -1
			|| _buffer.indexOf(F("\"0.0.0.0\"")) != -1) {
		return false;
	} else {
		return true;
	}
}

// GET IP Address
String gprsGetIP() {
	Serial.print(F("AT+SAPBR=2,1\r\n"));
	_readSerial();
	if (_buffer.indexOf(F("ERROR")) != -1
			|| _buffer.indexOf(F("\"0.0.0.0\"")) != -1) {
		return "ERROR:NO_IP";
	} else {
		if (_buffer.indexOf("+SAPBR:") != -1) {
			String res = _buffer.substring(_buffer.indexOf(F(",\"")) + 2,
					_buffer.lastIndexOf(F("\"")));
			res.trim();
			return res;
		} else {
			"ERROR:NO_IP_FETCH";
		}
	}
}

// Fecha conexao GPRS
bool gprsCloseConn() {
	Serial.print(F("AT+SAPBR=0,1\r"));
	_readSerial();
	if (_buffer.indexOf(F("OK")) != -1) {
		return true;
	} else {
		return false;
	}
}

bool envia_cabecalho(void) {
	String frase;
	frase = "##,imei:869286038679861,A;";
	Serial.print(frase);
	Serial.print((char) 26);
	_readSerial_6();
	if (_buffer.indexOf("LOAD") != -1) {
		frase = "";
		return true;
	} else {
		frase = "";
		return true;
	}
}

bool envia_string_gprs(TinyGPSDate &d, TinyGPSTime &t) {
	String frase;
	int gpsDia = d.day();
	int gpsMes = d.month();
	int gpsAno = d.year();
	gpsAno -= 2000;
	int gpsHora = t.hour();
	gpsHora += 3; //Brasil timezone

	if (gpsHora > 23) {
		gpsHora -= 24;
		CalcTomorrow (g(byte (gpsAno), byte (gpsMes), byte (gpsDia)));gpsAno
		= tomorrowY;
		gpsMes = tomorrowM;
		gpsDia = tomorrowD;
	}
	char time[6];
	sprintf(time, "%02d%02d%02d", gpsAno, gpsMes, gpsDia);
	frase = "imei:869286038679861,001,";
	frase.concat(time);
	frase.concat("0000");
	frase.concat(",,F,");
	int gpsMinuto = t.minute();
	int gpsSegundo = t.second();
	sprintf(time, "%02d%02d%02d", gpsHora, gpsMinuto, gpsSegundo);
	frase.concat(time);

	frase.concat(".000,A,");
	frase.concat(_latMinute);
	frase.concat(",S,");
	frase.concat(_lonMinute);
	//frase.concat(",W,0.00,,;");
	frase.concat(",W,");
	frase.concat(gps.speed.knots());
	frase.concat(',');
	frase.concat(_direcao);
	frase.concat(",;");
	Serial.print(frase);
	Serial.print((char) 26);

	return true;

}
