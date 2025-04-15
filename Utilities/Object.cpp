#include "Object.h"
Object::Object() { set_id(generate_uuid()); }

std::string Object::get_id() const {
    return object_id_;
}

bool Object::operator==(const Object& other) const {
    return object_id_ == other.object_id_;
}

bool Object::operator!=(const Object& other) const {
    return object_id_ != other.object_id_;
}
void Object::set_id(const std::string& id) { object_id_ = id; }
/*
boost::uuids::uuid Object::generate_uuid() {
    boost::uuids::random_generator generator;
    return generator();  // Generates a new random UUID
}
*/
std::string Object::generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15); // Hex digits (0 to F)

    std::stringstream uuid;
    uuid << std::hex << std::setfill('0');
    for (int i = 0; i < 8; ++i) {
        uuid << std::setw(1) << dis(gen);
    }
    uuid << "-";
    for (int i = 0; i < 4; ++i) {
        uuid << std::setw(1) << dis(gen);
    }
    uuid << "-4"; // Version 4 UUID
    for (int i = 0; i < 3; ++i) {
        uuid << std::setw(1) << dis(gen);
    }
    uuid << "-";
    for (int i = 0; i < 4; ++i) {
        uuid << std::setw(1) << dis(gen);
    }
    uuid << "-";
    for (int i = 0; i < 12; ++i) {
        uuid << std::setw(1) << dis(gen);
    }
    return uuid.str();
}