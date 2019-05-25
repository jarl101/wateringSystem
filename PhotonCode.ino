unsigned long lastTime = 0;

int soilMoisture;

int moisture_pin = A1;
int power_pin = D7;
int relayPin = D1;

int moistureMin = 80;
int waterTime = 0;
int waitTime = 10;

//For position in EEPROM
int addrWater = 0;
int addrWait = 10;
int addrMoisture = 20;

void setup() {
	Serial.begin(9600);
	pinMode(moisture_pin,INPUT);
	pinMode(power_pin,OUTPUT);
	pinMode(relayPin,OUTPUT);
	
	//Load from EEPROM 
	EEPROM.get(addrWater, waterTime);
	EEPROM.get(addrWait, waitTime);
	EEPROM.get(addrMoisture, moistureMin);
}

void loop() {
	//Serial.println(readSoil());
	readSoil();
	Particle.function("waterTime", waterDelay);
	Particle.function("waitTime", waitDelay);
	Particle.function("moistureThreshold", moisturePoint);
	
}
//
// Reads the soil moisture sensor every "waitTime"
//
int readSoil() {
    
    unsigned long now = millis();
    
    if ((now - lastTime) >= (waitTime * 60000)) { //Change to 60000
        lastTime = now;
        
        digitalWrite(power_pin, HIGH);
	
    	int moisture_analog = analogRead(moisture_pin); // read capacitive sensor
        int moisture_percentage = (((moisture_analog/3060.00) * 100 ));
        delay(100);
        digitalWrite(power_pin, LOW);
        delay(100);
        
        //Webhook
        String moisture_percentageString = String(moisture_percentage); 
        Particle.publish("soilMoisture", moisture_percentageString, PRIVATE);
    	
    	//If below "moistureMin" start watering
    	if (moisture_percentage < moistureMin) {
        	
        	digitalWrite(relayPin, HIGH);
        	delay(waterTime * 1000);
        	digitalWrite(relayPin, LOW);
    	}
    	
    	return moisture_percentage;
    }
}
//
// Change how long the plant is watered from the web
//
int waterDelay(String command) {
    int waterTime2 = command.toInt();
    
    if (0 < waterTime2 && waterTime2 <= 99999) {
        waterTime = waterTime2;
    }
    
    //Save to EEPROM
    EEPROM.put(addrWater, waterTime);
    
    return 1;
}
//
// Change time between readings from the moisture sensor
//
int waitDelay(String command) {
    int waitTime2 = command.toInt();
    
    if (0 < waitTime2 && waitTime2 <= 99999) {
        waitTime = waitTime2;
    }
    
    //Save to EEPROM
    EEPROM.put(addrWait, waitTime);
    
    return 1;
}
//
// Change the threshold for when the watering starts
//
int moisturePoint(String command) {
    int moistureMin2 = command.toInt();
    
    if (0 < moistureMin2 && moistureMin2 <= 100) {
        moistureMin = moistureMin2;
    }
    
    //Save to EEPROM
    EEPROM.put(addrMoisture, moistureMin);
    
    //Debug
    return 1;
}