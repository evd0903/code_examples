#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include <vector>
#include <iostream>
#include <string>
#include <string_view>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"

namespace project {

namespace server {

namespace detail {

struct Bus {
	std::string name;
	std::vector<std::string> stops;
	bool is_round = false;
};

struct Stop {
	std::string name;
	project::detail::Coordinates coordinates;
};


class PairStopHasher {
public:
	size_t operator()(const std::pair<Stop*, Stop*> p) const {
		return hasher_(p.first) + hasher_(p.second) * 37;
	}
private:
	std::hash<const void*> hasher_;
};

struct BusInfo {
	std::string name;
	size_t stops_on_route = 0;
	size_t unique_stops = 0;
	double route_lenght = 0.0;
	double curvature = 0.0;
};

struct StopInfo {
	std::string name;
	std::set<std::string> buses;
	bool valid = false;
};

}

using namespace server::detail;

class TransportCatalogue {
public:
	void AddBus(Bus&& bus);

	void AddStop(Stop&& stop);

	void AddDist(Stop* stop_a, Stop* stop_b, int dist);

	double GetDist(Stop* stop_a, Stop* stop_b);

	Bus* FindBus(std::string_view name);

	Stop* FindStop(std::string_view name);

	BusInfo FindBusInfo(std::string_view name);

	StopInfo FindStopInfo(std::string_view name);

private:
	std::unordered_map<std::string_view, Stop*> stops_by_name_;
	std::unordered_map<std::string_view, Bus*> buses_by_name_;
	std::unordered_map<std::string_view, std::set<std::string>> buses_to_stops_;
	std::unordered_map<std::pair<Stop*, Stop*>, int, PairStopHasher> dist_between_stops_;
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
};

}

}
