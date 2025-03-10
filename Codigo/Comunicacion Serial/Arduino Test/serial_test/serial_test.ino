void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud
  delay(1000); // Allow time for the serial port to initialize
Serial.print("Hello, Serial!"); // No automatic newline

}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n'); // Read incoming data until newline
    receivedData.trim(); // Remove any extra spaces or newlines
    Serial.println("Received: " + receivedData); // Echo back the received data
  }
}
