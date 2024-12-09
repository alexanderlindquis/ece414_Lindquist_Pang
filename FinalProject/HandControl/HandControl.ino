// Hand Controller Module
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_LSM6DSOX.h>
#include <Wire.h>
#include <SPI.h>

// Pin Definitions
#define TFT_CS    18
#define TFT_DC    16
#define TFT_RST   20
#define TFT_MOSI  19
#define TFT_SCK   17

// Constants
#define MIN_ANGLE 5.0
#define MAX_ANGLE 60.0
#define EMERGENCY_ANGLE 80.0
#define EMERGENCY_RECOVERY_DELAY 1000

// Movement states
enum MovementState {
    STATE_STOPPED,
    STATE_FORWARD,
    STATE_FORWARD_RIGHT,
    STATE_RIGHT,
    STATE_BACKWARD_RIGHT,
    STATE_BACKWARD,
    STATE_BACKWARD_LEFT,
    STATE_LEFT,
    STATE_FORWARD_LEFT
};

// Objects
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
Adafruit_LSM6DSOX sox;

// Global variables
float pitch = 0.0, roll = 0.0;
float compAngleX = 0.0, compAngleY = 0.0;
float pitch_offset = 0.0, roll_offset = 0.0;
unsigned long timer = 0;
MovementState currentState = STATE_STOPPED;
int currentSpeed = 0;
bool emergencyStopActive = false;
unsigned long emergencyStopTime = 0;

void calibrateSensor() {
    const int numSamples = 100;
    float pitch_sum = 0;
    float roll_sum = 0;
    
    for(int i = 0; i < numSamples; i++) {
        sensors_event_t accel;
        sensors_event_t gyro;
        sensors_event_t temp;
        sox.getEvent(&accel, &gyro, &temp);
        
        float accel_angle_y = atan2(-accel.acceleration.x, 
                                   sqrt(accel.acceleration.y * accel.acceleration.y + 
                                       accel.acceleration.z * accel.acceleration.z)) * RAD_TO_DEG;
        float accel_angle_x = atan2(accel.acceleration.y, accel.acceleration.z) * RAD_TO_DEG;
        
        pitch_sum += accel_angle_y;
        roll_sum += accel_angle_x;
        
        delay(10);
    }
    
    pitch_offset = pitch_sum / numSamples;
    roll_offset = roll_sum / numSamples;
    compAngleX = 0;
    compAngleY = 0;
}

void initializeDisplay() {
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_CYAN);
    tft.setCursor(10, 10);
    tft.println("Hand Controller");
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(10, 50);
    tft.println("Pitch:");
    tft.setCursor(10, 80);
    tft.println("Roll:");
    tft.setCursor(10, 110);
    tft.println("Speed:");
    tft.setCursor(10, 140);
    tft.println("State:");
}

void setup() {
    Serial1.begin(9600);  // Bluetooth
    
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(ILI9341_BLACK);
    
    if (!sox.begin_I2C()) {
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.setCursor(10, 10);
        tft.println("Sensor Error!");
        while (1) delay(100);
    }
    
    // Configure sensor
    sox.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
    sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
    sox.setAccelDataRate(LSM6DS_RATE_104_HZ);
    sox.setGyroDataRate(LSM6DS_RATE_104_HZ);
    
    // Calibration message
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Calibrating...");
    tft.println("Keep controller still");
    
    calibrateSensor();
    
    tft.fillScreen(ILI9341_BLACK);
    initializeDisplay();
    
    timer = millis();
}

void updateSensorFusion() {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&accel, &gyro, &temp);

    float dt = (millis() - timer) / 1000.0;
    timer = millis();

    float accel_angle_x = atan2(accel.acceleration.y, accel.acceleration.z) * RAD_TO_DEG - roll_offset;
    float accel_angle_y = atan2(-accel.acceleration.x, 
                               sqrt(accel.acceleration.y * accel.acceleration.y + 
                                   accel.acceleration.z * accel.acceleration.z)) * RAD_TO_DEG - pitch_offset;

    float alpha = 0.98;
    float gyro_magnitude = sqrt(gyro.gyro.x * gyro.gyro.x + gyro.gyro.y * gyro.gyro.y);
    if (gyro_magnitude > 1.0) {
        alpha = 0.99;  // More gyro weight during fast movement
    }

    compAngleX = alpha * (compAngleX + gyro.gyro.x * dt) + (1 - alpha) * accel_angle_x;
    compAngleY = alpha * (compAngleY + gyro.gyro.y * dt) + (1 - alpha) * accel_angle_y;

    pitch = constrain(compAngleY, -90, 90);
    roll = constrain(compAngleX, -90, 90);
}

