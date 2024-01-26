

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>


#include <ESP32_Servo.h>

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int servoPin = 23;


#define in1 27
#define in2 26
#define in3 33
#define in4 25


const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
int distanceCm;
int distanceInch;

int servo_0 = 0;
int servo_90 = 0;
int servo_180 = 0;
int Left = 0;
int Right = 0;
int Stop = 0;
int forward = 0;
int backwward = 0;


// WiFi settings
#define WIFI_MODE           1                     // 1 = AP mode, 2 = STA mode
#define SSID_AP             "Plant Leaf Detection"    // for AP mode
#define PASSWORD_AP         "12345678"            // for AP mode
#define SSID_STA            "Wifi Router Name"      // for STA mode
#define PASSWORD_STA        "Password"  // for STA mode

IPAddress local_ip(192, 168, 1, 1); //IP for AP mode
IPAddress gateway(192, 168, 1, 1); //IP for AP mode
IPAddress subnet(255, 255, 255, 0); //IP for AP mode
WebServer server(80);
int robot_mode = 0; // Robot Drive Mode initially 0 (Stop)

// initialize
void setup() {
  Serial.begin(115200);
  Serial.println("Plant Lead Detection");
  Wire.begin();
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  myservo.attach(servoPin);


  robot_control(); // Stop Robot


  if (WIFI_MODE == 1) { // AP mode
    WiFi.softAP(SSID_AP, PASSWORD_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
  } else { // STA mode
    WiFi.begin(SSID_STA, PASSWORD_STA);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP()); //the IP is needed for connection in STA mode
  }

  delay(2000);

  // setup web server to handle specific HTTP requests
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/servo_0", HTTP_GET, handle_servo_0);
  server.on("/servo_90", HTTP_GET, handle_servo_90);
  server.on("/servo_180", HTTP_GET, handle_servo_180);

  server.on("/forward", HTTP_GET, handle_forward);
  server.on("/backward", HTTP_GET, handle_backward);
  server.on("/left", HTTP_GET, handle_left);
  server.on("/right", HTTP_GET, handle_right);
  server.on("/stop", HTTP_GET, handle_stop);
  server.onNotFound(handle_NotFound);

  //start server
  server.begin();
  Serial.println("ROBOT WEBSERVER STARTED");
}

// handle HTTP requests and control car
void loop() {
  server.handleClient();
  robot_control();
  ultra();
}

// HTTP request: on connect
void handle_OnConnect() {
  robot_mode = 0;
  Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: stop car
void handle_stop() {
  robot_mode = 0;
  Serial.println("Stopped");
  server.send(200, "text/html", SendHTML());
}


// HTTP request: servo_0
void handle_servo_0() {
  robot_mode = 1;
  Serial.println("servo moves to 0");
  server.send(200, "text/html", SendHTML());
}


// HTTP request: servo_90
void handle_servo_90() {
  robot_mode = 2;
  Serial.println("Servo moves to 90");
  server.send(200, "text/html", SendHTML());
}


// HTTP request: vacume on
void handle_servo_180() {
  robot_mode = 3;
  Serial.println("Servo moves to 180");
  server.send(200, "text/html", SendHTML());
}
// HTTP request: turn forward
void handle_forward() {
  robot_mode = 4;
  Serial.println("Move Forward...");
  server.send(200, "text/html", SendHTML());
}


// HTTP request: turn Backward
void handle_backward() {
  robot_mode = 5;
  Serial.println("Move Backward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn left
void handle_left() {
  robot_mode = 6;
  Serial.println("Move left...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn right
void handle_right() {
  robot_mode = 7;
  Serial.println("Move right...");
  server.send(200, "text/html", SendHTML());
}



// HTTP request: other
void handle_NotFound() {
  robot_mode = 9;
  Serial.println("Page error");
  server.send(404, "text/plain", "Not found");
}

// control car movement
void robot_control() {


  if (robot_mode == 4 && distanceCm < 30 )
  {
    Serial.println("RobotStop, obstacle detected");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    robot_mode = 0;
  }


  switch (robot_mode) {
      Serial.print("robot mode:"); Serial.println(robot_mode);
    case 0: // stop robot
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      break;
    case 1: // servo_0
      for (pos <= 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      break;
    case 2: //servo 90
      if (pos < 10)
      {
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
      }

      if (pos > 170)
      {
        for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
      }
      break;

    case 3: // servo 180
      for (pos <= 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);
      }
      break;
    case 4:
      if (distanceCm > 10 )
      {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
      }
      break;
    case 5: // Robot move backward
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      break;
    case 6: // Robot move left
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      break;
    case 7: // Robot move right
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      break;
  }
}

// output HTML web page for user
String SendHTML() {
  String html = "<!DOCTYPE html>\n";
  html += "<html>\n";
  html += "<head>\n";
  html += "<title>Robot</title>\n";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<div align=\"center\">\n";
  html += "<h1 style=\"text-align: center;\"><u><strong>PLANT LEAF DETECTION ROBOT</strong> </u></h1>\n";
  html += "<h3 style=\"text-align: center;\">Developed by:</h3>";
  html += "<h3 style=\"text-align: center;\">K18ES19,K18ES23,K18ES41,KI8ES11</h3>";
  html += "<br>\n";
  html += "<form method=\"GET\">\n";
  html += "<input type=\"button\" value=\"Servo 0 Degree\" onclick=\"window.location.href='/servo_0'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Servo 90 Degree\" onclick=\"window.location.href='/servo_90'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Servo 180 Degree\" onclick=\"window.location.href='/servo_180'\">\n";
  html += "<br><br>\n";


  html += "<input type=\"button\" value=\"MOVE FORWARD\" onclick=\"window.location.href='/forward'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"MOVE BACKWARD\" onclick=\"window.location.href='/backward'\">\n";
  html += "<br><br>\n";

  html += "<input type=\"button\" value=\"MOVE LEFT\" onclick=\"window.location.href='/left'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"MOVE RIGHT\" onclick=\"window.location.href='/right'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"STOP\" onclick=\"window.location.href='/stop'\">\n";
  html += "</form>\n";
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";
  return html;
}

void ultra()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  delay(500);
}
