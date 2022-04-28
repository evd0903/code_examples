#include "stat_reader.h"

using namespace std;

namespace project {

namespace detail {

void Output(server::TransportCatalogue& db) {
	size_t cnt_q;
	cin >> cnt_q;
	string q;
	getline(cin, q);

	for (size_t i = 0; i != cnt_q; ++i) {
		getline(cin, q);
		if (q.front() == 'B') {
			OutputBusInfo(db, q);
		}
		else {
			OutputStopInfo(db, q);
		}
	}
}

void OutputStopInfo(server::TransportCatalogue & db, string& text) {
	string name = text.substr(5);
	auto ans = db.FindStopInfo(name);

	cout << "Stop "s << name << ": "s;
	if (!ans.valid) {
		cout << "not found"s << endl;
	}
	else {
		if (!ans.buses.size()) {
			cout << "no buses"s << endl;
		}
		else {
			cout << "buses "s;
			for (auto& bus_name : ans.buses) {
				cout << bus_name << " "s;
			}
			cout << endl;
		}
	}
}

void OutputBusInfo(server::TransportCatalogue & db, string& text) {
	string name = text.substr(4);
	auto ans = db.FindBusInfo(name);

	cout << "Bus "s << name << ": "s;
	if (ans.stops_on_route == 0) {
		cout << "not found"s << endl;
	}
	else {
		cout << ans.stops_on_route << " stops on route, "s << ans.unique_stops << " unique stops, "s <<
			setprecision(6) << ans.route_lenght << " route length, "s << setprecision(6) << ans.curvature << " curvature"s << endl;
	}
}

}

}
