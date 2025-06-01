#include "ConfigLoader.h"

#include <QDir>

#include "toml.hpp"

ConfigLoader::ConfigLoader()
{
}

auto ConfigLoader::setConfigSystem(const QString& _dir, const QString& _file) -> void
{
    QDir configDir{};
    if (!configDir.exists(_dir))
    {
        if (!configDir.mkpath(_dir))
        {
            return;
        }
    }
    configDir.setPath(_dir);
    if (!QFile::exists(configDir.filePath(_file)))
    {
        if (QFile ConfigFile{configDir.filePath(_file)}; ConfigFile.open(QIODevice::WriteOnly))
        {
            ConfigFile.close();
        }
    }
    m_filePath = configDir.filePath(_file);
}

auto ConfigLoader::setTomlPath(const QString& _filePath) -> void
{
    if (m_filePath == _filePath)
    {
        return;
    }
    m_filePath = _filePath;
}

auto ConfigLoader::loadFromFile(const QString& _value, const QString& _key) -> QString
{
    try
    {
        const auto tomlData{toml::parse(m_filePath.toStdString())};
        if (_key.isEmpty())
        {
            return QString::fromStdString(toml::find<std::string>(tomlData, _value.toStdString()));
        }
        return QString::fromStdString(toml::find<std::string>(toml::find(tomlData, _key.toStdString()), _value.toStdString()));
    }
    catch (const std::exception& e)
    {
        return QString{};
    }
    catch (...)
    {
        return QString{};
    }
}
