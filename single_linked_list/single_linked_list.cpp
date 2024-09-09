#include <algorithm> 
#include <cassert> 
#include <cstddef> 
#include <initializer_list> 
#include <iterator> 
#include <string> 
#include <utility> 
 
template <typename Type> 
class SingleLinkedList { 
     
    struct Node { 
        Node() = default; 
        Node(const Type& val, Node* next) 
            : value(val) 
            , next_node(next) { 
        } 
        Type value; 
        Node* next_node = nullptr; 
    }; 
     
    template <typename ValueType> 
    class BasicIterator { 
         
        friend class SingleLinkedList; 
         
        explicit BasicIterator(Node* node) : node_(node) { 
        } 
         
    public: 
         
        using iterator_category = std::forward_iterator_tag; 
        using value_type = Type; 
        using difference_type = std::ptrdiff_t; 
        using pointer = ValueType*; 
        using reference = ValueType&; 
 
        BasicIterator() = default; 
 
        BasicIterator(const BasicIterator<Type>& other) noexcept { 
                    node_ = other.node_; 
        } 
         
        BasicIterator& operator=(const BasicIterator& rhs) = default; 
 
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept { 
            return node_ == rhs.node_; 
        } 
 
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept { 
            return node_ != rhs.node_; 
        } 
 
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept { 
            return node_ == rhs.node_; 
        } 
 
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept { 
            return node_ != rhs.node_; 
        } 
 
        BasicIterator& operator++() noexcept { 
            assert(node_ != nullptr); 
            Node* next = node_->next_node; 
            node_ = next; 
            return *this; 
        } 
 
        BasicIterator operator++(int) noexcept { 
            auto old_value(*this); 
            ++(*this); 
            return old_value; 
        } 
 
        [[nodiscard]] reference operator*() const noexcept { 
            assert(node_ != nullptr); 
            return node_->value; 
        } 
 
        [[nodiscard]] pointer operator->() const noexcept { 
            assert(node_ != nullptr); 
            return &(node_->value); 
        } 
 
    private: 
        Node* node_ = nullptr; 
    }; 
     
public: 
     
    using value_type = Type; 
    using reference = value_type&; 
    using const_reference = const value_type&; 
    using Iterator = BasicIterator<Type>; 
    using ConstIterator = BasicIterator<const Type>; 
     
    [[nodiscard]] Iterator begin() noexcept { 
        return Iterator{head_.next_node}; 
    } 
 
    [[nodiscard]] Iterator end() noexcept { 
        Node* last = LastNode(); 
        return Iterator{last}; 
    } 
 
    [[nodiscard]] ConstIterator cbegin() const noexcept { 
        return ConstIterator{head_.next_node}; 
    } 
 
    [[nodiscard]] ConstIterator begin() const noexcept { 
        return this->cbegin(); 
    } 
 
    [[nodiscard]] ConstIterator cend() const noexcept { 
        Node* last = LastNode(); 
        return ConstIterator{last}; 
    } 
 
    Node* LastNode() const { 
        Node* first = head_.next_node; 
        while(first != nullptr){ 
            Node* second = first->next_node; 
            first = second; 
        } 
        return first; 
    } 
     
    [[nodiscard]] ConstIterator end() const noexcept { 
        return this->cend(); 
    } 
     
    SingleLinkedList(){} 
     
    ~SingleLinkedList() { 
        Clear(); 
    } 
 
    [[nodiscard]] size_t GetSize() const noexcept { 
        return size_; 
    } 
 
    [[nodiscard]] bool IsEmpty() const noexcept { 
        return size_ == 0; 
    } 
 
    void PushFront(const Type& value) { 
        head_.next_node = new Node(value, head_.next_node); 
        ++size_; 
    } 
     
    void Clear() noexcept { 
         
        while (head_.next_node != nullptr) { 
            Node* tmp = head_.next_node; 
            head_.next_node = tmp->next_node; 
            delete tmp; 
        } 
        size_ = 0; 
    } 
     
    template <typename InputIt> 
    SingleLinkedList(InputIt first, InputIt last) { 
        assert(size_ == 0 && head_.next_node == nullptr); 
 
        SingleLinkedList tmp; 
        Node* current = &tmp.head_; 
 
        for (InputIt it = first; it != last; ++it) { 
            Node* new_node = new Node(*it, nullptr); 
            current->next_node = new_node; 
            current = new_node; 
        } 
         
        tmp.size_ = std::distance(first, last); 
        swap(tmp); 
    } 
     
    SingleLinkedList(std::initializer_list<Type> values) 
        : SingleLinkedList(values.begin(), values.end()) { 
        } 
 
    // Конструктор копирования 
    SingleLinkedList(const SingleLinkedList& other) 
        : SingleLinkedList(other.begin(), other.end()) { 
        } 
     
    SingleLinkedList& operator=(const SingleLinkedList& rhs) { 
        if (this != &rhs){ 
            auto tmp(rhs); 
            swap(tmp); 
        } 
        return *this; 
    } 
 
    void swap(SingleLinkedList& other) noexcept { 
        std::swap(head_.next_node, other.head_.next_node); 
        std::swap(size_, other.size_); 
    } 
 
    [[nodiscard]] Iterator before_begin() noexcept { 
        return Iterator(&head_); 
    } 
 
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept { 
        return ConstIterator{const_cast<Node*>(&head_)}; 
    } 
 
    [[nodiscard]] ConstIterator before_begin() const noexcept { 
        return ConstIterator(&head_); 
    } 
     
    Iterator InsertAfter(ConstIterator pos, const Type& value) { 
        assert(pos.node_ != nullptr); 
        Node* new_node = new Node(value, pos.node_->next_node); 
        pos.node_->next_node = new_node; 
        ++size_; 
        return Iterator(new_node); 
    } 
 
    void PopFront() noexcept { 
        if (head_.next_node != nullptr) { 
            Node* to_delete = head_.next_node; 
            head_.next_node = head_.next_node->next_node; 
            delete to_delete; 
            --size_; 
        } 
    } 
 
    // Удаляет элемент после pos 
    Iterator EraseAfter(ConstIterator pos) noexcept { 
        assert(pos.node_ != nullptr); 
        if (pos.node_->next_node != nullptr) { 
            Node* to_delete = pos.node_->next_node; 
            pos.node_->next_node = to_delete->next_node; 
            delete to_delete; 
            --size_; 
            return Iterator(pos.node_->next_node); 
        } 
        return Iterator(nullptr); 
    } 
     
private: 
    Node head_; 
    size_t size_ = 0; 
}; 
 
template <typename Type> 
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept { 
    lhs.swap(rhs); 
} 
 
template <typename Type> 
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin()); 
} 
 
template <typename Type> 
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return !(lhs == rhs); 
} 
 
template <typename Type> 
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return std::lexicographical_compare(lhs.cbegin(),lhs.cend(),rhs.cbegin(), rhs.cend()); 
} 
 
template <typename Type> 
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return !(rhs < lhs); 
} 
 
template <typename Type> 
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return rhs < lhs; 
} 
 
template <typename Type> 
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) { 
    return !(lhs < rhs); 
} 
