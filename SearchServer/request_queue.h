#pragma once

#include <deque>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
	explicit RequestQueue(const SearchServer& search_server);
    
	template <typename DocumentPredicate>
	std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

	std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

	std::vector<Document> AddFindRequest(const std::string& raw_query);

	int GetNoResultRequests() const;
    
private:
	struct QueryResult{
        QueryResult(const std::string& query, const int size);
         
        std::string request;
		int req_size = 0;
    };
	
private:
	static const int kMinsInDay = 1440;
	
private:
	const SearchServer& server_;
	std::deque<QueryResult> requests_;
	int no_result_requests_ = 0;
};
