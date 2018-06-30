#include <DHT.h>;
#include <Servo.h>
#include <SoftwareSerial.h>

#define DHTPIN 8
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Servo servo;
SoftwareSerial BTserial(10, 11);

const int RAIN_SENSOR_MIN = 0;
const int RAIN_SENSOR_MAX = 1024;
const int IS_DRYING = 180;
const int NOT_DRYING = 0;

bool drying = false;
bool get = false;
int timer = 0;

void setup()
{
    dht.begin();
    servo.attach(9);
    BTserial.begin(9600);
    Serial.begin(9600);
    dry();
}

void loop()
{
    delay(10); // Each cyle length is 10 ms

    if(BTserial.available() > 0) {
        String command = BTserial.readString();
        if (command == "get") {
            get = true;
        } else if (command == "on") {
            drying = true;
            dry();
        } else if (command == "off") {
            drying = false;
            dry();
        }
    }

    if(get) {
        String info = getInfo();
        BTserial.println(info);
        get = false;
    }

    timer++;
    if (timer == 1000) { // Check every 10 seconds
        timer = 0;
        Serial.println("Checking rain drops");
        if (getRainDrops().indexOf("F") > 0) {
            Serial.println("Flooding");
            drying = false;
        } else {
            Serial.println("Not raining");
            drying = true;
        }
        dry();
    }
}

String getInfo() {
    return getHumidity() + ";" + getTemperature() + ";" + getRainDrops() + ";" + getDrying();
}

String getHumidity() {
    String res = "";

    res += "H:";
    res += dht.readHumidity();

    return res;
}

String getTemperature() {
    String res = "";

    res += "T:";
    res += dht.readTemperature();

    return res;
}

String getRainDrops() {
    String res = "";

    int sensorReading = analogRead(A0);
    int range = map(sensorReading, RAIN_SENSOR_MIN, RAIN_SENSOR_MAX, 0, 3);
    res += "R:";
    switch (range) {
        case 0:
            res += "F";
            drying = false;
            break;
        case 1:
            res += "W";
            drying = false;
            break;
        case 2:
            res += "N";
            drying = true;
            break;
    }

    return res;
}

String getDrying() {
    String res = "";

    res += "D:";
    if (drying) {
        res += "Y";
    } else {
        res += "N";
    }

    return res;
}

void dry() {
    if (drying) {
        servo.write(IS_DRYING);
        Serial.println("Start drying");
    } else {
        servo.write(NOT_DRYING);
        Serial.println("Stop drying");
    }
    delay(500);
}