#include "transport_catalogue.h"

using namespace std;

namespace project {

namespace server {

void TransportCatalogue::AddBus(Bus&& bus) {
	buses_.push_back(bus);
	buses_by_name_[buses_.back().name] = &buses_.back();
	for (auto& i : buses_.back().stops) {
		buses_to_stops_[i].insert(buses_.back().name);
	}
}

void TransportCatalogue::AddStop(Stop&& stop) {
	stops_.push_back(stop);
	stops_by_name_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddDist(Stop* stop_a, Stop* stop_b, int dist) {
	dist_between_stops_[{stop_a, stop_b}] = dist;
}

double TransportCatalogue::GetDist(Stop* stop_a, Stop* stop_b) {
	if (dist_between_stops_.count({ stop_a, stop_b })) {
		return static_cast<double>(dist_between_stops_[{stop_a, stop_b}]);
	}
	if (dist_between_stops_.count({ stop_b, stop_a })) {
		return static_cast<double>(dist_between_stops_[{stop_b, stop_a}]);
	}
	else {
		return project::detail::ComputeDistance(stop_a->coordinates, stop_b->coordinates);
	}
}

Bus* TransportCatalogue::FindBus(std::string_view name) {
	if (!buses_by_name_.count(name)) {
		return nullptr;
	}
	return buses_by_name_[name];
}

Stop* TransportCatalogue::FindStop(std::string_view name) {
	if (!stops_by_name_.count(name))
		return nullptr;
	return stops_by_name_[name];
}

BusInfo TransportCatalogue::FindBusInfo(std::string_view name) {
	auto bus = FindBus(name);
	if (!buses_by_name_.count(name) || bus == nullptr) {
		return { string{name}, 0, 0, 0 };
	}
	size_t stops_on_route = bus->stops.size();
	unordered_set<string> st;
	double route_len = 0.0;
	double geo_len = 0.0;
	for (size_t i = 0; i != bus->stops.size(); ++i) {
		st.insert(bus->stops[i]);
		if (i) {
			route_len += GetDist(FindStop(bus->stops[i - 1]), FindStop(bus->stops[i]));
			geo_len += project::detail::ComputeDistance(FindStop(bus->stops[i])->coordinates, FindStop(bus->stops[i - 1])->coordinates);
		}
	}
	if (!bus->is_round) {
		route_len += GetDist(FindStop(bus->stops.back()), FindStop(bus->stops.back()));
		for (size_t i = bus->stops.size() - 1; i > 0; --i) {
			st.insert(bus->stops[i]);
			if (i) {
				route_len += GetDist(FindStop(bus->stops[i]), FindStop(bus->stops[i - 1]));

			}
		}
		geo_len *= 2;
		stops_on_route *= 2;
		--stops_on_route;
	}
	if (bus->is_round) {
		if (bus->stops.front() != bus->stops.back()) {
			++stops_on_route;
			geo_len += project::detail::ComputeDistance(FindStop(bus->stops.back())->coordinates, FindStop(bus->stops.front())->coordinates);
			route_len += GetDist(FindStop(bus->stops.back()), FindStop(bus->stops.front()));
		}
	}
	size_t unique_stops = st.size();
	return { bus->name, stops_on_route, unique_stops, route_len, route_len / geo_len };
}

StopInfo TransportCatalogue::FindStopInfo(std::string_view name) {
	if (!stops_by_name_.count(name)) {
		return { string{name}, {}, false };
	}
	if (!buses_to_stops_.count(name)) {
		return { string{name}, {}, true };
	}
	return { string{name}, buses_to_stops_[name], true };
}

}

}
