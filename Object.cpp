#include "Object.h"
Object::Object() { set_id(generate_uuid()); }

boost::uuids::uuid Object::get_id() const {
    return object_id;
}

// Optional method to print the object_id as a string
std::string Object::id_to_string() const {
    return boost::uuids::to_string(object_id);  // Ensure the right boost header is included
}

// Optional method to print the object_id as a wstring
std::wstring Object::id_to_wstring() const {
    return boost::uuids::to_wstring(object_id);  // Ensure the right boost header is included
}
bool Object::operator==(const Object& other) const {
    return object_id == other.object_id;
}

bool Object::operator!=(const Object& other) const {
    return object_id != other.object_id;
}
void Object::set_id(const boost::uuids::uuid& id) { object_id = id; }

boost::uuids::uuid Object::generate_uuid() {
    boost::uuids::random_generator generator;
    return generator();  // Generates a new random UUID
}