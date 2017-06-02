/* 
queue.cpp

Authors: Adam Liber, Andrew Goodney + staff

Description: This is a queue created specifically for image labeling
breadth-first-search fill algorithm see label.cpp

*/

#include "queue.h"


// constructor. maxlen must be as large as the total number
// of Locations that will ever be entered into this Queue.

Queue::Queue(int maxlen) {
   contents = new Location[maxlen];
   head = 0;
   tail = 0;
}

// destructor. releases resources. C++ will call it automatically.
Queue::~Queue() {
   delete [] contents;
}

// insert a new Location at the end/back of our list   
void Queue::push(Location loc) {

   contents[tail] = loc;
   tail++;


   
}

// return and "remove" the oldest Location not already extracted
Location Queue::pop() {

   return contents[head++];
   
   
}

// check for empty
bool Queue::is_empty() {

   if(head == tail){
      return true;
      }
   else{
      return false;
   }
 
}

