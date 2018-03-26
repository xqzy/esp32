
#include <ESP8266WiFi.h>




const char* ssid = "stargate";
const char* password = "4110656003373428";

int pinLed = D5; // pin on ESP that controls LED
int pinHall = A0; // pin on ESP that is input of hall sensor
int statRunning = 0; // status: is hamster currently running? (0-no 1-yes)
int statDistDay = 0; // statistic of distance ran today (meters)
int statDistWk = 0; //statistic of distance ran this week (meters)
int statLastRun = 0; // statistic moment of last measured run TODO take right datetype
int statLastRunDuration = 0; // statistic duration of last measured run TODO take right datetype

int value = 0;
float volts = 0.0;
float tempc = 0.0;

WiFiServer server(80);

void setup()
{
	Serial.begin(115200);
	delay(10);


	pinMode(pinLed, OUTPUT);
	pinMode(pinHall, INPUT);
	digitalWrite(pinLed, LOW);

	// Connect to WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

	// Start the server
	server.begin();
	Serial.println("Server started");

	// Print the IP address
	Serial.print("Use this URL : ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");
}

void loop()
{
	int readHall;
	// Serial.print("temperature= ");
	// Serial.println(tempc);

	// Check if a client has connected
	WiFiClient client = server.available();
	if (!client) {
	
		readHall = analogRead(pinHall);
		Serial.print("Hall value: ");
		Serial.print(readHall);
		Serial.println();
		delay(250);
		return;
	}

	// Wait until the client sends some data
	Serial.println("new client");
	while (!client.available()) {
		delay(1);
	}

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();

	// Match the request

	int value = LOW;
	if (request.indexOf("/LED=ON") != -1) {
		digitalWrite(pinLed, HIGH);
		value = HIGH;
	}
	if (request.indexOf("/LED=OFF") != -1) {
		digitalWrite(pinLed, LOW);
		value = LOW;
	}

	

	returnWebPage(client);


	

}

void returnWebPage(WiFiClient client) {
	
	// Return the response
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println(""); //  do not forget this one
	client.println("<!DOCTYPE HTML>");
	client.println("<html>");
	client.println("<H1> Roxy </H1>");
	client.println("<H2> Activiteit </H2>");
	client.println("Hoe actief is Roxy ? <br> ");
	if (statRunning == 1) {
		client.println(" Roxy is nu lekker aan het rennen <br> ");
	}
	else {
		client.println("Roxy doet nu even rustig aan.... <br> ");
	}
	client.println("De laatste sessie duurde ");
	client.println(statLastRunDuration);
	client.println(" minuten. <br> ");
	client.println("De laatste sessie is ");
	client.println(statLastRun);
	client.println(" minuten geleden. <br>");

	client.println("Het is lekker warm bij Roxy..<br> ");

	value = analogRead(A0);
	volts = (value / 1024.0)*5.0;
	tempc = volts*100.0*0.66;
	// voltage range sensor = 0-5 but esp has 0-3.3V..

	client.println("<H2> Temperatuur </H2>");
	client.println("Het is ");
	client.println(tempc);
	client.println(" graden.");
	client.println("<br><br>");

	client.println("<H2> Schakelaar </H2>");
	client.print("Led pin is now: ");

	if (value == HIGH) {
		client.print("On");
	}
	else {
		client.print("Off");
	}
	client.println("<br><br>");
	client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
	client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");
	client.println("</html>");

	delay(1);
	Serial.println("Client disconnected");
	Serial.println("");

}