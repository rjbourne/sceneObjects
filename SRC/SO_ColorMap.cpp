/** \file SO_ColorMap.cpp */
#include "sceneObjects.hpp"

void sceneObjects::SO_ColorMap::setValue(float position, glm::vec3 color) {
    colors[position] = color;
}

void sceneObjects::SO_ColorMap::deleteValue(float position) {
    if (colors.count(position) != 1) { //key exists
        colors.erase(colors.find(position));
    } //otherwise no need to do anything
}

std::vector<float> sceneObjects::SO_ColorMap::getPositions() {
    std::vector<float> positions;
    for (auto it = colors.begin(); it != colors.end(); it++) {
        positions.push_back(it->first);
    }
    return positions;
}

std::vector<glm::vec3> sceneObjects::SO_ColorMap::getColors() {
    std::vector<glm::vec3> returnColors;
    for (auto it = colors.begin(); it != colors.end(); it++) {
        returnColors.push_back(it->second);
    }
    return returnColors;
}

glm::vec3 sceneObjects::SO_ColorMap::getLerpColor(float min, float max, float value) {
    if (colors.size() == 0) { //return black if no color
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    value = (value - min)/(max - min); //normalise values
    if (value <= colors.begin()->first) {
        return colors.begin()->second; // if below min value return lowest value
    }
    for (auto it = std::next(colors.begin()); it != colors.end(); ++it) { // if below max value returns a linear interpolation
        if (value <= it->first) {
            auto prev = std::prev(it);
            return lerp(prev->second, it->second, (value - prev->first)/(it->first - prev->first));
        }
    }
    return std::prev(colors.end())->second; //if above max value return greatest value
}