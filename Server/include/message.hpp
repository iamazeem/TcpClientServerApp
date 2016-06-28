#ifndef INCLUDE_MESSAGE_HPP_
#define INCLUDE_MESSAGE_HPP_


/** Macro Replacements for Message **/

#define MSG_VERSION             0x2

// Server Message Types - TO SEND
#define MSG_DEFAULT             0x00
#define MSG_WELCOME             0x11
#define MSG_DATA_FRAME          0x1A
#define MSG_FIN_DATA_FRAME      0x1B
#define MSG_FILE_PRESENT        0x13
#define MSG_FILE_NOT_PRESENT    0x14

// Client Message Types - TO RECV
#define MSG_COMMAND             0x1C
#define MSG_DATA_ACK            0x12
#define MSG_FIN_DATA_ACK        0x1F
#define MSG_EXIT                0x1D


#endif /* INCLUDE_MESSAGE_HPP_ */
