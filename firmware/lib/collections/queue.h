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
#include <stdlib.h> // size_t
#endif

namespace OxCollections {

template <class T, size_t L>
class Queue {
    private:
        T _arr[L];
        int _capacity = L;
        int _front = 0;
        int _rear = -1;
        int _count = 0;
    public:
        void enqueue(T t);
        void dequeue();
        T peek();
        int size();
        bool empty();
        bool full();
};


template<class T, size_t L>
void Queue<T, L>::dequeue() {
    if (empty()) {
        exit(EXIT_FAILURE);
    }
    _front = (_front + 1) % _capacity;
    _count--;
}

template<class T, size_t L>
void Queue<T, L>::enqueue(T item) {
    if (full()){
        exit(EXIT_FAILURE);
    }
    _rear = (_rear + 1) % _capacity;
    arr[_rear] = item;
    _count++;
}

template<class T, size_t L>
T Queue<T, L>::peek() {
    if (empty()) {
        exit(EXIT_FAILURE);
    }
    return _arr[_front];
}

template<class T, size_t L>
int Queue<T, L>::size() {
    return _count;
}

template<class T, size_t L>
bool Queue<T, L>::empty() {
    return (size() == 0);
}

template<class T, size_t L>
bool Queue<T, L>::full() {
    return (size() == _capacity);
}

}

#endif
