/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef QUEUE_H
#define QUEUE_H

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <stdlib.h>
#endif

namespace OxCollections {

template <class T, size_t L>
class Queue {
    private:
        T arr[L];
        int capacity = L;
        int front = 0;
        int rear = -1;
        int count = 0;
    public:
        void enqueue(T t);
        void dequeue();
        T peek();
        int size();
        bool isEmpty();
        bool isFull();
};


template<class T, size_t L>
void Queue<T, L>::dequeue() {
    if (isEmpty()) {
        exit(EXIT_FAILURE);
    }
    front = (front + 1) % capacity;
    count--;
}

template<class T, size_t L>
void Queue<T, L>::enqueue(T item) {
    if (isFull()){
        exit(EXIT_FAILURE);
    }
    rear = (rear + 1) % capacity;
    arr[rear] = item;
    count++;
}

template<class T, size_t L>
T Queue<T, L>::peek() {
    if (isEmpty()) {
        exit(EXIT_FAILURE);
    }
    return arr[front];
}

template<class T, size_t L>
int Queue<T, L>::size() {
    return count;
}

template<class T, size_t L>
bool Queue<T, L>::isEmpty() {
    return (size() == 0);
}

template<class T, size_t L>
bool Queue<T, L>::isFull() {
    return (size() == capacity);
}

}

#endif
