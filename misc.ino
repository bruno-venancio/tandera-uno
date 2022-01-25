/*
 ___ ___ ___ ___   _   _
 / __| __| _ \_ _| /_\ | |
 \__ \ _||   /| | / _ \| |__
 |___/___|_|_\___/_/ \_\____|

 */

void serialEvent() {

	_buffer_serial = "";
	_buffer_serial = Serial.readString();

	//SerialDebug.print(num);
	//SerialDebug.print(". ");

	if (_buffer_serial.indexOf(F("+CLIP:")) != -1) {
		String CLIPvalue = _buffer_serial.substring(
				_buffer_serial.indexOf(F("+CLIP:")));
		_buffer_numero = CLIPvalue.substring(CLIPvalue.indexOf(F("\"")) + 1,
				CLIPvalue.indexOf(F("\",")));
		_buffer_numero.trim();
		_FLAG_CALL = true;
	}
	if (_buffer_serial.indexOf(F("+CMTI:")) != -1) {
		String CMTIvalue = _buffer_serial.substring(
				_buffer_serial.indexOf(F("+CMTI:")));
		_buffer_sms = CMTIvalue.substring(CMTIvalue.indexOf("\",") + 2);
		_buffer_sms.trim();
		SMS_index = _buffer_sms.toInt();
		_FLAG_SMS = true;
	}

}

void verificaSMSCall(void) {
	if (_FLAG_SMS) {
		_FLAG_SMS = false;
		delay(500);
		comandos_SMS();
		return;
	}

	if (_FLAG_CALL) {
		_FLAG_CALL = false;
		_HANGUP();
		delay(2000);

		call_send_mapa (_buffer_numero);

		delay(2000);

		return;

	}
}

void beep(void) {
	digitalWrite(buzzer, HIGH);
	delay(200);
	digitalWrite(buzzer, LOW);

}
//$GPRMC,190903,A,1549.0560,S,04758.7610,W,001.0,180.0,191221,001.0,E*7E

void receive_gps(void) {

	while (SerialGPS.available() > 0) {
		//yield();
		ByteGPS = SerialGPS.read();

		if (gps.encode(ByteGPS))
			return;
	}
}

bool carrega_coord(void) {
	double _Flatitude;
	double _Flongitude;
	double f;
	double i;

	volatile long Tempt;

	if (gps.location.isValid()) {
		_Flatitude = gps.location.lat();
		_FGlatitude = gps.location.lat();
		_latitude = long(fabs(_Flatitude * 100000UL));

		f = modf(_Flatitude, &i);
		f = fabs(f * 60);
		i = fabs(i * 100);
		i = i + f;
		dtostrf(i, 7, 4, _latMinute);

		_Flongitude = gps.location.lng();
		_FGlongitude = gps.location.lng();
		_longitude = long(fabs(_Flongitude * 100000UL));

		f = modf(_Flongitude, &i);
		f = fabs(f * 60);
		i = fabs(i * 100);
		i = i + f;
		dtostrf(i, 7, 4, _lonMinute);

		return true;
	} else {
		return false;
	}
}

bool carrega_dir(void) {
	float _Fdirecao;
	if (gps.course.isValid()) {
		_Fdirecao = gps.course.deg();
		_direcao = int(fabs(_Fdirecao));
		return true;
	} else {
		return false;
	}
}

bool carrega_vel(void) {
	float _Fvelocidade;

	if (gps.speed.isValid()) {
		_Fvelocidade = gps.speed.kmph();
		_velocidade = int(fabs(_Fvelocidade));
		return true;
	} else {
		return false;
	}
}

/*  +---------------------------------------------+
 | Delay modificado para manter o objeto GPS   |
 | recebendo dados                             |
 +---------------------------------------------+
 */
static void smartDelay(unsigned long ms) {
	unsigned long start = millis();
	do {
		while (SerialGPS.available())
			gps.encode(SerialGPS.read());
	} while (millis() - start < ms);
}

/* 
 const char *TinyGPSPlus::cardinal(double course)
 {
 static const char* directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
 int direction = (int)((course + 11.25f) / 22.5f);
 return directions[direction % 16];
 }

 */

String retCardeal(void) {
	String dir;
	if (_direcao == 0) {
		dir = "Norte";
		return dir;
	} else if (_direcao > 0 && _direcao < 30) {
		dir = "Norte";
		return dir;
	} else if (_direcao > 30 && _direcao < 60) {
		dir = "Nordeste";
		return dir;
	} else if (_direcao > 60 && _direcao < 105) {
		dir = "Leste";
		return dir;
	} else if (_direcao > 105 && _direcao < 150) {
		dir = "Sudeste";
		return dir;
	} else if (_direcao > 150 && _direcao < 195) {
		dir = "Sul";
		return dir;
	} else if (_direcao > 195 && _direcao < 240) {
		dir = "Sudoeste";
		return dir;
	} else if (_direcao > 240 && _direcao < 300) {
		dir = "Oeste";
		return dir;
	} else if (_direcao > 300 && _direcao < 330) {
		dir = "Noroeste";
		return dir;
	} else if (_direcao > 330 && _direcao < 360) {
		dir = "Norte";
		return dir;
	}
}

void limpa_serial(void) {
	unsigned char temp = Serial.available();

	for (int i = temp; i >= 0; i--)
		Serial.read();
}

void CalcTomorrow(long g) {

	g = g + 1; // add one day

	int y = (10000 * g + 14780) / 3652425;
	int ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
	if (ddd < 0) {
		y = y - 1;
		ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
	}
	int mi = (100 * ddd + 52) / 3060;
	int mm = (mi + 2) % 12 + 1;
	y = y + (mi + 2) / 12;
	int dd = ddd - (mi * 306 + 5) / 10 + 1;

	tomorrowY = y;
	tomorrowM = mm;
	tomorrowD = dd;
}

long g(int y, int m, int d) {

	m = (m + 9) % 12;
	y = y - m / 10;
	return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 1);
}

/*
 *  Vetor interrupção do pânico
 */
void handlePanico(void) {
	_FLAG_PANICO = true;
}
