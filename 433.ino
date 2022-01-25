void callback_generic(const BitVector *recorded) {
	//Serial.print(F("Code received: "));
	char *printed_code = recorded->to_str();

	if (printed_code) {
		//Serial.print(recorded->get_nb_bits());
		// Serial.print(F(" bits: ["));
		Serial.print(printed_code);
		Serial.print(F("]\n"));

		free(printed_code);
	}
}

void callback1(const BitVector *recorded) {
	Serial.print(F("1> "));
	callback_generic(recorded);
}

