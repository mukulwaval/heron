#pragma once

#include <vector>
#include <string>
#include <deque>

#include "Sections/ModelConfiguration.h"

namespace HeronGui
{
    enum class ActType
    {
        ReLU,
        Sigmoid,
        Tanh,
        Softmax,
        None
    };

    struct LayerConfig
    {
        int size = 0;
        ActType activation = ActType::ReLU;
    };

    struct ModelConfig
    {
        std::vector<LayerConfig> layers;
        bool dirty = true;          // needs rebuild
    };

    struct TrainingState
    {
        bool running = false;
        bool finished = false;

        float learning_rate = 0.01f;
        int iterations = 100;

        int current_iter = 0;

        std::vector<float> accuracy_history;
    };

    struct ImageState
    {
        int width = 28;
        int height = 28;

        std::vector<float> pixels;     // 0..1
        int predicted = -1;

        bool dirty = true;
    };

    struct PathConfig
    {
        std::string dataset_dir = "datasets";
        std::string model_path = "model.hrnmdl";
        std::string image_dir = "images";
    };

    struct ConsoleState
    {
        std::deque<std::string> lines;
        bool auto_scroll = true;
        bool open = true;

        void log(const std::string& msg)
        {
            lines.push_back(msg);
            if (lines.size() > 500)
                lines.pop_front();
        }
    };

    struct AppState
    {
        ModelConfig   model;
        TrainingState training;
        ImageState    image;
        PathConfig    paths;
        ConsoleState  console;

        Sections::ModelConfiguration
            ::ModelConfigurationState model_config;

        bool exit_requested = false;
    };
}
