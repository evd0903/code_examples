#include "remove_duplicates.h"

#include<iterator>

using namespace std;

void RemoveDuplicates(SearchServer& search_server){
	map<set<string>, int> existing_word_sets;
    for (auto document_id : search_server){
		auto words_with_freq = search_server.GetWordFrequencies(document_id);	
		set<string> words;
		transform(words_with_freq.begin(), words_with_freq.end(), inserter(words, words.begin()), [](auto elem) {return elem.first; });
		if (existing_word_sets.find(words) != existing_word_sets.end()){
			search_server.RemoveDocument(document_id);	
		}
		else{
			existing_word_sets[words] = document_id;
		}
	}
}
