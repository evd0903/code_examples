#include "input_reader.h"
#include "stat_reader.h"

using namespace project;

int main() {
	server::TransportCatalogue a;
	detail::InputReader r;
	r.Read();
	r.Load(a);
	detail::Output(a);
	return 0;
}
