#pragma once

#include <vector>

template <typename Iterator>
class IteratorRange {
public:
	IteratorRange(Iterator begin, Iterator end)
		: first_(begin)
		, last_(end)
		, size_(distance(first_, last_)) {
	}

	Iterator begin() const {
		return first_;
	}

	Iterator end() const {
		return last_;
	}

	size_t size() const {
		return size_;
	}

private:
	Iterator first_, last_;
	std::size_t size_ = 0;
};

template <typename Iterator>
class Paginator {
using namespace std;
public:
	Paginator(Iterator begin, Iterator end, size_t page_size) {
		for (size_t left = distance(begin, end); left > 0;) {
			const size_t current_page_size = min(page_size, left);
			const Iterator current_page_end = next(begin, current_page_size);
			pages_.push_back({ begin, current_page_end });

			left -= current_page_size;
			begin = current_page_end;
		}
	}

	auto begin() const {
		return pages_.begin();
	}

	auto end() const {
		return pages_.end();
	}

	size_t size() const {
		return pages_.size();
	}

private:
	vector<IteratorRange<Iterator>> pages_;
};