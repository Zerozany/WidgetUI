_Pragma("once");

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "user32.lib")
#endif

#include <QScreen>
#include <QWidget>
#include <QWindow>

namespace Win32Function
{
    inline auto adjustCustomerArea(const MSG* _msg) noexcept -> void;

    inline auto achieveRoundedCorners(const MSG* _msg) noexcept -> HRESULT;

    inline auto coordinateMapping(const POINT& _point, const QWidget* _widget, const QWidget* _titleBar) noexcept -> QPoint;

    inline auto scalingCorrection(const POINT& _point, const QWindow* _handle) noexcept -> QPoint;

}  // namespace Win32Function

inline auto Win32Function::adjustCustomerArea(const MSG* _msg) noexcept -> void
{
    NCCALCSIZE_PARAMS* params{reinterpret_cast<NCCALCSIZE_PARAMS*>(_msg->lParam)};
    // 判断是否最大化
    WINDOWPLACEMENT wp{};
    wp.length = sizeof(WINDOWPLACEMENT);
    ::GetWindowPlacement(_msg->hwnd, &wp);
    const bool isMaximized{(wp.showCmd == SW_SHOWMAXIMIZED)};
    if (isMaximized)
    {
        // 从屏幕边缘缩进几像素
        params->rgrc[0].top += 8;
        params->rgrc[0].left += 8;
        params->rgrc[0].right -= 8;
        params->rgrc[0].bottom -= 8;
    }
}

auto Win32Function::achieveRoundedCorners(const MSG* _msg) noexcept -> HRESULT
{
    constexpr MARGINS margins{1, 1, 1, 1};
    HRESULT           hr{DwmExtendFrameIntoClientArea(_msg->hwnd, &margins)};
    return hr;
}

auto Win32Function::coordinateMapping(const POINT& _point, const QWidget* _widget, const QWidget* _titleBar) noexcept -> QPoint
{
    // 如果 Windows 设置了 150% 缩放，那么 dpr ≈ 1.5
    double dpr{_widget->devicePixelRatioF()};
    // 将物理像素坐标（来自 WinAPI 鼠标事件）转换为 Qt 所需的逻辑像素坐标
    QPoint pos{_titleBar->mapFromGlobal(QPoint(_point.x / dpr, _point.y / dpr))};
    return pos;
}

auto Win32Function::scalingCorrection(const POINT& _point, const QWindow* _handle) noexcept -> QPoint
{
    QPoint globalPos(_point.x, _point.y);
    // DPI 缩放校正
    if (_handle && _handle->screen())
    {
        QPoint offset{_handle->screen()->geometry().topLeft()};
        globalPos = (globalPos - offset) / _handle->screen()->devicePixelRatio() + offset;
    }
    return globalPos;
}
