/* 
 * File:   controll_message.h
 * Author: amaxilat
 *
 * Created on July 23, 2011, 5:50 PM
 */

#ifndef CONTROLL_MESSAGE_H
#define	CONTROLL_MESSAGE_H


namespace wiselib {

    template
    < typename OsModel_P, typename Radio_P>
    class ControllMsg {
    public:
        typedef OsModel_P OsModel;
        typedef Radio_P Radio;

        typedef typename Radio::node_id_t node_id_t;
        typedef typename Radio::size_t size_t;
        typedef typename Radio::block_data_t block_data_t;
        typedef typename Radio::message_id_t message_id_t;
        // message ids

        enum {
            CONTROLL_MSG = 120,
            ON = 1,
            OFF = 0,
            FAIL = 2,
            RECOVER = 3,
            CHANGE_K = 4,
            NONE = 100
        };

        enum data_positions {
            MSG_ID_POS = 0, // message id position inside the message [uint8]
            CONTROLL_TYPE_POS = 1,
            PAYLOAD_POS = 2
        };

        // --------------------------------------------------------------------

        ControllMsg() {
            set_msg_id(CONTROLL_MSG);
            set_controll_type(NONE);
        };
        // --------------------------------------------------------------------

        ~ControllMsg() {
        };

        // get the message id

        inline message_id_t msg_id() {
            return read<OsModel, block_data_t, uint8_t > (buffer + MSG_ID_POS);

        };
        // --------------------------------------------------------------------

        // set the message id

        inline void set_msg_id(message_id_t id) {
            write<OsModel, block_data_t, uint8_t > (buffer + MSG_ID_POS, id);
        };

        // get the collector id

        inline message_id_t controll_type() {
            return read<OsModel, block_data_t, uint8_t > (buffer + CONTROLL_TYPE_POS);
        };
        // --------------------------------------------------------------------

        // set the collector id

        inline void set_controll_type(message_id_t id) {
            write<OsModel, block_data_t, uint8_t > (buffer + CONTROLL_TYPE_POS, id);
        };

        inline int payload() {
            return read<OsModel, block_data_t, int > (buffer + PAYLOAD_POS);
        };

        inline void set_payload(int k_val) {
            write<OsModel, block_data_t, int > (buffer + PAYLOAD_POS, k_val);
        };

        inline size_t size() {
            return 2 * sizeof (uint8_t) + sizeof (int);
        }
    private:
        block_data_t buffer[Radio::MAX_MESSAGE_LENGTH]; // buffer for the message data
    };
}


#endif	/* CONTROLL_MESSAGE_H */

