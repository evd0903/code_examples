#include "input_reader.h"

using namespace std;

namespace project {

namespace detail {

std::vector<std::string_view> Split(std::string_view text, char delim) {
	std::vector<std::string_view> result;

	while (true) {
		size_t space_pos = text.find(delim);
		result.push_back(text.substr(0, space_pos));
		if (space_pos == text.npos) {
			break;
		}
		text.remove_prefix(space_pos + 1);
	}

	return result;
}

void InputReader::Read() {
	int cnt_q;
	cin >> cnt_q;
	string s;
	getline(cin, s);
	for (size_t i = 0; i < cnt_q; ++i) {
		getline(cin, s);
		if (s.substr(0, 3) == "Bus"s) {
			ReadBus(s.substr(4));
		}
		else {
			ReadStop(s.substr(5));
		}
	}
}

void InputReader::ReadStop(string text) {
	commands_.push_back(ParseStop(text));
	
}

CommandInfo InputReader::ParseStop(std::string text) {
	CommandInfo stop;
	stop.type = "stop"s;
	auto pos = find(text.begin(), text.end(), ':');
	stop.name = { text.begin(), pos };
	stop.coordinates = ParseCoordinates(text.substr(pos - text.begin() + 2, text.size()));
	stop.dists = ParseDists(text);
	return stop;
}

Coordinates InputReader::ParseCoordinates(std::string text) {
	auto vec = Split(text, ',');
	return { stod(string{vec[0]}), stod(string{vec[1]}) };
}

std::unordered_map<std::string, int> InputReader::ParseDists(std::string text) {
	std::unordered_map<std::string, int> result;
	auto vec = Split(text, ',');
	for (size_t i = 2; i != vec.size(); ++i) {
		auto pos = vec[i].find("to"s);
		result[string{ vec[i].substr(pos + 3) }] = stoi(string{ vec[i].substr(0, pos - 1) });
	}
	return result;
}

void InputReader::ReadBus(string text) {
	commands_.push_back(ParseBus(text));
}


CommandInfo InputReader::ParseBus(std::string text) {
	CommandInfo bus;
	bus.type = "bus"s;
	auto pos = find(text.begin(), text.end(), ':');
	bus.name = { text.begin(), pos };
	bus.stops = ParseStopsAtBus({ pos + 1, text.end() });
	if (text.find('>') != text.npos) {
		bus.is_round = true;
	}
	return bus;
}

std::vector<std::string> InputReader::ParseStopsAtBus(std::string text)
{
	vector<string> parsed_text;
	text += " ";
	if (text.find('>') != text.npos) {
		for (auto i : Split(text, '>')) {
			parsed_text.push_back(string{ i }.substr(1, i.size() - 2));
		}
	}
	else {
		for (auto i : Split(text, '-')) {
			parsed_text.push_back(string{ i }.substr(1, i.size() - 2));
		}
	}
	return parsed_text;
}

void InputReader::Load(server::TransportCatalogue & db) {
	partition(commands_.begin(), commands_.end(), [](CommandInfo item) {
		return item.type == "stop"s;
	});
	for (auto& item : commands_) {
		LoadCommand(db, item);
	}
	for (auto& item : commands_) {
		LoadDists(db, item);
	}
}

void InputReader::LoadCommand(server::TransportCatalogue & db, CommandInfo command) {
	if (command.type == "stop"s) {
		db.AddStop({ command.name, command.coordinates });
	}
	else {
		db.AddBus({ command.name, command.stops, command.is_round });
	}
}

void InputReader::LoadDists(server::TransportCatalogue& db, CommandInfo command) {
	if (command.type == "stop"s) {
		for (auto&[key, value] : command.dists) {
			db.AddDist(db.FindStop(command.name), db.FindStop(key), value);
		}
	}
}

}

}
