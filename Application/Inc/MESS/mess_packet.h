/*
 * mess_packet.h
 *
 *  Created on: Feb 12, 2025
 *      Author: ericv
 */

#ifndef MESS_MESS_PACKET_H_
#define MESS_MESS_PACKET_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "mess_main.h"
#include <stdbool.h>


/* Private includes ----------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/


typedef struct {
  uint8_t data[PACKET_MAX_LENGTH_BYTES];
  uint16_t bit_count;
  uint8_t sender_id;
  uint16_t data_len_bits;
  MessageData_t contents_data_type;
  uint16_t final_length;
  bool stationary_flag;
  bool preamble_received;
  bool fully_received;
} BitMessage_t;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/



/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief Prepares a message for transmission by encoding it into a bit packet
 *
 * This function performs the complete packet preparation sequence:
 * 1. Initializes the bit packet
 * 2. Adds preamble (skipped for EVAL type messages)
 * 3. Adds message payload
 * 4. Applies error correction coding (skipped for EVAL type messages)
 *
 * @param msg Pointer to the message to be transmitted
 * @param bit_msg Pointer to the bit message structure to be filled
 *
 * @return true if preparation succeeded, false on any failure
 */
bool Packet_PrepareTx(Message_t* msg, BitMessage_t* bit_msg);

/**
 * @brief Initializes a bit message structure for receiving incoming data
 *
 * @param bit_msg Pointer to the bit message structure to initialize
 *
 * @return true if initialization succeeded
 */
bool Packet_PrepareRx(BitMessage_t* bit_msg);

/**
 * @brief Adds a single bit to a bit message
 *
 * Appends a bit to the end of the bit message, handling the necessary
 * byte and bit indexing operations.
 *
 * @param bit_msg Pointer to the bit message structure
 * @param bit The bit value to add (true=1, false=0)
 *
 * @return true if successful, false if packet is already at maximum capacity
 */
bool Packet_AddBit(BitMessage_t* bit_msg, bool bit);

/**
 * @brief Retrieves a bit value from a specific position in a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param position Zero-based index of the bit to retrieve
 * @param bit Pointer where the retrieved bit will be stored
 *
 * @return true if successful, false if position is out of bounds
 */
bool Packet_GetBit(BitMessage_t* bit_msg, uint16_t position, bool* bit);

/**
 * @brief Extracts an arbitrary-length chunk of bits (up to 8) from a bit message
 *
 * This function handles misaligned bit access across byte boundaries.
 *
 * @param bit_msg Pointer to the bit message structure
 * @param start_position Pointer to the starting bit position (will be updated)
 * @param chunk_length Number of bits to extract (must be ≤ 8)
 * @param ret Pointer where the extracted bit chunk will be stored
 *
 * @return true if successful, false if requested bits exceed message bounds
 */
bool Packet_Get8BitChunk(BitMessage_t* bit_msg, uint16_t* start_position, uint8_t chunk_length, uint8_t* ret);

/**
 * @brief Adds an 8-bit value to a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param data The 8-bit value to add
 *
 * @return true if successful, false if packet would exceed maximum size
 */
bool Packet_Add8(BitMessage_t* bit_msg, uint8_t data);

/**
 * @brief Adds a 16-bit value to a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param data The 16-bit value to add
 *
 * @return true if successful, false if packet would exceed maximum size
 */
bool Packet_Add16(BitMessage_t* bit_msg, uint16_t data);

/**
 * @brief Adds a 32-bit value to a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param data The 32-bit value to add
 *
 * @return true if successful, false if packet would exceed maximum size
 */
bool Packet_Add32(BitMessage_t* bit_msg, uint32_t data);

/**
 * @brief Extracts an 8-bit value from a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param start_position Pointer to the starting bit position (will be updated)
 * @param data Pointer where the extracted value will be stored
 *
 * @return true if successful, false if requested bits exceed message bounds
 */
bool Packet_Get8(BitMessage_t* bit_msg, uint16_t* start_position, uint8_t* data);

/**
 * @brief Extracts a 16-bit value from a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param start_position Pointer to the starting bit position (will be updated)
 * @param data Pointer where the extracted value will be stored
 *
 * @return true if successful, false if requested bits exceed message bounds
 */
bool Packet_Get16(BitMessage_t* bit_msg, uint16_t* start_position, uint16_t* data);

/**
 * @brief Extracts a 32-bit value from a bit message
 *
 * @param bit_msg Pointer to the bit message structure
 * @param start_position Pointer to the starting bit position (will be updated)
 * @param data Pointer where the extracted value will be stored
 *
 * @return true if successful, false if requested bits exceed message bounds
 */
bool Packet_Get32(BitMessage_t* bit_msg, uint16_t* start_position, uint32_t* data);

/**
 * @brief Calculates the minimum power-of-2 packet size needed for a given payload
 *
 * @param str_len The length of data to accommodate
 *
 * @return The minimum packet size (always a power of 2)
 */
uint16_t Packet_MinimumSize(uint16_t str_len);

/**
 * @brief Registers modem parameters with the parameter subsystem for HMI access
 *
 * Registers:
 * - The modem identifier
 * - The stationary flag (indicating if the modem is in a fixed position)
 *
 * @return true if all parameters were registered successfully
 */
bool Packet_RegisterParams();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MESS_MESS_PACKET_H_ */
