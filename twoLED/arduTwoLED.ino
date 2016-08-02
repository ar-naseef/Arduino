#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 120); // IP address, may need to change depending on network
EthernetServer server(2200);  // create a server at port 80
File webFile;
String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default

void setup()
{
    SD.begin(4);
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(2, OUTPUT);       // LED on pin 2
    pinMode(8, OUTPUT);
    
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c; // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                    /*webFile = SD.open("four.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }*/
                    Serial.print(HTTP_req);
                    ProcessCheckbox(client,HTTP_req);
                    
                    HTTP_req = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        //Serial.println(HTTP_req);
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// switch LED and send back HTML for LED checkbox
void ProcessCheckbox(EthernetClient cl, String ht)
{
    File webFile;
    if (ht.indexOf("GET /?led1=on&led2=on") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
        digitalWrite(8,HIGH);
        digitalWrite(2,HIGH);
        webFile = SD.open("one.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            cl.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
    }
    else if (ht.indexOf("GET /?led1=on") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
        digitalWrite(2,HIGH);
        digitalWrite(8,LOW);
        webFile = SD.open("two.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            cl.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
    }
    else if (ht.indexOf("GET /?led2=on") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
        digitalWrite(8,HIGH);
        digitalWrite(2,LOW);
        webFile = SD.open("three.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            cl.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
    }
    else{
        digitalWrite(8,LOW);
        digitalWrite(2,LOW);
        webFile = SD.open("four.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            cl.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
    }
}
