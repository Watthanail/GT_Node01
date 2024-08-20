
void setup() {
  // Initialize the serial communication
  Serial.begin(115200);
  
  // Setup for leads off detection
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
}
 
void loop() {
  // Check if leads are off
  if (digitalRead(D0) == HIGH || digitalRead(D1) == HIGH) {
    Serial.println('!');
  } else {
    // Send the value of analog input A3
    Serial.println(analogRead(A3));
  }
  
  // Wait for a bit to keep serial data from saturating
  delay(100);
}
