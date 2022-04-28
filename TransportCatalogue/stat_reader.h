#pragma once

#include <iomanip>

#include "transport_catalogue.h"

namespace project {

namespace detail {

void Output(server::TransportCatalogue& db);

void OutputStopInfo(server::TransportCatalogue& db, std::string& text);

void OutputBusInfo(server::TransportCatalogue& db, std::string& text);

}

}
