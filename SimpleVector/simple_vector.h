#pragma once

#include <cassert>
#include <initializer_list>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <utility>
#include <iostream>

#include "array_ptr.h"

class ReserveProxyObj{
public:
    ReserveProxyObj(size_t capacity) : capacity_(capacity)
    {
    }
    
    size_t GetCapacity() 
	{
        return capacity_;
    }
    
private:
    size_t capacity_ = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
	{
		SimpleVector(size, Type{});
	}

    SimpleVector(size_t size, const Type& value) : items_(size), size_(size), capacity_(size)
    {
        std::fill(items_.Get(), items_.Get() + size, value);
    }

    SimpleVector(std::initializer_list<Type> init) : items_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }
    
    SimpleVector(ReserveProxyObj rhs)
	{
        Reserve(rhs.GetCapacity());
    }
    
    SimpleVector(const SimpleVector& other) : items_(other.size_), size_(other.size_), capacity_(other.size_)
    {
        std::copy(other.begin(), other.end(), items_.Get());
    }
    
    SimpleVector(SimpleVector&& other) : items_(other.size_)
    {
        std::move(other.begin(), other.end(), items_.Get());
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector& operator=(const SimpleVector& rhs) 
	{
        if (this != &rhs){
            auto tmp(rhs);
            swap(tmp);
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) 
	{
        auto tmp(std::move(rhs));
        items_.swap(tmp.items_);
        size_ = std::exchange(tmp.size_, 0);
        capacity_ = std::exchange(tmp.capacity_, 0);
        return *this;
    }
    
    void swap(SimpleVector& other) noexcept 
	{
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept 
	{
        return size_;
    }

    size_t GetCapacity() const noexcept 
	{
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    Type& operator[](size_t index) noexcept 
	{
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept 
	{
        return items_[index];
    }

    Type& At(size_t index) 
	{
        if (index >= size_)
            throw std::out_of_range("Out of range"s);
        return items_[index];
    }

    const Type& At(size_t index) const 
	{
        if (index >= size_)
            throw std::out_of_range("Out of range"s);
        return items_[index];
    }

    void Clear() noexcept 
	{
        size_ = 0;
    }

    void Resize(size_t new_size) 
	{
        if (new_size > capacity_){
            auto new_items = ReallocateCopy(new_size);
			std::fill(&new_items[size_], &new_items[new_size], Type{});
			items_.swap(new_items);
			size_ = new_size;
			capacity_ = new_size * 2;
        }
        else {
            std::fill(&items_[size_], &items_[new_size], Type{});
			size_ = new_size;
		}
		
    }

    Iterator begin() noexcept 
	{
        return items_.Get();
    }

    Iterator end() noexcept 
	{
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept 
	{
        return items_.Get();
    }

    ConstIterator end() const noexcept 
	{
        return items_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept 
	{
        return begin();
    }

    ConstIterator cend() const noexcept 
	{
        return end();
    }
    
    void PushBack(const Type& item) 
	{
		size_t new_size = size_ + 1;
		if (new_size > capacity_){
			size_t new_capacity = std::max(new_size, capacity_ * 2);
			auto new_items = ReallocateCopy(new_capacity);
			new_items[size_] = value;
			items_.swap(new_items);
			capacity_ = new_capacity;
		}
		else {
			items_[size_] = value;
		}
        size_ = new_size;
    }
    
    void PushBack(Type&& item) 
	{
        size_t new_size = size_ + 1;
		if (new_size > capacity_){
			size_t new_capacity = std::max(new_size, capacity_ * 2);
			auto new_items = ReallocateCopy(new_capacity);
			new_items[size_] = std::move(value);
			items_.swap(new_items);
			capacity_ = new_capacity;
		}
		else {
			items_[size_] = std::move(value);
		}
        size_ = new_size;
    }

    Iterator Insert(ConstIterator pos, const Type& value) 
	{
		assert(pos != nullptr);
		size_t new_size = size_ + 1;
        auto offset = pos - &items_[0];
		Iterator mutable_pos = begin() + offset;
		if (new_size <= capacity_){
			std::copy_backward(mutable_pos, end(), end() + 1);
			*mutable_pos = value;
		}
		else {
			size_t new_capacity = std::max(new_size, capacity_ * 2);
			ArrayPtr<Type> ItemsPtr(new_capacity);
			std::copy(begin(), mutable_pos, &ItemsPtr[0]);
			ItemsPtr[offset] = value;
			std::copy(mutable_pos, end(), &ItemsPtr[offset + 1]);
			items_.swap(ItemsPtr);
			capacity_ = new_capacity;
		}
		size_ = new_size;
		return mutable_pos;
    }

    Iterator Insert(ConstIterator pos, Type&& value) 
	{
		assert(pos != nullptr);
		size_t new_size = size_ + 1;
        auto offset = pos - &items_[0];
		Iterator mutable_pos = begin() + offset;
		if (new_size <= capacity_){
			std::move_backward(mutable_pos, end(), end() + 1);
			items_[offset] = std::move(value);
		}
		else {
			size_t new_capacity = std::max(new_size, capacity_ * 2);
			ArrayPtr<Type> ItemsPtr(new_capacity);
			std::move(begin(), mutable_pos, &ItemsPtr[0]);
			ItemsPtr[offset] = std::move(value);
			std::move(mutable_pos, end(), &ItemsPtr[offset + 1]);
			items_.swap(ItemsPtr);
			capacity_ = new_capacity;
		}
		size_ = new_size;
		return mutable_pos;
    }
    
    void PopBack() noexcept 
	{
        if (size_) {
            --size_;
		}
    }

    Iterator Erase(ConstIterator pos) 
	{
        auto p = pos - &items_[0];
        std::move(std::make_move_iterator(&items_[p + 1]), std::make_move_iterator(&items_[size_]), &items_[p]);
        size_--;
        return &items_[p];
    }
    
    void Reserve(size_t new_capacity)
	{
        if (new_capacity > capacity_) {
            auto new_items =  ReallocateCopy(new_capacity);
            std::copy(&items_[0], &items_[size_], &new_items[0]);
            items_.swap(new_items);
            capacity_ = new_capacity;
        }
    }
	
private:
	template <typename Type>
	ArrayPtr<Type> ReallocateCopy(size_t new_capacity)
	{
		size_t move_size = std::min(new_capacity, size_);
		ArrayPtr<Type> tmp(move_size);
		std::move(&items_[0], &items_[move_size], &tmp[0]);
		return tmp;
	}
        
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(lhs < rhs);
} 

ReserveProxyObj Reserve(size_t capacity_to_reserve) 
{
    return ReserveProxyObj(capacity_to_reserve);
}
