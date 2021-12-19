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

#include <list.h>

namespace OxCollections {

template<class T>
List<T>::List(int size) {
    arr = new T[size];
    capacity = size;
    count = 0;
    index = 0;
}

template<class T>
int List<T>::size() {
    return count;
}

template<class T>
T List<T>::next() {
    int i = index++;
    if (index > count) {
        index = 0;
    }
    //std::cout << "Return item: " << i << "/" << count << " Index: " << index 
    //                << " Capacity: " << capacity << std::endl;
    return arr[i];
}

template<class T>
bool List<T>::add(T item) {
    if (count >= capacity) {
        //std::cout << "List is full! Cant add item!\n";    
        return false;
    }
    //std::cout << "Added item. Now " << count + 1 << "/" << capacity << " items\n";
    arr[count++] = item;
    return true;
}

}
