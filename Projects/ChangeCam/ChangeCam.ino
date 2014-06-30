	// Consts

const int PINSpeed     = A0;
const int PPowerLED    = 13;
const int PPauseButton = 2;
const int PCOut        = 11;
const int PC12         = 7;
const int PC34         = 8;
const int MINSpeed     = 1000;
const int MAXSpeed     = 10000;

// Vars
int currentCAM;
int Tdelay;

/* --------------------------------------------------------------------------- */


void openCam () {

	switch (currentCAM) {
	
		case 0:
			digitalWrite(PCOut,LOW);
			digitalWrite(PC12, LOW);	
			return;
		break;
		case 1:
			digitalWrite(PCOut,LOW );
			digitalWrite(PC12, HIGH);
			return;
		break;
		case 2:
			digitalWrite(PCOut,HIGH);
			digitalWrite(PC34, LOW );
			return;
		break;
		case 3:
			digitalWrite(PCOut,HIGH);
			digitalWrite(PC34, HIGH);
			return;
		break;
	}
}

void readSpeed () {

	int readValue;
	readValue = analogRead( PINSpeed );            
	Tdelay = map(readValue, 0, 1023, MINSpeed, MAXSpeed);  
}

void setup () {

	currentCAM = 0;
	digitalWrite(PCOut,LOW);
	digitalWrite(PC12, LOW);
	digitalWrite(PC34, LOW);

	pinMode (PPauseButton, INPUT );
	pinMode (PPowerLED,    OUTPUT);		
	pinMode (PCOut,        OUTPUT);	
	pinMode (PC12,         OUTPUT);	
	pinMode (PC34,         OUTPUT);		

	digitalWrite(PPowerLED, HIGH);
}

void loop () {

	readSpeed ();
	
	if (! digitalRead(PPauseButton) )
		currentCAM = ((currentCAM + 1)%4);

	openCam ();
	delay(Tdelay);
	
}