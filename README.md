Moto Mingle - ESP8266 Chat Application
Overview
Moto Mingle (Campus Chatmate) is a simple chat application running on an ESP8266 microcontroller. It sets up a local Wi-Fi access point and allows users to chat with each other using WebSockets. The application displays real-time user counts and allows anonymous and custom usernames for participants.

Features
WebSocket Communication: Real-time, two-way communication between the client and the ESP8266 server.
Custom Usernames: Users can set a custom username or join as anonymous.
Real-time Chat: Messages are sent instantly to all connected clients.
Access Point Mode: The ESP8266 acts as a Wi-Fi access point for users to connect.
Responsive UI: The interface adapts to different screen sizes with mobile support.
Setup
Requirements
ESP8266 microcontroller.
Arduino IDE installed with the ESP8266 board package.
Libraries:
ESP8266WiFi.h
DNSServer.h
ESP8266WebServer.h
WebSocketsServer.h
Installation
Open the Arduino IDE.
Go to File > Preferences and add the following URL to the "Additional Boards Manager URLs":
bash
Copy code
http://arduino.esp8266.com/stable/package_esp8266com_index.json
Go to Tools > Board > Boards Manager and install the ESP8266 package.
Install the necessary libraries from the Library Manager:
ESP8266WebServer
WebSocketsServer
DNSServer
Hardware Setup
ESP8266 Board: Ensure you are using a board like the NodeMCU or Wemos D1 mini.
LED Pin: Built-in LED (GPIO2) is used to indicate activity.
How to Use
1. Upload the Code
Connect your ESP8266 to the computer via USB.
In the Arduino IDE, select your ESP8266 board and port:
Tools > Board > NodeMCU 1.0 (ESP-12E Module)
Tools > Port > Select the correct port
Copy the code provided into the Arduino IDE and click on Upload.
2. Access the Chat
Once the code is uploaded and the ESP8266 starts, it will create a Wi-Fi access point named "Campus Chatmate Server".
Connect to the Wi-Fi network with any Wi-Fi-enabled device (e.g., laptop, smartphone).
Open a web browser and go to the IP address 172.217.28.1. This will open the chat interface.
3. Set Your Username
Enter a nickname in the text field at the top and press the Set Username button. If the nickname is available, you can start chatting.
If the username is already taken, the system will notify you.
4. Send Messages
Type a message in the text field and press the Send button to broadcast it to all users.
Code Overview
This project utilizes ESP8266 for setting up the access point and serving a real-time chat app through WebSockets.

WebServer serves an HTML page with the chat UI.
WebSocket handles the real-time message exchange.
DNSServer is used to redirect all DNS requests to the ESP8266 IP address when users connect.
Main Functions
setup()
Initializes the serial communication and sets up the access point (Wi-Fi.softAP).
Configures the DNS server and starts both the web and WebSocket servers.
loop()
Continuously processes requests from the DNS server, WebSocket connections, and the web server.
webSocketEvent()
Handles incoming WebSocket messages, including username assignments, message sending, and client count updates.
Code Walkthrough
Setting up Wi-Fi Access Point: The ESP8266 sets up a Wi-Fi access point with the name "Campus Chatmate Server" and an IP address 172.217.28.1.

WebSocket Communication:

The WebSocket listens on port 81, allowing multiple clients to send and receive messages in real-time.
The server broadcasts the number of connected users to all clients.
HTML and CSS:

The HTML structure creates an interactive chat interface with a user input field for the nickname and message.
CSS styles make the UI responsive and visually appealing with a light theme and scrollable message area.
JavaScript:

Handles WebSocket connections and dynamic updates of the chat interface.
Sends the messages and updates the username dynamically.
Handles escaping HTML to prevent XSS attacks.
Contributors
Prince Mercado: Developer, Project Lead
License
This project is licensed under the MIT License. See the LICENSE file for details.

Future Improvements
User Authentication: Adding user login features for more secure access.
Message History: Storing and displaying message history for users who reconnect.
Acknowledgements
ESP8266 Community for the development of the libraries and tools used in this project.
Feel free to adapt the documentation to your project structure and add more specific details as needed. 😊
