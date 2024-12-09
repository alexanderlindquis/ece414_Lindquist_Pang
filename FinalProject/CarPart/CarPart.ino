// Debug Car Control Code for DRV8871 drivers

// Pin definitions for motor drivers
#define MOTOR_LEFT_IN1  2
#define MOTOR_LEFT_IN2  3
#define MOTOR_RIGHT_IN1 4
#define MOTOR_RIGHT_IN2 5

// Constants
#define MAX_SPEED 153     // ~60% of 255 for 9V power
#define MIN_ANGLE 5.0     // Minimum angle to start moving
#define MAX_ANGLE 60.0    // Angle for maximum speed
#define EMERGENCY_ANGLE 80.0

// Buffer for receiving data
char buffer[32];
int bufferIndex = 0;

// Variables to store received angles
float receivedPitch = 0.0;
float receivedRoll = 0.0;

void setup() {
    Serial.begin(115200);  // USB Serial for debugging
    Serial1.begin(9600);   // Bluetooth Serial
    
    // Initialize motor pins
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    
    stopMotors();
    Serial.println("\n=== Car Control System Starting ===");
    
    // Basic motor test
    Serial.println("\nTesting motors...");
    testMotors();
    
    Serial.println("\nReady to receive commands");
    Serial.println("Format: C,pitch,roll");
}

void testMotors() {
    Serial.println("Testing Left Motor Forward");
    analogWrite(MOTOR_LEFT_IN1, 100);
    analogWrite(MOTOR_LEFT_IN2, 0);
    delay(1000);
    stopMotors();
    delay(500);
    
    Serial.println("Testing Left Motor Reverse");
    analogWrite(MOTOR_LEFT_IN1, 0);
    analogWrite(MOTOR_LEFT_IN2, 100);
    delay(1000);
    stopMotors();
    delay(500);
    
    Serial.println("Testing Right Motor Forward");
    analogWrite(MOTOR_RIGHT_IN1, 100);
    analogWrite(MOTOR_RIGHT_IN2, 0);
    delay(1000);
    stopMotors();
    delay(500);
    
    Serial.println("Testing Right Motor Reverse");
    analogWrite(MOTOR_RIGHT_IN1, 0);
    analogWrite(MOTOR_RIGHT_IN2, 100);
    delay(1000);
    stopMotors();
    
    Serial.println("Motor test complete");
}

void stopMotors() {
    analogWrite(MOTOR_LEFT_IN1, 0);
    analogWrite(MOTOR_LEFT_IN2, 0);
    analogWrite(MOTOR_RIGHT_IN1, 0);
    analogWrite(MOTOR_RIGHT_IN2, 0);
    Serial.println("Motors stopped");
}

void setLeftMotor(int speed) {
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    Serial.print("Left Motor: ");
    if (speed >= 0) {
        analogWrite(MOTOR_LEFT_IN1, speed);
        analogWrite(MOTOR_LEFT_IN2, 0);
        Serial.print("Forward ");
    } else {
        analogWrite(MOTOR_LEFT_IN1, 0);
        analogWrite(MOTOR_LEFT_IN2, -speed);
        Serial.print("Reverse ");
    }
    Serial.println(abs(speed));
}

void setRightMotor(int speed) {
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    Serial.print("Right Motor: ");
    if (speed >= 0) {
        analogWrite(MOTOR_RIGHT_IN1, speed);
        analogWrite(MOTOR_RIGHT_IN2, 0);
        Serial.print("Forward ");
    } else {
        analogWrite(MOTOR_RIGHT_IN1, 0);
        analogWrite(MOTOR_RIGHT_IN2, -speed);
        Serial.print("Reverse ");
    }
    Serial.println(abs(speed));
}

void updateMotors() {
    Serial.println("\n=== Processing Movement ===");
    
    // Emergency stop check
    if (abs(receivedPitch) > EMERGENCY_ANGLE || abs(receivedRoll) > EMERGENCY_ANGLE) {
        Serial.println("EMERGENCY STOP - Angle too high");
        stopMotors();
        return;
    }

    // Calculate base movement
    int forwardSpeed = 0;
    int turnSpeed = 0;

    // Forward/Backward based on pitch
    if (abs(receivedPitch) > MIN_ANGLE) {
        forwardSpeed = map(receivedPitch, -MAX_ANGLE, MAX_ANGLE, MAX_SPEED, -MAX_SPEED);
        Serial.print("Forward Speed: "); Serial.println(forwardSpeed);
    }

    // Turning based on roll
    if (abs(receivedRoll) > MIN_ANGLE) {
        turnSpeed = map(receivedRoll, -MAX_ANGLE, MAX_ANGLE, -MAX_SPEED/2, MAX_SPEED/2);
        Serial.print("Turn Speed: "); Serial.println(turnSpeed);
    }

    // If only turning (no forward/backward movement)
    if (abs(receivedPitch) <= MIN_ANGLE && abs(receivedRoll) > MIN_ANGLE) {
        setLeftMotor(turnSpeed);
        setRightMotor(-turnSpeed);
    }
    // Combined movement
    else if (forwardSpeed != 0 || turnSpeed != 0) {
        int leftSpeed = forwardSpeed + turnSpeed;
        int rightSpeed = forwardSpeed - turnSpeed;
        
        Serial.print("Combined - Left: "); Serial.print(leftSpeed);
        Serial.print(" Right: "); Serial.println(rightSpeed);
        
        setLeftMotor(leftSpeed);
        setRightMotor(rightSpeed);
    }
    // No movement
    else {
        stopMotors();
    }
}

void parseCommand(char* cmd) {
    if (cmd[0] != 'C') {
        Serial.println("Invalid command format");
        return;
    }
    
    char* ptr = strchr(cmd, ',');
    if (!ptr) {
        Serial.println("Missing pitch value");
        return;
    }
    ptr++;
    receivedPitch = atof(ptr);
    
    ptr = strchr(ptr, ',');
    if (!ptr) {
        Serial.println("Missing roll value");
        return;
    }
    ptr++;
    receivedRoll = atof(ptr);
    
    Serial.print("\nReceived - Pitch: "); Serial.print(receivedPitch);
    Serial.print(" Roll: "); Serial.println(receivedRoll);
}

void readBluetooth() {
    while (Serial1.available()) {
        char c = Serial1.read();
        
        if (bufferIndex < sizeof(buffer) - 1) {
            buffer[bufferIndex++] = c;
        }
        
        if (c == '\n') {
            buffer[bufferIndex] = '\0';
            Serial.print("Raw BT data: "); Serial.println(buffer);
            parseCommand(buffer);
            updateMotors();
            bufferIndex = 0;
        }
    }
}

void loop() {
    readBluetooth();
    delay(10);
}
