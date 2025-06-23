#include "ConfigLoader.h"

#include <spdlog/spdlog.h>

#include <QDir>

#include "toml.hpp"

ConfigLoader::ConfigLoader(const QString& _file, const QString& _dir, const QString& _exePath, QObject* _parent)
    : QObject{_parent}, m_fileName{_file}
{
    std::invoke(&spdlog::set_level, spdlog::level::debug);
    std::invoke(&ConfigLoader::setConfigDirPath, this, _dir, _exePath);
    std::invoke(&ConfigLoader::setConfigDirFilePath, this, QString{});
}

auto ConfigLoader::connectSignalToSlot() noexcept -> void
{
}

auto ConfigLoader::getConfigDirPath() const noexcept -> QString
{
    return this->m_configDirPath;
}

auto ConfigLoader::setConfigDirPath(const QString& _dir, const QString& _exePath) noexcept -> void
{
    do
    {
        if (QDir dir(_exePath); !_exePath.isEmpty())
        {
            dir.setPath(_exePath);
            dir.cdUp();
            m_configDirPath = dir.absoluteFilePath("Config");
            break;
        }
        QDir configDir{_dir};
        if (configDir.exists())
        {
            break;
        }
        if (!configDir.mkpath(_dir))
        {
            spdlog::warn("Directory creation failed:{}", _dir.toStdString());
            break;
        }
        m_configDirPath = _dir;
    } while (false);
    Q_EMIT configDirPathChanged();
}

auto ConfigLoader::getConfigDirFilePath() const noexcept -> QString
{
    return m_configDirFilePath;
}

auto ConfigLoader::setConfigDirFilePath(const QString& _dirFile) noexcept -> void
{
    do
    {
        if (!_dirFile.isEmpty())
        {
            if (m_configDirFilePath != _dirFile)
            {
                m_configDirFilePath = _dirFile;
                break;
            }
        }
        m_configDirFilePath = m_configDirPath + QDir::separator() + m_fileName;
    } while (false);
    Q_EMIT this->configDirFilePathChanged();
}

auto ConfigLoader::loadFromFile(const QString& _value, const QString& _key) -> QString
{
    try
    {
        const auto tomlData{toml::parse(m_configDirFilePath.toStdString())};
        if (_key.isEmpty())
        {
            return QString::fromStdString(toml::find<std::string>(tomlData, _value.toStdString()));
        }
        return QString::fromStdString(toml::find<std::string>(toml::find(tomlData, _key.toStdString()), _value.toStdString()));
    }
    catch (const std::exception& e)
    {
        spdlog::warn("Toml file read Failed:{}", e.what());
        return QString{};
    }
    catch (...)
    {
        spdlog::warn("Toml file read Failed:{}");
        return QString{};
    }
}
