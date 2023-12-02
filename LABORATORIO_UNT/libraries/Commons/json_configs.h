#pragma once
#include "json/json.hpp"
#include <iostream>
#include <fstream>
#include <SFML/Window/ContextSettings.hpp>

using json = nlohmann::json;

json readJson(std::string path)
{
    // Abrir el archivo JSON
    std::ifstream archivo(path);

    // Verificar si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo JSON." << std::endl;
        return 0;
    }
    json miJson;
    archivo >> miJson;
    archivo.close();
    return miJson;
}

sf::ContextSettings  set_context_configuration(std::string path)
{
    sf::ContextSettings settings;
    json configuration = readJson(path);
    settings.depthBits = configuration["context_settings"]["depthBits"];
    settings.stencilBits = configuration["context_settings"]["stencilBits"];
    settings.antialiasingLevel = configuration["context_settings"]["antialiasing_level"];
    settings.majorVersion = configuration["context_settings"]["major_version"];
    settings.minorVersion = configuration["context_settings"]["minor_version"];
    return settings;
}