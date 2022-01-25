// Comandos SMS

void comandos_SMS(void) {
	String resposta = Recebe_SMS(SMS_index);
	//SerialDebug.println(resposta);
	//SerialDebug.println(SMS_msg);

	if (SMS_msg.indexOf(F("Mapa")) != -1 || SMS_msg.indexOf(F("mapa")) != -1
			|| SMS_msg.indexOf(F("map")) != -1
			|| SMS_msg.indexOf(F("Map")) != -1) {
		char TEL_RESP[16];
		SMS_numero.toCharArray(TEL_RESP, 16);

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
		Message.concat("Direcao: ");
		if (_velocidade < 5) {
			Message.concat("Parado");
		} else {
			Message.concat(retCardeal());
		}
		Message.concat("\n");
		Message.concat("Localizacao:");
		Message.concat("\n");

		Message.concat("http://maps.google.com.br/maps?q=");
		dtostrf(_FGlatitude, 4, 5, Slatitude); //4 is mininum width, 6 is precision
		Message.concat(Slatitude);
		Message.concat(",");
		dtostrf(_FGlongitude, 4, 5, Slongitude);
		Message.concat(Slongitude);
		char frase[Message.length()];
		Message.toCharArray(frase, Message.length());

		envia_sms(TEL_RESP, frase);

	}

	if (SMS_msg.indexOf(F("Vmax")) != -1 || SMS_msg.indexOf(F("vmax")) != -1
			|| SMS_msg.indexOf(F("VMAX")) != -1
			|| SMS_msg.indexOf(F("Velocidade")) != -1) {

		char *frase = "Velocidade Maxima setada.";
		char TEL_RESP[16];
		SMS_numero.toCharArray(TEL_RESP, 16);
		_vmax = 100;

		envia_sms(TEL_RESP, frase);
		delay(4000);

	}

	if (SMS_msg.indexOf(F("Manobrista")) != -1
			|| SMS_msg.indexOf(F("manobrista")) != -1
			|| SMS_msg.indexOf(F("MANOBRISTA")) != -1
			|| SMS_msg.indexOf(F("Manob")) != -1) {

		char *frase = "Funcao manobrista setada.";
		char TEL_RESP[16];
		SMS_numero.toCharArray(TEL_RESP, 16);
		_manobrista = 0xAB;

		envia_sms(TEL_RESP, frase);
		delay(4000);

	}

	if (SMS_msg.indexOf(F("Alarme")) != -1 || SMS_msg.indexOf(F("alarme")) != -1
			|| SMS_msg.indexOf(F("ALARME")) != -1
			|| SMS_msg.indexOf(F("Alarm")) != -1) {
		char *frase = "Alarme ativado!";
		char TEL_RESP[16];
		SMS_numero.toCharArray(TEL_RESP, 16);
		_alarme = 0xAB;

		envia_sms(TEL_RESP, frase);
		delay(4000);

	}

	return;
}

