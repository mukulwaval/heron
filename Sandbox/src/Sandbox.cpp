#include <iostream>
#include <string>

#include <Heron.h>

using namespace Heron;

int main() {
restart:
	std::string dataset_path = "datasets";
	int choice;
	std::cout << "\nChoose mode:\n"
		<< "1. Train model & save\n"
		<< "2. Load model & get accuracy\n"
		<< "3. Test single image\n"
		<< "4. Test external 28x28 image\n> ";
	std::cin >> choice;

	Loader loader(dataset_path);
	std::vector<std::vector<float>> X;
	std::vector<uint8_t> Y;

	if (choice == 1) {
		float lr;
		int iters;
		std::cout << "Learning rate: ";
		std::cin >> lr;
		std::cout << "Iterations: ";
		std::cin >> iters;

		if (!loader.load("train")) {
			std::cerr << "Failed to load training dataset\n";
			return 1;
		}

		X.reserve(loader.size());
		Y.reserve(loader.size());
		for (size_t i = 0; i < loader.size(); ++i) {
			X.push_back(loader.get_image(i));
			Y.push_back(loader.get_label(i));
		}

		// Define network: input 28*28, hidden 10, output 10
		std::vector<size_t> layer_sizes = { 28 * 28, 10, 10 };
		std::vector<ActFn> activations = { Activation::relu, Activation::softmax };
		Network net(layer_sizes, activations);

		Trainer::gradient_descent(net, X, Y, lr, iters);

		net.save_model("model.hrnmdl");

		std::cout << "Model saved.\n";
	}
	else if (choice == 2 || choice == 3 || choice == 4) {
		if (choice == 4) {
			// For external image testing, no dataset loading needed
		}
		else if (!loader.load(choice == 2 || choice == 3 ? "test" : "")) {
			std::cerr << "Failed to load dataset\n";
			return 1;
		}
		else {
			X.reserve(loader.size());
			Y.reserve(loader.size());
			for (size_t i = 0; i < loader.size(); ++i) {
				X.push_back(loader.get_image(i));
				Y.push_back(loader.get_label(i));
			}
		}

		// Same network architecture
//		std::vector<size_t> layer_sizes = { 28 * 28, 10, 10 };
//  		std::vector<ActFn> activations = { Activation::relu, Activation::softmax };
		Network net/*(layer_sizes, activations)*/;
		net.load_model("model.hrnmdl");

		if (choice == 2) {
			std::vector<int> preds;
			preds.reserve(X.size());
			std::vector<std::vector<float>> Z, A;

			for (auto& x : X) {
				net.predict(x, Z, A);
				preds.push_back(Trainer::get_prediction(A.back()));
			}

			float acc = Trainer::get_accuracy(preds, Y);
			std::cout << "Accuracy: " << acc << "\n";
		}
		else if (choice == 3) {
			size_t idx;
			std::cout << "Enter image index: ";
			std::cin >> idx;

			std::vector<std::vector<float>> Z, A;
			net.predict(X[idx], Z, A);

			int pred = Trainer::get_prediction(A.back());
			std::cout << "Prediction: " << pred << "\n";

			FileIO::write_28x28_bmp(
				X[idx], "images\\output_" + std::to_string(idx) + ".bmp");
			std::cout << "Image written to images\\output_" << idx << ".bmp\n";
		}
		else if (choice == 4) {
			std::string img_path;
			std::cout << "Enter image path: ";
			std::cin >> img_path;

			std::vector<float> img = FileIO::read_28x28_bmp(img_path);
			FileIO::write_28x28_bmp(img, std::string("images\\") + "loaded_image.bmp");

			std::vector<std::vector<float>> Z, A;
			net.predict(img, Z, A);
			int pred = Trainer::get_prediction(A.back());
			std::cout << "Predicted digit: " << pred << "\n";
		}
	}
	else {
		std::cout << "Invalid option.\n";
	}

	std::cout << "Would you like to continue [Y\\n]: ";
	char a;
	std::cin >> a;
	if (a == 'y' || a == 'Y') goto restart;

	return 0;
}
