/***************************************************************************
 *                                                                         *
 *                                                                         *
 *                    File Name : common_buf.h                              	*
 *                                                                         *
 *                   Programmer : Bill Randolph                            *
 *                                                                         *
 *                   Start Date : December 19, 1994                        *
 *                                                                         *
 *                  Last Update : April 1, 1995   [BR]                 		*
 *                                                                         *
 *-------------------------------------------------------------------------*
 *                                                                         *
 * This class's job is to store outgoing messages & incoming messages, 		*
 * and serves as a storage area for various flags for ACK & Retry logic.	*
 *                                                                         *
 * This class stores buffers in a non-sequenced order; it allows freeing	*
 * any entry, so the buffers can be kept clear, even if packets come in		*
 * out of order.																				*
 *                                                                         *
 * The class also contains routines to maintain a cumulative response time	*
 * for this queue.  It's up to the caller to call add_delay() whenever		*
 * it detects that an outgoing message has been ACK'd; this class adds		*
 * that delay into a computed average delay over the last few message 		*
 * delays.																						*
 *                                                                         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef COMMON_BUF_H
#define COMMON_BUF_H


/*
********************************** Defines **********************************
*/
/*---------------------------------------------------------------------------
This is one output queue entry
---------------------------------------------------------------------------*/
typedef struct {
	unsigned int is_active	: 1;	// 1 = this entry is ready to be processed
	unsigned int is_ACK		: 1;	// 1 = ACK received for this packet
	unsigned long first_time;		// time this packet was first sent
	unsigned long last_time;		// time this packet was last sent
	unsigned long send_count;		// times this packet has been sent(first send and retry send)
	int buf_len;					// size of the packet stored in this entry
	char *buffer;					// the data packet
	int extra_len;					// size of extra data
	char *extra_buffer;				// extra data buffer
} SendQueueType;

/*---------------------------------------------------------------------------
This is one input queue entry
---------------------------------------------------------------------------*/
typedef struct {
	unsigned int is_active	: 1;	// 1 = this entry is ready to be processed
	unsigned int is_read	: 1;	// 1 = caller has read this entry
	unsigned int is_ACK		: 1;	// 1 = ACK sent for this packet
	int buf_len;					// size of the packet stored in this entry
	char *buffer;					// the data packet
	int extra_len;					// size of extra data
	char *extra_buffer;				// extra data buffer
} ReceiveQueueType;

/*
***************************** Class Declaration *****************************
*/
class CommBufferClass {
	/*
	---------------------------- Public Interface ----------------------------
	*/
	public:
		CommBufferClass(int num_send_queue, int num_recieve_queue, int max_packet_len, int max_extra_len);
		virtual ~CommBufferClass();
		void init(void);
		void init_send_queue(void);

	/*
	......................... Send Queue routines .........................
	*/
	
};

