#ifndef PROTOCOL_H
#define PROTOCOL_H

// Message types
#define MSG_CONTROL 'C'  // Control message from hand controller to car
#define MSG_STATUS  'S'  // Status message from car to hand controller
#define MSG_ERROR   'E'  // Error message from car to hand controller

// Status codes
#define STATUS_OK           0
#define STATUS_LOW_BATTERY  1
#define STATUS_ERROR        2
#define STATUS_EMERGENCY    3

// Control message format: C,tilt,turn\n
// Example: "C,45.5,-30.2\n"
// Status message format: S,speed\n
// Example: "S,128\n"
// Error message format: E,code\n
// Example: "E,1\n"

#endif // PROTOCOL_H