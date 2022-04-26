#include "request_queue.h"
#include "search_server.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) : server_(search_server)
{
		
}

template <typename DocumentPredicate>
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
	auto request = server_.FindTopDocuments(raw_query, document_predicate);
	if (requests_.size() == kMinsInDay) {
		if (requests_.front().req_size == 0){
			no_result_requests_--;
		}
		requests_.pop_front();
	}
	if (request.empty()){
		no_result_requests_++;
	}
	requests_.push_back((raw_query, static_cast<int>(request.size())));
	return request;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
	return RequestQueue::AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
		return document_status == status;
	});
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
	return RequestQueue::AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
	return no_result_requests_;
}
		
RequestQueue::QueryResult::QueryResult(const string& query, const int size) : request(query), req_size(size)
{

}
