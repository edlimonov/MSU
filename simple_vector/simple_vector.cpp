#pragma once
#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include <string>
#include <utility>

using namespace std;

class ReserveProxyObj {
    
public:
    ReserveProxyObj(size_t capacity_to_reserve) : capacity_(capacity_to_reserve)
    {
    }
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size), vector_(ArrayPtr<Type>(size))
    {
        if (size != 0){
            for (size_t i = 0; i < size; i++){
                vector_[i] = Type();
            }
        }
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), vector_(ArrayPtr<Type>(size))
    {
        if (size != 0){
            for (size_t i = 0; i < size; i++){
                vector_[i] = value;
            }
        }
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()), vector_(ArrayPtr<Type>(init.size()))
    {
        if (size_ != 0){
            std::copy(init.begin(), init.end(), vector_.Get());
        }
    }
    
    SimpleVector(const SimpleVector& other) {
        assert(size_ == 0 && vector_.Get() == nullptr);
        
        vector_ = ArrayPtr<Type>(other.size_);
        
        for (size_t i = 0; i < other.size_; i++){
           vector_[i] = other[i];
        }
        
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        vector_ = ArrayPtr<Type>(rhs.size_);
        
        for (size_t i = 0; i < rhs.size_; i++){
            vector_[i] = std::move(rhs[i]);
        }
        
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        return *this;
    }
    
    SimpleVector(SimpleVector&& other){
        ArrayPtr<Type> tmp(std::move(other.vector_));
        vector_.swap(tmp);
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        assert(this != &rhs);
        vector_ = std::move(rhs.vector_);
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        return *this;
    }
    
    SimpleVector(ReserveProxyObj tmp){
        ArrayPtr<Type> test(tmp.capacity_);
        vector_.swap(test);
        size_ = 0;
        capacity_ = tmp.capacity_;
    }
    
    void Reserve(size_t new_capacity){
        if (new_capacity > capacity_){
            ArrayPtr<Type> tmp(new_capacity);
            for (size_t i = 0; i < size_; i++){
                tmp[i] = vector_[i];
            }
            vector_.swap(tmp);
            capacity_ = new_capacity;
        }
    }
    
    void PushBack(const Type& item) {
        
        if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            tmp[0] = item;
            vector_.swap(tmp);
            size_ = 1;
            capacity_ = 1;
            return;
        }
        
        if (size_ < capacity_){
            vector_[size_] = item;
            size_++;
            return;
        }
        
        ArrayPtr<Type> tmp(2*size_);
        
        for (size_t i = 0; i < size_; i++){
            tmp[i] = vector_[i];
        }
        tmp[size_] = item;
        vector_.swap(tmp);
        capacity_ = 2*size_;
        size_++;
    }
    
    void PushBack(Type&& item) {
        
        if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            tmp[0] = std::move(item);
            vector_.swap(tmp);
            size_ = 1;
            capacity_ = 1;
            return;
        }
        
        if (size_ < capacity_){
            vector_[size_] = std::move(item);
            size_++;
            return;
        }
        
        ArrayPtr<Type> tmp(2*size_);
        
        for (size_t i = 0; i < size_; i++){
            tmp[i] = std::move(vector_[i]);
        }
        tmp[size_] = std::move(item);
        vector_.swap(tmp);
        capacity_ = 2*size_;
        size_++;
    }
    
    void PopBack() noexcept {
        assert(size_ != 0);
        size_--;
    }
    
    void swap(SimpleVector& other) noexcept {
        vector_.swap(other.vector_);
        std::swap(size_, other.size());
        std::swap(capacity_, other.capacity_);
    }
    
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t dist = distance(cbegin(), pos);
        assert(dist <= size_);
        
        if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            tmp[0] = value;
            vector_.swap(tmp);
            size_ = 1;
            capacity_ = 1;
            return vector_.Get();
        }
        
        if (size_ < capacity_){
            copy_backward(pos, end(), end()+1);
            vector_[dist] = value;
            size_++;
            return &vector_[dist];
        }
        
        ArrayPtr<Type> tmp(2*size_);
        copy(begin(), pos, tmp.Get());
        tmp[dist] = value;
        copy(pos, end(), tmp.Get() + dist+1);
        
        vector_.swap(tmp);
        capacity_ = 2*size_;
        size_++;
        return &vector_[dist];
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t dist = distance(cbegin(), pos);
        assert(dist <= size_);
        
        if (capacity_ == 0){
            ArrayPtr<Type> tmp(1);
            tmp[0] = std::move(value);
            vector_.swap(tmp);
            size_ = 1;
            capacity_ = 1;
            return vector_.Get();
        }
        
        if (size_ < capacity_){
            copy_backward(move_iterator<Type*>(const_cast<Type*>(pos)), move_iterator<Type*>(end()), end()+1);
            vector_[dist] = std::move(value);
            size_++;
            return &vector_[dist];
        }
        
        ArrayPtr<Type> tmp(2*size_);
        copy(move_iterator<Type*>(begin()), move_iterator<Type*>(const_cast<Type*>(pos)), tmp.Get());
        tmp[dist] = std::move(value);
        copy(move_iterator<Type*>(const_cast<Type*>(pos)), move_iterator<Type*>(end()), tmp.Get() + dist+1);
        
        vector_.swap(tmp);
        capacity_ = 2*size_;
        size_++;
        return &vector_[dist];
    }
    
    Iterator Erase(ConstIterator pos) {
        size_t dist = distance(cbegin(), pos);
        assert(dist < size_);
        copy(move_iterator<Type*>(const_cast<Type*>(pos+1)), move_iterator<Type*>(end()), begin()+dist);
        size_--;
        return &vector_[dist];
    }
    
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return vector_[index];
    }

    Type& At(size_t index) {
        if (index >= size_){
            throw out_of_range("You are out of range!"s);
        }
        return vector_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_){
            throw out_of_range("You are out of range!"s);
        }
        return vector_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_){
            size_ = new_size;
        } else if (new_size <= capacity_){
            size_t tmp = size_;
            for (size_t i = tmp; i < new_size; i++){
                vector_[i] = Type();
            }
            size_ = new_size;
        } else {
            ArrayPtr<Type> tmp(new_size);
            
            for (size_t i = 0; i < size_; i++){
                tmp[i] = std::move(vector_[i]);
            }
            for (size_t i = size_; i < new_size; i++){
                tmp[i] = Type();
            }
            
            vector_.swap(tmp);
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    // Возвращает итератор на начало массива
    Iterator begin() noexcept {
        return vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    Iterator end() noexcept {
        return vector_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    ConstIterator begin() const noexcept {
        return vector_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator end() const noexcept {
        return vector_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    ConstIterator cbegin() const noexcept {
        return const_cast<const Type*>(vector_.Get());
    }

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator cend() const noexcept {
        return const_cast<const Type*>(vector_.Get()+size_);
    }
    
private:
    ArrayPtr<Type> vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) return false;
    for (size_t i = 0; i < lhs.GetSize(); i++){
        if (lhs[i] != rhs[i]) return false;
    }
    return true;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs == rhs || lhs < rhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};
