#include "JsonParameter.hpp"
#include <fstream>
#include <iostream>

int Parameters::L0 = 1;
double Parameters::mm = 1.0;
double Parameters::nn = 1.0;
double Parameters::R0 = 4.4364;
double Parameters::Eta0 = 1e-6;
double Parameters::dr = 1.0 / Nr;
double Parameters::rs = 0.3;
double Parameters::q0 = 0.9;
double Parameters::Ew = 2 * 1e-6 / 0.9;
double Parameters::simT = 500;
double Parameters::savT = 50;
double Parameters::dt = 0.01;
int Parameters::count = 0;
double Parameters::Time = 0;
double Parameters::r0 = 0;
double Parameters::r[Nr + 1];
double Parameters::eta[Nr + 1];

void Parameters::load(const std::string& jsonFilePath) {
    std::ifstream i(jsonFilePath);
    if (!i.is_open()) {
        throw std::runtime_error("Cannot open JSON file.");
    }
    nlohmann::json j;
    i >> j;

    L0 = j["L0"].get<int>();
    mm = j["mm"].get<double>();
    nn = j["nn"].get<double>();
    R0 = j["R0"].get<double>();
    Eta0 = j["Eta0"].get<double>();
    dr = j["dr"].get<double>();
    rs = j["rs"].get<double>();
    q0 = j["q0"].get<double>();
    Ew = j["Ew"].get<double>();
    simT = j["simT"].get<double>();
    savT = j["savT"].get<double>();
    dt = j["dt"].get<double>();
    count = j["count"].get<int>();
    Time = j["Time"].get<double>();
    r0 = j["r0"].get<double>();
    for (int i = 0; i <= Nr; ++i) {
        r[i] = j["r"][i].get<double>();
        eta[i] = j["eta"][i].get<double>();
    }
}

void Parameters::save(const std::string& jsonFilePath) {
    nlohmann::json j;
    j["L0"] = L0;
    j["mm"] = mm;
    j["nn"] = nn;
    j["R0"] = R0;
    j["Eta0"] = Eta0;
    j["dr"] = dr;
    j["rs"] = rs;
    j["q0"] = q0;
    j["Ew"] = Ew;
    j["simT"] = simT;
    j["savT"] = savT;
    j["dt"] = dt;
    j["count"] = count;
    j["Time"] = Time;
    j["r0"] = r0;
    for (int i = 0; i <= Nr; ++i) {
        j["r"][i] = r[i];
        j["eta"][i] = eta[i];
    }

    std::ofstream o(jsonFilePath);
    if (!o.is_open()) {
        throw std::runtime_error("Cannot open JSON file.");
    }
    o << std::setw(4) << j << std::endl;
}