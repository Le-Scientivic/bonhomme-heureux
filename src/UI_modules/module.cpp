#include "module.hpp"

Module::Module() : est_visible_(true) {}

void Module::afficher() {
    est_visible_ = true;
}

void Module::cacher() {
    est_visible_ = false;
}

bool Module::estVisible() const {
    return est_visible_;
}