#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "document.h"
#include "paginator.h"

std::string ReadLine();

int ReadLineWithNumber();

std::ostream& std::operator<<(std::ostream& out, const Document& document);

void PrintDocument(const Document& document);

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status);

template <typename Iterator>
std::ostream& std::operator<<(std::ostream& out, const IteratorRange<Iterator>& range) {
	for (Iterator it = range.begin(); it != range.end(); ++it) {
		out << *it;
	}
	return out;
}
