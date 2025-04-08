#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class Object {
public:
    //constructor
    Object();
    //deleted copy constructor 
    Object(const Object& in) = delete;
    //destructor 
    virtual ~Object() = default;
    //return the id as type uuid
    boost::uuids::uuid get_id() const;
    // returns id as a string
    std::string id_to_string() const;
    // returns id as a wstring
    std::wstring id_to_wstring() const;
    //equality/inequality check on ids
    bool operator==(const Object& other) const;
    bool operator!=(const Object& other) const;
protected:
    // the object id
    std::atomic<boost::uuids::uuid> object_id;  // UUID object as ID
private:
    // set the id of the object
    virtual void set_id(const boost::uuids::uuid& id);
    // generate id
    boost::uuids::uuid generate_uuid();
};