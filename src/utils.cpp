//
// Created by bvasseur on 9/12/25.
//

# include "utils.hpp"

#include <algorithm>
#include <vector>

int maxIndex(std::vector<float> &lst) {
	return std::max_element(lst.begin(), lst.end()) - lst.begin();
}

int min(const int *lst, size_t size) {
	if (!lst || size == 0)
		return 0;
	int	min = lst[0];

	for (size_t i = 1; i < size; ++i) {
		if (lst[i] < min)
			min = lst[i];
	}
	return min;
}
