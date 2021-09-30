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

#include <queue.h>

namespace OxCollections {

template <class T>
Queue<T>::Queue(int size) {
    arr = new T[size];
    capacity = size;
    front = 0;
    rear = -1;
    count = 0;
}

template<class T>
void Queue<T>::dequeue() {
    if (isEmpty()) {
        std::cout << "Underflow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Removing " << arr[front] << std::endl;
    front = (front + 1) % capacity;
    count--;
}

template <class T>
void Queue<T>::enqueue(T item) {
    if (isFull()){
        std::cout << "Overflow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Inserting " << item << std::endl;
    rear = (rear + 1) % capacity;
    arr[rear] = item;
    count++;
}

template <class T>
T Queue<T>::peek() {
    if (isEmpty()) {
        std::cout << "Underflow\nProgram Terminated\n";
        exit(EXIT_FAILURE);
    }
    return arr[front];
}

template <class T>
int Queue<T>::size() {
    return count;
}

template <class T>
bool Queue<T>::isEmpty() {
    return (size() == 0);
}

template <class T>
bool Queue<T>::isFull() {
    return (size() == capacity);
}

}
