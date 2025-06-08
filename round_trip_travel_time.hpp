#ifndef ROUND_TRIP_TRAVEL_TIME_HPP
#define ROUND_TRIP_TRAVEL_TIME_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>

// this class measures the round trip travel time of packets
class RoundTripTravelTime {
  public:
    explicit RoundTripTravelTime(size_t hist_size = 100) : history_size(hist_size) {}

    void register_send(unsigned int packet_id) {
        auto current_time = std::chrono::high_resolution_clock::now();
        packet_id_to_send_time[packet_id] = current_time;
    }

    void register_receive(unsigned int packet_id) {
        auto current_time = std::chrono::high_resolution_clock::now();

        auto it = packet_id_to_send_time.find(packet_id);
        if (it != packet_id_to_send_time.end()) { // NOTE: we always send before reciving so this is guarenteed to run
            auto send_time = it->second;
            auto round_trip_time_ms =
                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - send_time).count();

            round_trip_times_ms.push_back(round_trip_time_ms);

            // if the history exceeds the limit, truncate it
            if (round_trip_times_ms.size() > history_size) {
                round_trip_times_ms.erase(round_trip_times_ms.begin());
            }
        } else {
            std::cerr << "Packet ID " << packet_id << " not found in sent packets." << std::endl;
        }
    }

    double get_average_round_trip_time_ms() const {
        if (round_trip_times_ms.empty()) {
            return 0.0;
        }

        double sum = 0;
        for (double time : round_trip_times_ms) {
            sum += time;
        }

        return sum / round_trip_times_ms.size();
    }

  private:
    size_t history_size;
    std::unordered_map<unsigned int, std::chrono::high_resolution_clock::time_point> packet_id_to_send_time;
    std::vector<double> round_trip_times_ms; // Store the round trip times
};

#endif // ROUND_TRIP_TRAVEL_TIME_HPP
