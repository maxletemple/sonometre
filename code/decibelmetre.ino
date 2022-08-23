#include <Wire.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const int pinAdc = 34;
const int RS = 16, E = 5, DB4 = 19, DB5 = 21, DB6 = 22, DB7 = 23, ledRed = 14, ledGreen = 12, ledBlue = 13;
const int frameRate = 1;	   // Nombre de raffraichissements de l'écran par seconde
const int historyRate = 5;	   // intervalle etre chaque enregistrement dans l'historique (en secondes)
const int seuilDangereux = 80; // Seuil de dangerosité en dB

const int sampleSize = 500;

int dbValues[5];
int currentDBValue;

LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);
int moyDB, ampl;
int maxVal, minVal;
int historyBuffer;

TaskHandle_t taskCore0;

const char *ssid = "ssid";
const char *password = "password";

int currentSound;
const int historySize = 12;
int history[historySize];

WebServer server(80);

void handleRoot()
{
	String temp = "Bonjour, vous utilisez le décibelmètre de la salle des machines.\n";
	temp += "Utilisation:\n";
	temp += "/sound : renvoie la valeur actuelle du capteur en décibels\n/history : renvoie un historique des valeurs";
	server.send(200, "text/plain; charset=UTF-8", temp);
}

void handleSound()
{
	char temp[100];
	sprintf(temp, "{\"sound\" : %d}", matchDB(ampl));
	server.send(200, "application/json", temp);
}

void handleHistory()
{
	char tempStr[10];
	String temp("{");
	temp += "\"size\" :";
	sprintf(tempStr, "%d", historySize);
	temp += tempStr;
	temp += ",\"history\" : [";
	int current = currentSound;
	for (int i = 0; i < historySize; i++)
	{
		sprintf(tempStr, "%d", history[current++]);
		temp += tempStr;
		temp += ",";
		if (current == historySize)
			current = 0;
	}
	temp[temp.length() - 1] = ']';
	temp += "}";
	server.send(200, "application/json", temp);
}

void handleNotFound()
{
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}

	server.send(404, "text/plain", message);
}

void setup()
{
	Serial.begin(115200);
	for (int i = 0; i < historySize; i++)
	{
		history[i] = 0;
	}

	pinMode(RS, OUTPUT);
	pinMode(E, OUTPUT);
	pinMode(DB4, OUTPUT);
	pinMode(DB5, OUTPUT);
	pinMode(DB6, OUTPUT);
	pinMode(DB7, OUTPUT);
	pinMode(pinAdc, PULLDOWN);
	pinMode(ledGreen, OUTPUT);
	pinMode(ledRed, OUTPUT);
	pinMode(ledBlue, OUTPUT);
	digitalWrite(ledRed, 1);
	digitalWrite(ledGreen, 1);
	digitalWrite(ledBlue, 1);
	ampl = 0;
	currentDBValue = 0;
	historyBuffer = 0;
	lcd.begin(16, 2);
	lcd.print("Bonjour");
	xTaskCreatePinnedToCore(taskCore0Code, "Task core 0", 10000, NULL, 0, &taskCore0, 0);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");
	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("esp32"))
	{
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);
	server.on("/sound", handleSound);
	server.on("/history", handleHistory);
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	int temp;
	int time;
	maxVal = 0;
	minVal = 5000;
	for (int i = 0; i < sampleSize; i++)
	{
		time = micros() + 250;
		temp = analogRead(pinAdc);
		if (temp > maxVal)
			maxVal = temp;
		else if (temp < minVal)
			minVal = temp;
		while (micros() < time)
			;
	}
	ampl = maxVal - minVal;
}

int matchDB(double vol)
{
	if (ampl < 150)
	{
		return 38;
	}
	if (ampl < 500)
	{
		return 0.062 * ampl + 39;
	}
	double temp = 0.02 * ampl + 60;
	if(temp > 100) temp = 100;
	return temp;
}

void taskCore0Code(void *pvParameters)
{
	int frameDelay = 1000 / frameRate;
	for (;;)
	{
		moyDB = matchDB(ampl);
		dbValues[currentDBValue] = moyDB;
		currentDBValue++;
		lcd.clear();
		if (moyDB == 38)
		{
			lcd.print("Trop calme");
			
		}
		else if (moyDB == 100)
		{
			lcd.print("Trop fort");
		}
		else
		{
			lcd.print(moyDB);
			lcd.print(" dB");
		}

		lcd.setCursor(0, 1);
		if (moyDB < seuilDangereux)
		{
			lcd.print("Pas dangereux");
			analogWrite(ledRed, 255);
			analogWrite(ledGreen, 155);
		}
		else
		{
			lcd.print("Dangereux");
			analogWrite(ledRed, 155);
			analogWrite(ledGreen, 255);
		}
		delay(frameDelay);

		historyBuffer++;
		if (historyBuffer >= historyRate / (frameDelay / 1000))
		{
			int tempMoy = 0;
			for (int i = 0; i < 5; i++)
			{
				tempMoy += dbValues[i];
			}
			tempMoy /= 5;
			currentDBValue = 0;
			history[currentSound] = tempMoy;
			historyBuffer = 0;
			currentSound++;
			if (currentSound == historySize)
				currentSound = 0;
		}
		server.handleClient();
	}
}