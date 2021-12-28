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

#ifndef MAP_H
#define MAP_H

#include <cstddef>
#include <iostream>

namespace OxCollections {

template<typename K, typename V>
struct KeyValue {
    K key;
    V value;
};

template <typename K, typename V, std::size_t L>
class Map {
    private:
        KeyValue<K, V> arr[L];
        int _count = 0;
    public:
        bool add(K key, V value);
        V getValue(K key);
        int hasKey(K key);
        int size();
        bool isEmpty();
        bool isFull();
        V getValueByIndex(int index);
        int getCount();
};


template <typename K, typename V, std::size_t L>
bool Map<K, V, L>::add(K k, V v) {
    std::cout << "K: " << k << " V: " << v << std::endl;
    if (hasKey(k) >= 0) {
        return false;
    }
    if (true == isFull()) {
        return false;
    }
    arr[_count].key = k;
    arr[_count].value = v;
    //std::cout << "_count: " << _count << std::endl;
    //std::cout << "K: " << arr[_count].key << " V: " << arr[_count].value << std::endl;
    _count++;
    std::cout << "_count: " << _count << std::endl;
    return true;
}

template <typename K, typename V, std::size_t L>
V Map<K, V, L>::getValue(K key) {
    int i = hasKey(key);
    std::cout << "K: " << key << std::endl;
    std::cout << "i: " << i << std::endl;
    
    if (i >= 0) {
        std::cout << "arr[i].value: " << arr[i].value << std::endl;
        return arr[i].value;
    } else {
        std::cout << "Cant get value!" << std::endl;
        return nullptr;
    }
}

template <typename K, typename V, std::size_t L>
int Map<K, V, L>::hasKey(K key) {
    for (int i = 0; i < _count; i++) {
        if (arr[i].key == key) {
            return i;
        }
    }
    return -1;
}

template <typename K, typename V, std::size_t L>
int Map<K, V, L>::size() {
    return L;
}

template <typename K, typename V, std::size_t L>
bool Map<K, V, L>::isEmpty() {
    return _count > 0;
}

template <typename K, typename V, std::size_t L>
bool Map<K, V, L>::isFull() {
    return _count == L;
}

template <typename K, typename V, std::size_t L>
V Map<K, V, L>::getValueByIndex(int index) {
    if ((index >= 0) && (index < _count)) {
        //std::cout << "key: " << arr[index].key << " value: " << arr[index].value << std::endl;
        return arr[index].value;
    } else {
        std::cout << "Cant get value!" << std::endl;
        return nullptr;
    }
}

template <typename K, typename V, std::size_t L>
int Map<K, V, L>::getCount() {
    return _count;
}

}

#endif