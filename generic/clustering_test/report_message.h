/* 
 * File:   report_message.h
 * Author: amaxilat
 *
 * Created on July 23, 2011, 9:46 PM
 */

#ifndef REPORT_MESSAGE_H
#define	REPORT_MESSAGE_H

namespace wiselib {

    template
    < typename OsModel_P, typename Radio_P>
    class ReportMsg {
    public:
        typedef OsModel_P OsModel;
        typedef Radio_P Radio;

        typedef typename Radio::node_id_t node_id_t;
        typedef typename Radio::size_t size_t;
        typedef typename Radio::block_data_t block_data_t;
        typedef typename Radio::message_id_t message_id_t;
        // message ids

        enum {
            REPORT_MSG = 121,            
            CLUSTER = 0,
            CLUSTER_MESSAGE = 1,
            NEW_NB = 5,
            NEW_NB_BIDI = 6,
            LOST_NB_BIDI = 7,
            DROPPED_NB = 8,

            NONE = 100
        };

        enum data_positions {
            MSG_ID_POS = 0, // message id position inside the message [uint8]
            REPORT_TYPE_POS = 1,
            MESSAGES_TYPE = 2,
            CLUSTER_ID_POS = 2,
            NODE_ID_POS = 2
        };

        // --------------------------------------------------------------------

        ReportMsg() {
            set_msg_id(REPORT_MSG);
            set_report_type(NONE);
        };
        // --------------------------------------------------------------------

        ~ReportMsg() {
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
        // --------------------------------------------------------------------
        // get the report id

        inline message_id_t report_type() {
            return read<OsModel, block_data_t, uint8_t > (buffer + REPORT_TYPE_POS);
        };


        // set the report id

        inline void set_report_type(message_id_t id) {
            write<OsModel, block_data_t, uint8_t > (buffer + REPORT_TYPE_POS, id);
        };
        // --------------------------------------------------------------------

        inline node_id_t cluster_id() {
            return read<OsModel, block_data_t, node_id_t > (buffer + CLUSTER_ID_POS);
        };

        inline void set_cluster_id(node_id_t cluster) {
            write<OsModel, block_data_t, node_id_t > (buffer + CLUSTER_ID_POS, cluster);
        };

        inline node_id_t node_id() {
            return read<OsModel, block_data_t, node_id_t > (buffer + NODE_ID_POS);
        };

        inline void set_node_id(node_id_t cluster) {
            write<OsModel, block_data_t, node_id_t > (buffer + NODE_ID_POS, cluster);
        };

        inline size_t size() {
            return 2 * sizeof (uint8_t) + sizeof (node_id_t);
        }
    private:
        block_data_t buffer[Radio::MAX_MESSAGE_LENGTH]; // buffer for the message data
    };
}


#endif	/* REPORT_MESSAGE_H */

