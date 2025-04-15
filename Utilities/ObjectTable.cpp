#include "ObjectTable.h"

bool ObjectTable::operator==(const ObjectTable& rhs) {
    return data_ == rhs.data_;  // Compare the underlying maps
}

bool ObjectTable::operator!=(const ObjectTable& rhs) {
    return !(*this == rhs);
}
// The operator[] to access or insert an object
Object& ObjectTable::operator[](const std::string& key) {
    std::lock_guard<std::mutex> lock(table_lock_);

    // If the object doesn't exist, we insert a nullptr and return a reference to it
    auto it = data_.find(key);
    if (it != data_.end()) {
        return *it->second;  // Return the object if found
    }

    // Object not found, insert a nullptr (or a default object if preferred)
    std::shared_ptr<Object> new_obj = nullptr;  // Could also create a default object here if needed
    data_[key] = new_obj;
    return *new_obj;  // Return a reference to the nullptr or default object
}
template <typename T>
void ObjectTable::insert(std::shared_ptr<T> object) {
    std::lock_guard<std::mutex> lock(table_lock_);
    data_[object->get_id()] = object;
}

// Find object by UUID
template <typename T>
std::shared_ptr<T> ObjectTable::find(const std::string& id) {
    std::lock_guard<std::mutex> lock(table_lock_);
    auto it = data_.find(id);
    if (it != data_.end()) {
        return std::static_pointer_cast<T>(it->second);
    }
    return nullptr; // Return nullptr if not found
}

// remove object by UUID
void ObjectTable::remove(const std::string& id) {
    std::lock_guard<std::mutex> lock(table_lock_);
    data_.erase(id);
}


