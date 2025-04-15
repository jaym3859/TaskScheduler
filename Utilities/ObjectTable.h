#pragma once
#include "Object.h"
#include <unordered_map>
#include <mutex>
//a hash table of objects key'd by uuid 
class ObjectTable
{
public:
    ObjectTable() = default;
    ObjectTable(const ObjectTable& in) = default;
    ObjectTable& operator=(const ObjectTable& rhs) = default;
    ~ObjectTable() = default;
    bool operator==(const ObjectTable& rhs);
    bool operator!=(const ObjectTable& rhs);
    Object& operator[](const std::string& key);
    // Insert object into the table
    template <typename T>
    void insert(std::shared_ptr<T> object);
    // Find object by UUID
    template <typename T>
    std::shared_ptr<T> find(const std::string& id);
    // remove object by UUID
    void remove(const std::string& id);
  
private:
    std::mutex table_lock_;
    std::unordered_map<std::string, std::shared_ptr<Object>> data_;
};

