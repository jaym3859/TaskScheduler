#pragma once
#include "Event.h"

    // Subscribe a listener to the event
    void Event::subscribe(std::shared_ptr<Listener> listener) {
        listeners_.emplace_back(std::move(listener));  // Add the listener to the list
    }

    // Set a callback to be triggered when the event is notified
    void Event::set_callback(std::function<void()> callback) {
        callback_ = std::move(callback);
    }

    // Notify all listeners and trigger the callback if defined
    void Event::notify() const {
        // Create a local copy of the listener pool to avoid modification during iteration
       // auto listeners_copy = listeners_.get_pool();  // This now works with a const reference

        for (const auto& listener : listeners_) {
            listener.get()->on_event_triggered();  // Notify each listener
        }

        // If a callback is defined, execute it
        if (callback_) {
            callback_();  // Execute the callback function
        }
    }
