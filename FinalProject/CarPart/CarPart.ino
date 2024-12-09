// Car Control Code for Raspberry Pi Pico with DRV8871 drivers

// Pin definitions for motor drivers
#define MOTOR_LEFT_IN1  2  // GPIO2 to left motor driver IN1
#define MOTOR_LEFT_IN2  3  // GPIO3 to left motor driver IN2
#define MOTOR_RIGHT_IN1 4  // GPIO4 to right motor driver IN1
#define MOTOR_RIGHT_IN2 5  // GPIO5 to right motor driver IN2

// Constants
#define MAX_SPEED 153    // ~60% of 255 for 9V power
#define MIN_SPEED 40     // Minimum speed threshold
#define RAMP_RATE 5      // Speed change per update
#define MIN_ANGLE 5.0    // Minimum angle to start moving
#define MAX_ANGLE 60.0   // Angle for maximum speed
#define EMERGENCY_ANGLE 80.0

// Buffer for receiving data
char buffer[32];
int bufferIndex = 0;

// Variables to store received angles
float receivedPitch = 0.0;
float receivedRoll = 0.0;

// Motor speeds
int targetLeftSpeed = 0;
int targetRightSpeed = 0;
int currentLeftSpeed = 0;
int currentRightSpeed = 0;

void setup() {
    // Initialize serial for Bluetooth
    Serial1.begin(9600);
    
    // Initialize motor control pins
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    
    // Ensure motors are stopped at startup
    stopMotors();
}

void setLeftMotor(int speed) {
    // Gradually adjust speed
    if (currentLeftSpeed < speed) {
        currentLeftSpeed = min(currentLeftSpeed + RAMP_RATE, speed);
    } else if (currentLeftSpeed > speed) {
        currentLeftSpeed = max(currentLeftSpeed - RAMP_RATE, speed);
    }
    
    // Apply speed limits
    currentLeftSpeed = constrain(currentLeftSpeed, -MAX_SPEED, MAX_SPEED);
    
    // Apply minimum speed threshold
    if (abs(currentLeftSpeed) < MIN_SPEED) {
        currentLeftSpeed = 0;
    }
    
    // Set motor direction and speed
    if (currentLeftSpeed >= 0) {
        analogWrite(MOTOR_LEFT_IN1, currentLeftSpeed);
        analogWrite(MOTOR_LEFT_IN2, 0);
    } else {
        analogWrite(MOTOR_LEFT_IN1, 0);
        analogWrite(MOTOR_LEFT_IN2, -currentLeftSpeed);
    }
}

void setRightMotor(int speed) {
    // Gradually adjust speed
    if (currentRightSpeed < speed) {
        currentRightSpeed = min(currentRightSpeed + RAMP_RATE, speed);
    } else if (currentRightSpeed > speed) {
        currentRightSpeed = max(currentRightSpeed - RAMP_RATE, speed);
    }
    
    // Apply speed limits
    currentRightSpeed = constrain(currentRightSpeed, -MAX_SPEED, MAX_SPEED);
    
    // Apply minimum speed threshold
    if (abs(currentRightSpeed) < MIN_SPEED) {
        currentRightSpeed = 0;
    }
    
    // Set motor direction and speed
    if (currentRightSpeed >= 0) {
        analogWrite(MOTOR_RIGHT_IN1, currentRightSpeed);
        analogWrite(MOTOR_RIGHT_IN2, 0);
    } else {
        analogWrite(MOTOR_RIGHT_IN1, 0);
        analogWrite(MOTOR_RIGHT_IN2, -currentRightSpeed);
    }
}

void stopMotors() {
    // Stop both motors
    analogWrite(MOTOR_LEFT_IN1, 0);
    analogWrite(MOTOR_LEFT_IN2, 0);
    analogWrite(MOTOR_RIGHT_IN1, 0);
    analogWrite(MOTOR_RIGHT_IN2, 0);
    
    // Reset current speeds
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
}

void updateMotors() {
    // Check for emergency stop condition
    if (abs(receivedPitch) > EMERGENCY_ANGLE || abs(receivedRoll) > EMERGENCY_ANGLE) {
        stopMotors();
        return;
    }

    float magnitude = sqrt(receivedPitch * receivedPitch + receivedRoll * receivedRoll);
    
    // Check minimum movement threshold
    if (magnitude < MIN_ANGLE) {
        stopMotors();
        return;
    }

    // Calculate movement direction and speed
    float direction = atan2(receivedRoll, -receivedPitch) * 180.0 / PI;
    float speedFactor = constrain(magnitude, MIN_ANGLE, MAX_ANGLE);
    speedFactor = map(speedFactor, MIN_ANGLE, MAX_ANGLE, MIN_SPEED, MAX_SPEED);

    // Calculate individual motor speeds based on direction
    float leftSpeed = 0;
    float rightSpeed = 0;

    // Forward quadrant
    if (direction >= -90 && direction <= 90) {
        if (direction <= 0) {
            // Forward-left
            rightSpeed = speedFactor;
            leftSpeed = rightSpeed * (1 + direction/90.0);
        } else {
            // Forward-right
            leftSpeed = speedFactor;
            rightSpeed = leftSpeed * (1 - direction/90.0);
        }
    }
    // Backward quadrant
    else {
        if (direction <= -90) {
            // Backward-left
            rightSpeed = -speedFactor;
            leftSpeed = rightSpeed * (1 + (direction+180)/90.0);
        } else {
            // Backward-right
            leftSpeed = -speedFactor;
            rightSpeed = leftSpeed * (1 + (180-direction)/90.0);
        }
    }

    // Apply calculated speeds to motors
    setLeftMotor(leftSpeed);
    setRightMotor(rightSpeed);
}

void parseCommand(char* cmd) {
    // Check for control message format
    if (cmd[0] != 'C') return;
    
    // Parse first value (pitch)
    char* ptr = strchr(cmd, ',');
    if (!ptr) return;
    ptr++;
    receivedPitch = atof(ptr);
    
    // Parse second value (roll)
    ptr = strchr(ptr, ',');
    if (!ptr) return;
    ptr++;
    receivedRoll = atof(ptr);
}

void readBluetooth() {
    // Read incoming Bluetooth data
    while (Serial1.available()) {
        char c = Serial1.read();
        
        // Store character if buffer has space
        if (bufferIndex < sizeof(buffer) - 1) {
            buffer[bufferIndex++] = c;
        }
        
        // Process complete message on newline
        if (c == '\n') {
            buffer[bufferIndex] = '\0';
            parseCommand(buffer);
            updateMotors();
            bufferIndex = 0;
        }
    }
}

void loop() {
    readBluetooth();
    delay(10);  // Small delay to prevent overwhelming the system
}
