#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <utility>
template <typename Type>
class ArrayPtr {
public:
    
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0){
            raw_ptr_ = nullptr;
        } else {
            Type* ptr = new Type[size];
            raw_ptr_ = ptr;
        }
    }
    
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;
    
    ArrayPtr(ArrayPtr&& other) : raw_ptr_(other.raw_ptr_)
    {
        other.raw_ptr_ = nullptr;
    }
    
    ArrayPtr& operator=(ArrayPtr&& right) {
        delete[] raw_ptr_;
        raw_ptr_ = right.raw_ptr_;
        right.raw_ptr_ = nullptr;
        return *this;
    }
    
    ~ArrayPtr() {
        delete[] raw_ptr_;
    }
    
    [[nodiscard]] Type* Release() noexcept {
        auto tmp = raw_ptr_;
        raw_ptr_ = nullptr;
        return tmp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_+index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_+index);
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }
    
private:
    Type* raw_ptr_ = nullptr;
};
