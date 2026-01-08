#ifndef RINGQUEUE_H
#define RINGQUEUE_H

#include <Arduino.h>

/**
 * RingQueue: Fixed-size ring buffer FIFO.
 * 
 * Template-based to hold any fixed-size payload.
 * 
 * Usage:
 *   struct Event { uint8_t cmd; uint8_t meta; };
 *   RingQueue<Event, 16> queue;
 *   
 *   queue.enqueue({CMD_FIRE, 0});
 *   if (!queue.isEmpty()) {
 *     Event e = queue.dequeue();
 *   }
 */
/**
 * @brief Ring queue (circular buffer) template class
 * 
 * @tparam T The data type of elements stored in the queue
 * @tparam SIZE A compile-time constant that specifies the maximum capacity of the queue.
 *              SIZE is set when you instantiate the template, e.g.:
 *              RingQueue<int, 10> myQueue;  // SIZE = 10
 *              RingQueue<float, 256> myQueue;  // SIZE = 256
 *              This creates a fixed-size circular buffer that can hold up to SIZE elements.
 */
template <typename T, size_t SIZE>
class RingQueue {
public:
  RingQueue() : head(0), tail(0), count(0) {
  }
  
  // Add item to queue
  bool enqueue(const T& item) {
    if (count >= SIZE) {
      return false;  // Queue full
    }
    buffer[head] = item;
    head = (head + 1) % SIZE;
    count++;
    return true;
  }
  
  // Remove item from queue
  T dequeue() {
    T item = buffer[tail];
    tail = (tail + 1) % SIZE;
    count--;
    return item;
  }
  
  // Check if empty
  bool isEmpty() const {
    return count == 0;
  }
  
  // Check if full
  bool isFull() const {
    return count >= SIZE;
  }
  
  // Get count
  size_t getCount() const {
    return count;
  }
  
  // Peek at front (don't remove)
  T peek() const {
    return buffer[tail];
  }
  
  // Clear queue
  void clear() {
    head = 0;
    tail = 0;
    count = 0;
  }

private:
  T buffer[SIZE];
  size_t head;
  size_t tail;
  size_t count;
};

#endif // RINGQUEUE_H
