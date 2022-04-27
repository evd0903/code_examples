#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <utility>

using namespace std;

template <typename Type>
class SingleLinkedList {
public:
	using value_type = Type;
	using reference = value_type & ;
	using const_reference = const value_type&;
	using Iterator = BasicIterator<Type>;
	using ConstIterator = BasicIterator<const Type>;

	SingleLinkedList(std::initializer_list<Type> values) {
		vector<Type> g(values);
		size_ = 0;
		Assign(g.rbegin(), g.rend());
	}

	SingleLinkedList(const SingleLinkedList& other) {
		size_ = 0;
		vector<Type> g;
		for (auto value : other) {
			g.push_back(value);
		}
		SingleLinkedList tmp;
		tmp.Assign(g.rbegin(), g.rend());
		swap(tmp);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (this != &rhs) {
			auto tmp(rhs);
			swap(tmp);
		}
		return *this;
	}

	void swap(SingleLinkedList& other) noexcept {
		std::swap(size_, other.size_);
		std::swap(head_.next_node, other.head_.next_node);
	}

	SingleLinkedList() : size_(0)
	{

	}
	
	~SingleLinkedList() {
		Clear();
	}

	[[nodiscard]] Iterator begin() noexcept {
		return Iterator{ head_.next_node };

	}

	[[nodiscard]] Iterator end() noexcept {
		return Iterator{ nullptr };
	}

	[[nodiscard]] ConstIterator begin() const noexcept {
		return ConstIterator{ head_.next_node };
	}

	[[nodiscard]] ConstIterator end() const noexcept {
		return ConstIterator{ nullptr };
	}

	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return this->begin();
	}

	[[nodiscard]] ConstIterator cend() const noexcept {
		return this->end();
	}

	[[nodiscard]] Iterator before_begin() noexcept {
		return Iterator{&head_};
	}
    
    [[nodiscard]] ConstIterator before_begin() const noexcept {
		return ConstIterator{const_cast<Node*>(&head_)};
	}

	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return this->before_begin();    
	}

	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		assert(pos.node_ != nullptr);
		Node* node = new Node(value, pos.node_->next_node);
		pos.node_->next_node = node;
        size_++;
        return Iterator{node};
	}

	void PopFront() noexcept {
		assert(size_ != 0);
		assert(head_ != nullptr);
		Node* tmp = head_.next_node->next_node;
		delete head_.next_node;
		head_.next_node = tmp;
        size_--;
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		assert(size_ != 0);
		assert(pos.node_ != nullptr);
		Node* node = pos.node_->next_node->next_node;
		delete pos.node_->next_node;
		pos.node_->next_node = node;
        size_--;
        return Iterator{node};
	}

	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}

	[[nodiscard]] bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	void PushFront(const Type& value) {
		assert(head_ != nullptr);
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	void Clear() noexcept {
		while (size_--) {
			Node* next = head_.next_node->next_node;
			delete head_.next_node;
			head_.next_node = next;

		}
		size_ = 0;
	}
	
public:
	template <typename ValueType>
	class BasicIterator {
		friend class SingleLinkedList;
		
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Type;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType * ;
		using reference = ValueType & ;

		BasicIterator() = default;

		BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_)
		{

		}

		BasicIterator& operator=(const BasicIterator& rhs) = default;

		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		BasicIterator& operator++() noexcept {
			node_ = node_->next_node;
			return *this;
		}

		BasicIterator operator++(int) noexcept {
			BasicIterator now = *this;
			++(*this);
			return now;
		}

		[[nodiscard]] reference operator*() const noexcept {
			return node_->value;
		}

		[[nodiscard]] pointer operator->() const noexcept {
			return &(node_->value);
		}
		
	private:		
		explicit BasicIterator(Node* node) : node_(node)
		{

		}

	private:
		Node* node_ = nullptr;
	};
	
private:
	template <typename InputIterator>
	void Assign(InputIterator from, InputIterator to){
		SingleLinkedList tmp;
		for (auto it = from; it != to; ++it){
			tmp.PushFront(*it);
		}
		swap(tmp);
	}

private:
	struct Node {
		Node() = default;
		
		Node(const Type& val, Node* next) : value(val), next_node(next) 
		{
		
		}
		
		Type value;
		Node* next_node = nullptr;
	};

private:
	Node head_ = nullptr;
	size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return  rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs);
}

int main() {
	return 0;
} 
