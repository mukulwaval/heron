#pragma once

#include <vector>

namespace Heron
{
	class HERON_API Utils {
	public:
		static std::vector<float> mat_vec(
			const std::vector<std::vector<float>>& mat,
			const std::vector<float>& vec,
			const std::vector<float>& bias
		) {
			size_t rows = mat.size();
			size_t cols = mat[0].size();
			std::vector<float> result(rows, 0.0f);
			for (size_t i = 0; i < rows; ++i) {
				for (size_t j = 0; j < cols; ++j) {
					result[i] += mat[i][j] * vec[j];
				}
				result[i] += bias[i];
			}
			return result;
		}

		static std::vector<float> matT_vec(
			const std::vector<std::vector<float>>& M,
			const std::vector<float>& x
		) {
			std::vector<float> out(M[0].size(), 0.0f);
			for (size_t i = 0; i < M.size(); ++i)
				for (size_t j = 0; j < M[i].size(); ++j)
					out[j] += M[i][j] * x[i];
			return out;
		}

		static void outer_product(
			const std::vector<float>& a, const std::vector<float>& b, std::vector<std::vector<float>>& out
		) {
			out.assign(a.size(), std::vector<float>(b.size()));
			for (size_t i = 0; i < a.size(); ++i)
				for (size_t j = 0; j < b.size(); ++j)
					out[i][j] = a[i] * b[j];
		}

		static std::vector<float> one_hot(int label, int classes) {
			// Returns a list of zeros of length classes except the labelth index is 1
			std::vector<float> oh(classes, 0.0f);
			oh[label] = 1.0f;
			return oh;
		}
	};
} // namespace Heron