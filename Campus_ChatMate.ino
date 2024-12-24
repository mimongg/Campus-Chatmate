#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <vector>
#include <algorithm> // For std::remove

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
WebSocketsServer webSocket(81); // Using port 81 for WebSocket server

const int ledPin = LED_BUILTIN; // Built-in LED on ESP8266 (GPIO2)
int lastClientCount = 0; // Track the number of connected clients
int connectedClients = 0; // Number of connected WebSocket clients
int anonymousCounter = 0;  // Counter for "Anonymous" nicknames


std::vector<String> activeUsernames; // List of active usernames
std::vector<String> clientUsernames; // List of usernames by client index

String responseHTML =
"<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"  <title>Moto Mingle</title>"
"  <style>"
"body {"
"  font-family: Arial, sans-serif;"
"  background-color: #e9ecef;"
"  color: #333;"
"  margin: 0;"
"  padding: 0;"
"}"
""
".container {"
"  border: 2px solid black;"
"  max-width: 600px;"
"  margin: 50px auto;"
"  padding: 20px;"
"  background: #fff;"
"  border-radius: 8px;"
"  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);"
"  background-color: whitesmoke;"
"}"
""
"h1 {"
"  color: black;"
"  font-size: 2em;"
"  margin-bottom: 0%;"
"  display: inline-block;"
"}"
""
".header-wrapper {"
"  display: flex;"
"  align-items: center;"
"  margin-bottom: 2%;"
"  flex-wrap: wrap;"
"  border: 2px solid black; /* Added border */"
"  padding: 10px;"
"  border-radius: 5px;"
"}"
""
".header-input {"
"  border: 2px solid black; /* Changed border color to black */"
"  flex: 1;"
"  padding: 10px;"
"  border-radius: 5px;"
"  font-size: 0.8em;"
"  margin-top: 3%;"
"  margin-right: 5px;"
"  margin-left: 50px;"
"  min-width: 150px;"
"  background-color: ghostwhite;"
"}"
""
".header-button {"
"  border: 2px solid black; /* Changed border color to black */"
"  width: 50px;"
"  margin-top: 3%;"
"  padding: 10px;"
"  border-radius: 5px;"
"  background-color: ghostwhite;"
"  color: black;"
"  font-weight: bolder;"
"  cursor: pointer;"
"  font-size: 0.8em;"
"}"
""
".header-button:hover {"
"  border: 2px solid black; /* Changed border color to black */"
"  background-color: #ADD8E6;"
"  color: black;"
"}"
""
".chat-box {"
"  border: 2px solid black; /* Changed border color to black */"
"  height: 250px;"
"  overflow-y: scroll;"
"  padding: 10px;"
"  background-color: whitesmoke;"
"  border-radius: 5px;"
"  margin-bottom: 10px;"
"  margin-top: 10px;"
"}"
""
"/* Custom Scrollbar */"
".chat-box::-webkit-scrollbar {"
"  width: 8px; /* Small width */"
"}"
""
".chat-box::-webkit-scrollbar-track {"
"  background: transparent; /* Dark background for the track */"
"  border-radius: 2.5px; /* Rounded corners */"
"}"
""
".chat-box::-webkit-scrollbar-thumb {"
"  background: #333; /* Black thumb */"
"  border: 2px solid black;"
"  border-radius: 10px; /* Rounded corners */"
"}"
""
".chat-box::-webkit-scrollbar-thumb:hover {"
"  background: #444; /* Slightly lighter on hover */"
"}"
""
".chat-message-sent {"
"  border: 2px solid black;"
"  border-radius: 7.5px;"
"  display: inline-block;"
"  word-wrap: break-word;"
"  word-break: break-word;"
"  overflow-wrap: break-word;"
"  margin-bottom: 3px;"
"  padding: 5px;"
"  max-width: 80%;"
"  background-color: #ADD8E6; /* Light blue background for sent messages */"
"  float: right;" /* Aligns the message to the right */
"  text-align: left;"
"  clear: both;" /* Ensures messages don't overlap */
"}"
""
".chat-message-received {"
"  border: 2px solid black;"
"  border-radius: 7.5px;"
"  display: inline-block;"
"  word-wrap: break-word;"
"  word-break: break-word;"
"  overflow-wrap: break-word;"
"  margin-bottom: 3px;"
"  padding: 5px;"
"  max-width: 80%;"
"  background-color: #FFB6C1; /* Light pink background for received messages */"
"  float: left;" /* Aligns the message to the left */
"  text-align: left;"
"  clear: both;" /* Ensures messages don't overlap */
"}"
""
".chat-message-container {"
"  display: block;" /* Container spans the full width of its parent */
"  max-width: 100%;"
"  margin-bottom: 3px;"
"  overflow: hidden;" /* Clears floats within the container */
"}"
""
".form-group {"
"  display: flex;"
"  margin-bottom: 10px;"
"}"
""
".form-group input[type='text'] {"
"  border: 2px solid black; /* Changed border color to black */"
"  flex: 1;"
"  padding: 10px;"
"  border-radius: 5px;"
"  font-size: 1em;"
"  margin-right: 5px;"
"  background-color: ghostwhite;"
"}"
""
".form-group button {"
"  border: 2px solid black;"
"  width: 60px;"
"  padding: 10px;"
"  border-radius: 5px;"
"  background-color: ghostwhite;"
"  color: black;"
"  font-weight: bolder;"
"  cursor: pointer;"
"  font-size: 1em;"
"}"
""
".form-group button:hover {"
"  border: 2px solid black;"
"  background-color: #ADD8E6;"
"  color: black;"
"}"
""
".disclaimer-wrapper {"
"  border: 2px solid black; /* Set border color and thickness */"
"  border-radius: 5px; /* Round the corners */"
"  background-color: whitesmoke; /* Set background color */"
"  margin-top: 10px; /* Adjust top margin */"
"}"
""
".disclaimer {"
"  font-size: 0.55em;"
"  color: black;"
"}"
""
"@media (max-width: 600px) {"
"  .header-wrapper {"
"    border: 2px solid black; /* Added border */"
"    display: flex;"
"    align-items: center;"
"    margin-bottom: 10px;"
"    flex-wrap: wrap;"
"    padding: 10px;"
"    border-radius: 5px;"
"  }"
""
"  .container {"
"    max-width: 80vw;"
"}"
""
"  .header-input {"
"    border: 2px solid black; /* Changed border color to black */"
"    flex: 1;"
"    padding: 10px;"
"    border-radius: 5px;"
"    font-size: 0.8em;"
"    margin-top: 5px;"
"    margin-bottom: 0px;"
"    margin-right: 5px;"
"    margin-left: 0px;"
"    background-color: ghostwhite;"
"  }"
""
".disclaimer {"
"  font-size: 0.45em;"
"  color: black;"
"}"
""
"  .header-button {"
"    border: 2px solid black; /* Changed border color to black */"
"    flex: 0.1;"
"    width: 50px;"
"    padding: 10px;"
"    margin-top: 5px;"
"    border-radius: 5px;"
"    background-color: ghostwhite;"
"    color: black;"
"    font-weight: bolder;"
"    font-size: 0.8em;"
"  }"
"}"
"  </style>"
"</head>"
"<body>"
"  <div class=\"container\">"
"    <div class=\"header-wrapper\">"
"      <h1>Campus Chatmate</h1>"
"      <input id=\"username\" type=\"text\" placeholder=\"Enter nickname...\" class=\"header-input\">"
"      <button class=\"header-button\" onclick=\"setUsername()\"><img src=\"data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzMjAgNTEyIj48IS0tIUZvbnQgQXdlc29tZSBGcmVlIDYuNi4wIGJ5IEBmb250YXdlc29tZSAtIGh0dHBzOi8vZm9udGF3ZXNvbWUuY29tIExpY2Vuc2UgLSBodHRwczovL2ZvbnRhd2Vzb21lLmNvbS9saWNlbnNlL2ZyZWUgQ29weXJpZ2h0IDIwMjQgRm9udGljb25zLCBJbmMuLS0+PHBhdGggZD0iTTI3NS4xIDMzNGMtMjcuNCAxNy40LTY1LjEgMjQuMy05MCAyNi45bDIwLjkgMjAuNiA3Ni4zIDc2LjNjMjcuOSAyOC42LTE3LjUgNzMuMy00NS43IDQ1LjctMTkuMS0xOS40LTQ3LjEtNDcuNC03Ni4zLTc2LjZMODQgNTAzLjRjLTI4LjIgMjcuNS03My42LTE3LjYtNDUuNC00NS43IDE5LjQtMTkuNCA0Ny4xLTQ3LjQgNzYuMy03Ni4zbDIwLjYtMjAuNmMtMjQuNi0yLjYtNjIuOS05LjEtOTAuNi0yNi45LTMyLjYtMjEtNDYuOS0zMy4zLTM0LjMtNTkgNy40LTE0LjYgMjcuNy0yNi45IDU0LjYtNS43IDAgMCAzNi4zIDI4LjkgOTQuOSAyOC45czk0LjktMjguOSA5NC45LTI4LjljMjYuOS0yMS4xIDQ3LjEtOC45IDU0LjYgNS43IDEyLjQgMjUuNy0xLjkgMzgtMzQuNSA1OS4xek0zMC4zIDEyOS43QzMwLjMgNTggODguNiAwIDE2MCAwczEyOS43IDU4IDEyOS43IDEyOS43YzAgNzEuNC01OC4zIDEyOS40LTEyOS43IDEyOS40cy0xMjkuNy01OC0xMjkuNy0xMjkuNHptNjYgMGMwIDM1LjEgMjguNiA2My43IDYzLjcgNjMuN3M2My43LTI4LjYgNjMuNy02My43YzAtMzUuNC0yOC42LTY0LTYzLjctNjRzLTYzLjcgMjguNi02My43IDY0eiIvPjwvc3ZnPg==\" alt=\"SVG Image\" style=\"width: 17px; height: 17px; margin-top: -5px; margin-bottom: -4px; margin-right: -5px; margin-left: -5px;\"></button>"
"    </div>"
"    <div id=\"chat-box\" class=\"chat-box\"></div>"
"    <div id=\"user-count\">Users Online: 0</div>"
"    <div class=\"form-group\">"
"      <input id=\"message\" type=\"text\" placeholder=\"Type your message...\">"
"      <button onclick=\"sendMessage()\"><img src=\"data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA1MTIgNTEyIj48IS0tIUZvbnQgQXdlc29tZSBGcmVlIDYuNi4wIGJ5IEBmb250YXdlc29tZSAtIGh0dHBzOi8vZm9udGF3ZXNvbWUuY29tIExpY2Vuc2UgLSBodHRwczovL2ZvbnRhd2Vzb21lLmNvbS9saWNlbnNlL2ZyZWUgQ29weXJpZ2h0IDIwMjQgRm9udGljb25zLCBJbmMuLS0+PHBhdGggZD0iTTQ5OC4xIDUuNmMxMC4xIDcgMTUuNCAxOS4xIDEzLjUgMzEuMmwtNjQgNDE2Yy0xLjUgOS43LTcuNCAxOC4yLTE2IDIzcy0xOC45IDUuNC0yOCAxLjZMMjg0IDQyNy43bC02OC41IDc0LjFjLTguOSA5LjctMjIuOSAxMi45LTM1LjIgOC4xUzE2MCA0OTMuMiAxNjAgNDgwbDAtODMuNmMwLTQgMS41LTcuOCA0LjItMTAuOEwzMzEuOCAyMDIuOGM1LjgtNi4zIDUuNi0xNi0uNC0yMnMtMTUuNy02LjQtMjItLjdMMTA2IDM2MC44IDE3LjcgMzE2LjZDNy4xIDMxMS4zIC4zIDMwMC43IDAgMjg4LjlzNS45LTIyLjggMTYuMS0yOC43bDQ0OC0yNTZjMTAuNy02LjEgMjMuOS01LjUgMzQgMS40eiIvPjwvc3ZnPg==\" alt=\"SVG Image\" style=\"width: 15px; height: 15px; margin-top: 1px; margin-bottom: -2px; margin-right: -5px; margin-left: -5px;\"></button>"
"    </div>"
"    <div class=\"disclaimer-wrapper\"> <!-- Disclaimer container -->"
"        <center>"
"      <p class=\"disclaimer\">"
"        <b>Want to support us? (I mean me)</b><br>Donations help keep the chatroom running! Send support to <b>0928-411-5727.</b><br><br>"
"         This chatroom is monitored. Please keep the conversation respectful and clean!<br><br>"
"        <b>Copyright 2024 Mercado. All rights reserved.</b>"
"      </p>"
"        </center>"
"    </div>"
"    <script>"
"      var ws = new WebSocket('ws://' + window.location.hostname + ':81');"
"      var Username;"
"      var newUsername;"
"ws.onmessage = function(event) {"
"  var chatBox = document.getElementById('chat-box');"
"  var messageContainer = document.createElement('div');"
"  var message = document.createElement('div');"
"  messageContainer.className = 'chat-message-container';"
""
"if (event.data.startsWith('USERNAME_ALERT:')) {"
"    alert('Username: ' + event.data.substring(15));"
" Username = event.data.substring(15);"
"} else if (event.data.startsWith('Users Online:')) {"
"    document.getElementById('user-count').innerText = event.data;"
"} else if (event.data.startsWith('USERNAME_TAKEN: ')) {"
" alert('Nickname already in use.');"
"} else if (event.data.startsWith('You are: ')) {"
" Username = event.data.substring(9);"
" console.log('Your nickname is: ' + Username);"  
"} else {"
"    var [user, msg] = event.data.split(': ', 2);"
"    if (!msg) return;"
""
"    message.className = user === Username ? 'chat-message-sent' : 'chat-message-received';"
"    message.innerHTML = '<span>' + escapeHTML(user) + ':</span> ' + escapeHTML(msg);"
""
"    messageContainer.appendChild(message);"
"    chatBox.appendChild(messageContainer);"
"    chatBox.scrollTop = chatBox.scrollHeight;"
"  }"
"};"
"      function sendMessage() {"
"        var messageInput = document.getElementById('message');"
"        var message = messageInput.value;"
"        if (message) {"
"          ws.send(username + ': ' + message);"
"          messageInput.value = '';"
"        }"
"      }"
"      function setUsername() {"
"        var usernameInput = document.getElementById('username');"
"        newUsername = usernameInput.value.trim();"
"        if (newUsername) {"
"          ws.send('SET_USERNAME: ' + newUsername);"
"          usernameInput.value = '';"
"        }"
"      }"
"      function escapeHTML(unsafe) {"
"        return unsafe"
"          .replace(/&/g, '&amp;')"
"          .replace(/</g, '&lt;')"
"          .replace(/>/g, '&gt;')"
"          .replace(/\"/g, '&quot;')"
"          .replace(/'/g, '&#039;');"
"      }"
"    </script>"
"  </div>"
"</body>"
"</html>";

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn off LED (LED_BUILTIN is active LOW)

  // Set up the Access Point (AP) mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Campus Chatmate Server");

  // Start the DNS server
  dnsServer.start(DNS_PORT, "*", apIP);

  // Serve the HTML page
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.begin();
  
  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("Access Point started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  webSocket.loop();

  // Broadcast the number of connected clients if it has changed
  if (connectedClients != lastClientCount) {
    blinkLED();
    String countMessage = "Users Online: " + String(connectedClients);
    webSocket.broadcastTXT(countMessage);
    lastClientCount = connectedClients;
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  String message;
  String welcomeMessage;
  String nickname;
  String takenNickname;

  switch(type) {
  case WStype_TEXT:
  message = String((char*)payload);
  Serial.printf("Received message: %s from client %u\n", message.c_str(), num);

  if (message.startsWith("SET_USERNAME: ")) {
    String requestedUsername = message.substring(14);
    requestedUsername = requestedUsername.charAt(0) + requestedUsername.substring(1); // Capitalize first letter

    if (isUsernameTaken(requestedUsername)) {
      takenNickname = "USERNAME_TAKEN: " + requestedUsername;
      webSocket.sendTXT(num, takenNickname);
    } else {
      if (num >= clientUsernames.size()) {
        clientUsernames.resize(num + 1, "Anonymous"); // Initialize new client with default username
      }

      // Remove previous username from activeUsernames if necessary
      if (clientUsernames[num] != "Anonymous") {
        activeUsernames.erase(std::remove(activeUsernames.begin(), activeUsernames.end(), clientUsernames[num]), activeUsernames.end());
      }
      // Set new username
      clientUsernames[num] = requestedUsername;
      activeUsernames.push_back(requestedUsername);
      String alertMessage = "USERNAME_ALERT:" + requestedUsername;
      webSocket.sendTXT(num, alertMessage);
      Serial.printf("Client %u set username to %s\n", num, requestedUsername.c_str());
    }
  } else {
    // Ensure username is set before broadcasting message
    String senderUsername = num < clientUsernames.size() ? clientUsernames[num] : "Anonymous";

    // Debug to check the message content
    Serial.printf("Broadcasting message from %s: %s\n", senderUsername.c_str(), message.c_str());

    // Split the message to avoid issues
    int delimiterIndex = message.indexOf(": ");
    if (delimiterIndex != -1) {
      String actualMessage = message.substring(delimiterIndex + 2); // Skip ": "
      String broadcastMessage = senderUsername + ": " + actualMessage;
      webSocket.broadcastTXT(broadcastMessage);
    } else {
      Serial.println("Invalid message format received.");
    }

    messageDone();
  }
  break;
case WStype_CONNECTED:
  Serial.printf("Client %u connected\n", num);
  connectedClients++;
  
  // Increment the anonymousCounter to ensure unique nicknames
  anonymousCounter++;
  nickname = "Motorian " + String(anonymousCounter);  // Create unique nickname

  if (num >= clientUsernames.size()) {
    clientUsernames.resize(num + 1); // Resize the clientUsernames array if necessary
  }

  clientUsernames[num] = nickname;  // Assign the unique nickname to the new client
  Serial.printf("%s has joined\n", nickname.c_str());  // Log the nickname

  // Send the nickname to the new client
  welcomeMessage = "You are: " + nickname;
  webSocket.sendTXT(num, welcomeMessage);  // Send the nickname to the client

  break;
    case WStype_DISCONNECTED:
      Serial.printf("Client %u disconnected\n", num);
      connectedClients--;
      // Remove the username of the disconnected client
      removeUsername(num);
      break;
  }
}

bool isUsernameTaken(const String& username) {
  // Check if the username exists in either activeUsernames or clientUsernames
  return std::find(activeUsernames.begin(), activeUsernames.end(), username) != activeUsernames.end() ||
         std::find(clientUsernames.begin(), clientUsernames.end(), username) != clientUsernames.end();
}


void removeUsername(uint8_t clientNum) {
  if (clientNum < clientUsernames.size()) {
    String username = clientUsernames[clientNum];
    if (username != "Anonymous") {
      activeUsernames.erase(std::remove(activeUsernames.begin(), activeUsernames.end(), username), activeUsernames.end());
    }
  }
}

void blinkLED() {
  for(int i=0; i < 3; i++){
    digitalWrite(ledPin, LOW);  // Turn on LED
    delay(100);                 // LED on for 100ms
    digitalWrite(ledPin, HIGH); // Turn off LED
    delay(100);                 // LED off for 100ms
  }
}

void messageDone() {
  digitalWrite(ledPin, LOW);  // Turn on LED
  delay(100);                 // LED on for 100ms
  digitalWrite(ledPin, HIGH); // Turn off LED
  delay(100);
}
