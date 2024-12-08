// Motor driver pins (DRV8871)
#define MOTOR_A_IN1 2  // Left motor
#define MOTOR_A_IN2 3
#define MOTOR_B_IN1 4  // Right motor
#define MOTOR_B_IN2 5

// Constants
#define MAX_SPEED 255
#define MIN_SPEED 0
#define MIN_ANGLE 5.0
#define MAX_ANGLE 60.0
#define EMERGENCY_ANGLE 80.0

// Buffer for receiving data
char buffer[32];
int bufferIndex = 0;

// Variables to store received angles
float receivedPitch = 0.0;
float receivedRoll = 0.0;

// Motor speeds
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    
    pinMode(MOTOR_A_IN1, OUTPUT);
    pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_IN1, OUTPUT);
    pinMode(MOTOR_B_IN2, OUTPUT);
    
    stopMotors();
    Serial.println("Car Control Started");
}

void setLeftMotor(int speed) {
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    if (speed > 0) {
        analogWrite(MOTOR_A_IN1, speed);
        analogWrite(MOTOR_A_IN2, 0);
    } else if (speed < 0) {
        analogWrite(MOTOR_A_IN1, 0);
        analogWrite(MOTOR_A_IN2, -speed);
    } else {
        analogWrite(MOTOR_A_IN1, 0);
        analogWrite(MOTOR_A_IN2, 0);
    }
}

void setRightMotor(int speed) {
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    if (speed > 0) {
        analogWrite(MOTOR_B_IN1, speed);
        analogWrite(MOTOR_B_IN2, 0);
    } else if (speed < 0) {
        analogWrite(MOTOR_B_IN1, 0);
        analogWrite(MOTOR_B_IN2, -speed);
    } else {
        analogWrite(MOTOR_B_IN1, 0);
        analogWrite(MOTOR_B_IN2, 0);
    }
}

void stopMotors() {
    setLeftMotor(0);
    setRightMotor(0);
}

void updateMotors() {
    if (abs(receivedPitch) > EMERGENCY_ANGLE || abs(receivedRoll) > EMERGENCY_ANGLE) {
        stopMotors();
        Serial.println("EMERGENCY STOP!");
        return;
    }

    float magnitude = sqrt(receivedPitch * receivedPitch + receivedRoll * receivedRoll);
    
    if (magnitude < MIN_ANGLE) {
        stopMotors();
        Serial.println("STOPPED - Below minimum angle");
        return;
    }

    float direction = atan2(receivedRoll, -receivedPitch) * 180.0 / PI;
    float speedFactor = constrain(magnitude, MIN_ANGLE, MAX_ANGLE);
    speedFactor = map(speedFactor, MIN_ANGLE, MAX_ANGLE, 0, MAX_SPEED);

    float leftSpeed = 0;
    float rightSpeed = 0;

    // Forward quadrant
    if (direction >= -90 && direction <= 90) {
        if (direction <= 0) {
            // Forward-left quadrant
            rightSpeed = speedFactor;
            leftSpeed = rightSpeed * (1 + direction/90.0);
        } else {
            // Forward-right quadrant
            leftSpeed = speedFactor;
            rightSpeed = leftSpeed * (1 - direction/90.0);
        }
    }
    // Backward quadrant
    else {
        if (direction <= -90) {
            // Backward-left quadrant
            rightSpeed = -speedFactor;
            leftSpeed = rightSpeed * (1 + (direction+180)/90.0);
        } else {
            // Backward-right quadrant
            leftSpeed = -speedFactor;
            rightSpeed = leftSpeed * (1 + (180-direction)/90.0);
        }
    }

    setLeftMotor(leftSpeed);
    setRightMotor(rightSpeed);

    Serial.println("\n----- Movement Status -----");
    Serial.print("Pitch: "); Serial.print(receivedPitch);
    Serial.print("° Roll: "); Serial.print(receivedRoll);
    Serial.print("° Direction: "); Serial.print(direction);
    Serial.print("° Magnitude: "); Serial.println(magnitude);
    
    Serial.print("Left Motor: "); Serial.print(leftSpeed);
    Serial.print(" Right Motor: "); Serial.println(rightSpeed);
}

void parseCommand(char* cmd) {
    if (cmd[0] != 'C') return;
    
    char* ptr = strchr(cmd, ',');
    if (!ptr) return;
    ptr++;
    
    receivedPitch = atof(ptr);
    
    ptr = strchr(ptr, ',');
    if (!ptr) return;
    ptr++;
    
    receivedRoll = atof(ptr);
}

void readBluetooth() {
    while (Serial1.available()) {
        char c = Serial1.read();
        
        if (bufferIndex < sizeof(buffer) - 1) {
            buffer[bufferIndex++] = c;
        }
        
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
    delay(10);
}