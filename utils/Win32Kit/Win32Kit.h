_Pragma("once");

#include <QScreen>
#include <QWidget>
#include <QWindow>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "user32.lib")

class Win32Kit
{
public:
    static auto adjustCustomerArea(const MSG* _msg) noexcept -> void;

    static auto achieveRoundedCorners(const MSG* _msg) noexcept -> HRESULT;

    /// @brief 在高DPI环境下，将物理像素坐标转换为某个控件的局部逻辑坐标
    static auto coordinateMapping(const POINT& _point, const QWidget* _widget, const QWidget* _titleBar) noexcept -> QPoint;

    /// @brief 处理多屏幕环境下的高DPI（High-DPI）坐标转换
    static auto scalingCorrection(const POINT& _point, const QWindow* _handle) noexcept -> QPoint;

    /// @brief 修改窗口样式
    static auto modifyWinStyle(QWidget* _widget) noexcept -> void;
};
#endif
