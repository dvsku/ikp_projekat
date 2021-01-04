#include "queueing_service_client.h"
#include "../helpers/enums.h"
#include "../helpers/message.h"
#include "../queueing_service.h"

namespace queueing_service {
    queueing_service_client::queueing_service_client(queueing_service* t_parent) : 
        common::base_client::base_client(), service_message_handler::service_message_handler(t_parent) {

        m_parent = t_parent;
    }

    /// <summary>
    /// Called when service receives a message from host queueing service.
    /// </summary>
    /// <param name="t_buffer">Message buffer</param>
    /// <param name="t_length">Bytes read</param>
    void queueing_service_client::handle_recv(char* t_buffer, unsigned int t_length) {
        handle_message(t_buffer, t_length);
    }

    /// <summary>
    /// Called when the host queueing service disconnects. 
    /// </summary>
    void queueing_service_client::handle_disconnect() {
        m_parent->on_service_disconnected();
    }
}