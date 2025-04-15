#pragma once
#include <string>
#include <iomanip>
#include <sstream>
#include <random>

class Entity {
public:
    //constructor
    Entity();
    //deleted copy constructor 
    Entity(const Entity& in) = delete;
    //destructor 
    virtual ~Entity() = default;
    //return the id as type uuid
    virtual std::string get_id() const;
    //equality/inequality check on ids
    bool operator==(const Entity& other) const;
    bool operator!=(const Entity& other) const;
protected:
    // the object id
    std::string object_id_;  // UUID object as ID
private:
    // set the id of the object
    virtual void set_id(const std::string& id);
    // generate id
    std::string generate_uuid();
};