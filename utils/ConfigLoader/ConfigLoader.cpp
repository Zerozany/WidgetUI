#include "ConfigLoader.h"

#include <spdlog/spdlog.h>

#include <QDir>

#include "toml.hpp"

ConfigLoader::ConfigLoader(const QString& _file, const QString& _dir, const QString& _exePath, QObject* _parent) : QObject{_parent}
{
    std::invoke(&spdlog::set_level, spdlog::level::debug);
    std::invoke(&ConfigLoader::setConfigFilePath, this, _file);
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
            return;
        }
        QDir configDir{_dir};
        if (!configDir.exists())
        {
            if (!configDir.mkpath(_dir))
            {
                spdlog::warn("Directory creation failed:{}", _dir.toStdString());
                return;
            }
        }
        m_configDirPath = _dir;
    } while (false);
    Q_EMIT configDirPathChanged();
}

auto ConfigLoader::getConfigFilePath() const noexcept -> QString
{
    return this->m_configFilePath;
}

auto ConfigLoader::setConfigFilePath(const QString& _file) noexcept -> void
{
    QDir dir{m_configDirPath};
    if (!QFile::exists(dir.filePath(_file)))
    {
        if (QFile configFile{dir.filePath(_file)}; configFile.open(QIODevice::WriteOnly))
        {
            configFile.close();
        }
    }
    m_configFilePath = dir.filePath(_file);
    Q_EMIT this->configFilePathChanged();
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
        m_configDirFilePath = m_configDirPath + R"(/)" + m_configFilePath;
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
