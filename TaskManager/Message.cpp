#include "Message.h"
// Define the message structure
Message::Message(MessageType type = MessageType::Pool) : type_(type) {}

Message::Message(const Message& message) {
    operator=(message);
}

Message& Message::operator=(const Message& message) {
    type_ = message.type_;
    return *this;
}

// Function to convert enum to string
std::string Message::to_string(MessageType type) {
    switch (type) {
    case MessageType::Pause: return "Pause";
    case MessageType::Resume: return "Resume";
    case MessageType::Stop: return "Stop";
    case MessageType::Pool: return "Pool";
    default: return "Unknown";
    }
}

std::string Message::get_message()const {
    return to_string(type_);
}

void Message::set_message(MessageType message_type) {
    type_ = message_type;  // Directly update the type
}