void calculateMovement() {
    // Check for emergency stop
    if (abs(pitch) > EMERGENCY_ANGLE || abs(roll) > EMERGENCY_ANGLE) {
        if (!emergencyStopActive) {
            emergencyStopActive = true;
            emergencyStopTime = millis();
            currentState = STATE_STOPPED;
            currentSpeed = 0;
        }
        return;
    }
    
    // Handle recovery from emergency stop
    if (emergencyStopActive) {
        if (millis() - emergencyStopTime >= EMERGENCY_RECOVERY_DELAY) {
            emergencyStopActive = false;
            compAngleX = roll;
            compAngleY = pitch;
        } else {
            currentState = STATE_STOPPED;
            currentSpeed = 0;
            return;
        }
    }
    
    float magnitude = sqrt(pitch * pitch + roll * roll);
    
    if (magnitude < MIN_ANGLE) {
        currentState = STATE_STOPPED;
        currentSpeed = 0;
        return;
    }
    
    currentSpeed = map(constrain(magnitude, MIN_ANGLE, MAX_ANGLE), MIN_ANGLE, MAX_ANGLE, 0, 100);
    float direction = atan2(-roll, -pitch) * 180.0 / PI;
    
    // Determine movement state based on direction
    int sector = ((int)(direction + 202.5) % 360) / 45;
    switch(sector) {
        case 0: currentState = STATE_FORWARD; break;
        case 1: currentState = STATE_FORWARD_RIGHT; break;
        case 2: currentState = STATE_RIGHT; break;
        case 3: currentState = STATE_BACKWARD_RIGHT; break;
        case 4: currentState = STATE_BACKWARD; break;
        case 5: currentState = STATE_BACKWARD_LEFT; break;
        case 6: currentState = STATE_LEFT; break;
        case 7: currentState = STATE_FORWARD_LEFT; break;
        default: currentState = STATE_STOPPED;
    }
}

void updateDisplay() {
    char buffer[32];
    
    // Update pitch
    tft.fillRect(100, 50, 100, 20, ILI9341_BLACK);
    tft.setCursor(100, 50);
    tft.setTextColor(ILI9341_WHITE);
    snprintf(buffer, sizeof(buffer), "%.1f", pitch);
    tft.print(buffer);
    
    // Update roll
    tft.fillRect(100, 80, 100, 20, ILI9341_BLACK);
    tft.setCursor(100, 80);
    snprintf(buffer, sizeof(buffer), "%.1f", roll);
    tft.print(buffer);
    
    // Update speed
    tft.fillRect(100, 110, 100, 20, ILI9341_BLACK);
    tft.setCursor(100, 110);
    snprintf(buffer, sizeof(buffer), "%d%%", currentSpeed);
    tft.print(buffer);
    
    // Update state
    tft.fillRect(100, 140, 140, 20, ILI9341_BLACK);
    tft.setCursor(100, 140);
    tft.setTextColor(ILI9341_YELLOW);
    
    switch(currentState) {
        case STATE_FORWARD: 
        tft.print("FORWARD"); 
        break;
        case STATE_FORWARD_RIGHT: 
        tft.print("FWD RIGHT"); 
        break;
        case STATE_RIGHT: 
        tft.print("RIGHT"); 
        break;
        case STATE_BACKWARD_RIGHT: 
        tft.print("BACK RIGHT"); 
        break;
        case STATE_BACKWARD: 
        tft.print("BACKWARD"); 
        break;
        case STATE_BACKWARD_LEFT: 
        tft.print("BACK LEFT"); 
        break;
        case STATE_LEFT: 
        tft.print("LEFT"); 
        break;
        case STATE_FORWARD_LEFT: 
        tft.print("FWD LEFT"); 
        break;

        default: 
        tft.print("STOPPED");
    }
}

void sendControlData() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "C,%.1f,%.1f\n", pitch, roll);
    Serial1.write(buffer);
}

void loop() {
    static unsigned long lastSensorUpdate = 0;
    static unsigned long lastDisplayUpdate = 0;
    
    unsigned long currentTime = millis();
    
    // Sensor update at ~104 Hz (10ms)
    if (currentTime - lastSensorUpdate >= 10) {
        updateSensorFusion();
        calculateMovement();
        sendControlData();
        lastSensorUpdate = currentTime;
    }
    
    // Display update at 10 Hz
    if (currentTime - lastDisplayUpdate >= 100) {
        updateDisplay();
        lastDisplayUpdate = currentTime;
    }
}
