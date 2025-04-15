#pragma once
#include <string>
#include "Entity.h"

enum class MessageType {
    Pause,
    Resume,
    Pool,
    Stop,
};

// Define the message structure
struct Message : public Entity {
    MessageType type_;  // type of message
    Message() = default;
    Message(MessageType type);
    Message(std::string s);
    Message(const Message& message);
    Message& operator=(const Message& message);
    // Function to convert enum to string
    static std::string to_string(MessageType type);
    std::string get_message() const;
    void set_message(MessageType message_type);
};