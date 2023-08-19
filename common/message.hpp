#pragma once

// Macro Replacements for Message

#define MSG_VERSION 0x2

// Server Message Types - TO SEND
#define MSG_DEFAULT 0x00
#define MSG_WELCOME 0x11
#define MSG_COMMAND_OUTPUT 0x1A

// Client Message Types - TO RECV
#define MSG_COMMAND 0x1C
#define MSG_DATA_ACK 0x12
#define MSG_FIN_DATA_ACK 0x1F
#define MSG_EXIT 0x1D
