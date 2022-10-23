#ifdef ESP8266
#include <ESP8266WiFi.h> // ESP8266 WiFi support.  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#else
#include <WiFi.h>		// Wi-Fi support.  https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h or https://www.arduino.cc/en/Reference/WiFi
#endif


char ipAddress[16];										// A character array to hold the IP address.
char macAddress[18];										// A character array to hold the MAC address, and append a dash and 3 numbers.
long rssi;													// A global to hold the Received Signal Strength Indicator.
unsigned int printInterval = 10000;					// How long to wait between MQTT publishes.
unsigned long printCount = 0;							// A counter of how many times the stats have been published.
unsigned long lastPrintTime = 0;						// The last time a MQTT publish was performed.
unsigned long wifiConnectionTimeout = 10000;		// The amount of time to wait for a Wi-Fi connection.
const unsigned int MCU_LED = 2;						// The GPIO which the onboard LED is connected to.
const char *wifiSsid = "nunya";						// Wi-Fi SSID.
const char *wifiPassword = "nunya";					// Wi-Fi password.
const char *hostname = "GenericESP";				// The hostname.


/**
 * @brief lookupWifiCode() will return the string for an integer code.
 */
void lookupWifiCode( int code, char * buffer)
{
	switch( code )
	{
		case 0:
			snprintf( buffer, 26, "%s", "Idle" );
			break;
		case 1:
			snprintf( buffer, 26, "%s", "No SSID" );
			break;
		case 2:
			snprintf( buffer, 26, "%s", "Scan completed" );
			break;
		case 3:
			snprintf( buffer, 26, "%s", "Connected" );
			break;
		case 4:
			snprintf( buffer, 26, "%s", "Connection failed" );
			break;
		case 5:
			snprintf( buffer, 26, "%s", "Connection lost" );
			break;
		case 6:
			snprintf( buffer, 26, "%s", "Disconnected" );
			break;
		default:
			snprintf( buffer, 26, "%s", "Unknown Wi-Fi status code" );
	}
} // End of lookupWifiCode() function.


/**
 * @brief wifiBasicConnect() will connect to a SSID.
 */
void wifiBasicConnect()
{
 	// Turn the LED off to show Wi-Fi is not connected.
	digitalWrite( MCU_LED, LOW );

	Serial.printf( "Attempting to connect to Wi-Fi SSID '%s'", wifiSsid );
	WiFi.mode( WIFI_STA );
	WiFi.config( INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE );
	WiFi.setHostname( hostname );
	WiFi.begin( wifiSsid, wifiPassword );

	unsigned long wifiConnectionStartTime = millis();

	// Loop until connected, or until wifiConnectionTimeout.
	while( WiFi.status() != WL_CONNECTED && ( millis() - wifiConnectionStartTime < wifiConnectionTimeout ) )
	{
		Serial.print( "." );
		delay( 1000 );
	}
	Serial.println( "" );

	if( WiFi.status() == WL_CONNECTED )
	{
		// Print that Wi-Fi has connected.
		Serial.println( "\nWi-Fi connection established!" );
		snprintf( ipAddress, 16, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
   	// Turn the LED on to show that Wi-Fi is connected.
		digitalWrite( MCU_LED, HIGH );
		return;
	}
  else
    Serial.println( "Wi-Fi failed to connect in the timeout period.\n" );
} // End of wifiBasicConnect() function.


/**
 * @brief readTelemetry() will read the telemetry to global variables.
 */
void readTelemetry()
{
	rssi = WiFi.RSSI();
} // End of readTelemetry() function.


/**
 * @brief printTelemetry() will print the telemetry to the serial port.
 */
void printTelemetry()
{
	printCount++;
	Serial.printf( "Publish count %ld\n", printCount );
	Serial.printf( "MAC address: %s\n", macAddress );
	int wifiStatusCode = WiFi.status();
	char buffer[26];
	lookupWifiCode( wifiStatusCode, buffer );
	Serial.printf( "Wi-Fi status text: %s\n", buffer );
	Serial.printf( "Wi-Fi status code: %d\n", wifiStatusCode );
	if( wifiStatusCode == 3 )
	{
		Serial.printf( "IP address: %s\n", ipAddress );
		Serial.printf( "RSSI: %ld\n", rssi );
	}
} // End of printTelemetry() function.


/**
 * @brief setup() will configure the program.
 */
void setup()
{
	// Start Serial communications.
	Serial.begin( 115200 );
	if( !Serial )
		delay( 500 );
	Serial.println( "\n\nsetup() is beginning." );

	// Set GPIO 2 (MCU_LED) as an output.
	pinMode( MCU_LED, OUTPUT );
	// Turn the LED on.
	digitalWrite( MCU_LED, HIGH );

	// Set the MAC address variable to its value.
	snprintf( macAddress, 18, "%s", WiFi.macAddress().c_str() );
} // End of setup() function.


/**
 * @brief loop() repeats over and over.
 */
void loop()
{
	if( WiFi.status() != WL_CONNECTED )
	{
		wifiBasicConnect();
		// initWiFi();
	}

	long time = millis();
	// Print the first time.  Avoid subtraction overflow.  Print every interval.
	if( lastPrintTime == 0 || ( time > printInterval && ( time - printInterval ) > lastPrintTime ) )
	{
		readTelemetry();
		printTelemetry();
		lastPrintTime = millis();

		Serial.printf( "Next print in %u seconds.\n\n", printInterval / 1000 );
	}
} // End of loop() function.
