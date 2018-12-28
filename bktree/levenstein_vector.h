//
// Created by vlad on 26.12.18.
//

#ifndef BKTREE_LEVENSTEIN_VECTOR_H
#define BKTREE_LEVENSTEIN_VECTOR_H
#include <algorithm>
#include <vector>

namespace MB {
    template<typename T>
    class levenshtein_distance_vec {
        mutable std::vector<size_t> costs_;

    public:
        explicit levenshtein_distance_vec(size_t initial_size = 8)
                : costs_(initial_size) {}

        size_t operator()(const std::vector<T> &s1, const std::vector<T> &s2) const {
            const size_t m(s1.size());
            const size_t n(s2.size());

            if (m == 0) return n;
            if (n == 0) return m;

            if (costs_.size() < std::max(n, m))
                costs_.resize(std::max(n, m));

            for (size_t k = 0; k <= n; k++) costs_[k] = k;

            size_t i = 0;
            for (auto it1 = s1.begin(); it1 != s1.end(); ++it1, ++i) {
                costs_[0] = i + 1;
                size_t corner = i;

                size_t j = 0;
                for (auto it2 = s2.begin(); it2 != s2.end(); ++it2, ++j) {
                    size_t upper = costs_[j + 1];
                    if (*it1 == *it2) {
                        costs_[j + 1] = corner;
                    } else {
                        size_t t(upper < corner ? upper : corner);
                        costs_[j + 1] = (costs_[j] < t ? costs_[j] : t) + 1;
                    }

                    corner = upper;
                }
            }

            size_t result = costs_[n];

            return result;
        };
    };

};  // namespace MB
#endif //BKTREE_LEVENSTEIN_VECTOR_H
