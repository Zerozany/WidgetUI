_Pragma("once");

#include <QObject>

class ConfigLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString configDirPath READ getConfigDirPath WRITE setConfigDirPath NOTIFY configDirPathChanged)
    Q_PROPERTY(QString configFilePath READ getConfigFilePath WRITE setConfigFilePath NOTIFY configFilePathChanged)
    Q_PROPERTY(QString configDirFilePath READ getConfigDirFilePath WRITE setConfigDirFilePath NOTIFY configDirFilePathChanged)

public:
    explicit(true) ConfigLoader(const QString& _file, const QString& _dir, const QString& _exePath = QString{}, QObject* _parent = nullptr);
    ~ConfigLoader() noexcept = default;

public:
    /// @brief Q_PROPERTY
    auto getConfigDirPath() const noexcept -> QString;
    auto setConfigDirPath(const QString& _dir, const QString& _exePath = QString{}) noexcept -> void;

    auto getConfigFilePath() const noexcept -> QString;
    auto setConfigFilePath(const QString& _file) noexcept -> void;

    auto getConfigDirFilePath() const noexcept -> QString;
    auto setConfigDirFilePath(const QString& _dirFile = QString{}) noexcept -> void;

public:
    auto loadFromFile(const QString& _value, const QString& _key = QString{}) -> QString;

private:
    auto connectSignalToSlot() noexcept -> void;

Q_SIGNALS:
    void configDirPathChanged();

    void configFilePathChanged();

    void configDirFilePathChanged();

private Q_SLOTS:
    // void onConfigDirPathChanged();

private:
    QString m_configFilePath{};
    QString m_configDirPath{};
    QString m_configDirFilePath{};
};
