#pragma once

#include "transport_catalogue.h"

namespace project {

namespace detail {

std::vector<std::string_view> Split(std::string_view text, char delim);

struct CommandInfo {
	std::string type;
	std::string name;
	std::vector<std::string> stops;
	bool is_round = false;
	project::detail::Coordinates coordinates;
	std::unordered_map<std::string, int> dists;
};

class InputReader {
public:
	void Read();

	void ReadStop(std::string text);

	void ReadBus(std::string text);

	CommandInfo ParseStop(std::string text);

	Coordinates ParseCoordinates(std::string text);

	std::unordered_map<std::string, int> ParseDists(std::string text);

	CommandInfo ParseBus(std::string text);

	std::vector<std::string> ParseStopsAtBus(std::string);

	void Load(server::TransportCatalogue& db);

	void LoadCommand(server::TransportCatalogue& db, CommandInfo command);

	void LoadDists(server::TransportCatalogue& db, CommandInfo command);

private:
	std::vector<CommandInfo> commands_;
};

}

}
