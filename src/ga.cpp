#include "ga.hpp"
#include <algorithm>
#include <random>

namespace {
std::mt19937 gen(0); //Standard mersenne_twister_engine seeded with fixed seed for replicability
};

matching get_valid_point(std::vector<matching> matches, size_t max_size1, size_t max_size2) {
    std::uniform_int_distribution<size_t> distribution(0, max_size1 - 1);
    size_t p1, p2;
	unsigned int tries = 0;
    bool valid;
	size_t turn = max_size2 - 1;

	for (size_t i = 0; i < matches.size()-1; ++i) {
		if (matches[i].second > matches[i+1].second) {
			turn = i;
			break;
		}
	}

    do {
        valid = true;
        p1 = distribution(gen);
        
        size_t i = 0;
        while (matches[i].first <= p1 && i <= matches.size()-1) {
            ++i;
        }
        size_t prev, next;
        
        if (matches[i-1].first == p1) { //If repeated, not valid
            valid = false;
            continue;
        } 

        if (i == matches.size() && matches[i-1].second == matches[0].second-1) { //If this should be the last but no space
            valid = false;
            continue;
        } else if (matches[i].second == matches[i-1].second + 1) { //There is no space
            valid = false;
            continue;
        }

        prev = (i != 0? matches[i-1].second : matches[matches.size()-1].second);
        next = (i == matches.size()? matches[0].second : matches[i].second);

        if (next >= prev) { //No rotation of pixels
            if (next <= prev + 1) {
                valid = false;
                continue;
            }
            std::uniform_int_distribution<size_t> distribution2(prev+1, next-1);
            p2 = distribution2(gen);
        } else if (i == turn + 1) {
            size_t sample_size = (max_size2 - prev - 1) + (next);
            if (sample_size == 0) {
                valid = false; 
                continue;
            }
            std::uniform_int_distribution<size_t> distribution2(1, sample_size);
            size_t rand = distribution2(gen);
			p2 = rand + prev;
			if (p2 >= max_size2) p2 -= max_size2;
            /*if (rand < (max_size2 - prev - 1)) {
                //Until the end of shape
                p2 = rand + prev + 1;
            } else {
                p2 = rand - (max_size2 - prev);
            }*/
        } else {
			valid = false;
			continue;
		}
    } while (!valid && tries++ < 100);

	if (!valid && tries >= 100) //Not valid. We tried 100 times.
		return std::make_pair(max_size1, max_size2);
    return std::make_pair(p1, p2);
}
