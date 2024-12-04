#ifndef PROTOCOL_H
#define PROTOCOL_H

// Message types
#define MSG_CONTROL 'C'
#define MSG_STATUS  'S'
#define MSG_ERROR   'E'

// Status codes
#define STATUS_OK           0
#define STATUS_LOW_BATTERY  1
#define STATUS_ERROR        2

// Control message format: C,tilt,turn\n
// Example: "C,45.5,-30.2\n"
// Status message format: S,speed,battery\n
// Example: "S,128,75\n"
// Error message format: E,code\n
// Example: "E,1\n"

#endif // PROTOCOL_H