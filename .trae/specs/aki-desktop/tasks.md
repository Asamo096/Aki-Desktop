# Aki Desktop - The Implementation Plan (Decomposed and Prioritized Task List)

## [x] Task 1: 项目骨架搭建
- **Priority**: P0
- **Depends On**: None
- **Description**:
  - 初始化 Meson 构建系统
  - 配置 CI 工作流
  - 建立代码规范和目录结构
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `programmatic` TR-1.1: `meson build && ./build/aki` 不崩溃
  - `human-judgment` TR-1.2: 目录结构符合设计规范
- **Notes**: 参考 TODO.md 中的目录结构设计

## [x] Task 2: wlroots 集成
- **Priority**: P0
- **Depends On**: Task 1
- **Description**:
  - 集成 wlroots 0.17 库
  - 实现 Backend、Renderer、Compositor 初始化
  - 显示纯色背景
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `programmatic` TR-2.1: TTY 下运行显示纯色背景
  - `programmatic` TR-2.2: 系统启动无崩溃
- **Notes**: 参考 TODO.md 中的系统架构设计

## [x] Task 3: 日志系统实现
- **Priority**: P0
- **Depends On**: Task 1
- **Description**:
  - 实现结构化日志系统
  - 添加性能追踪点
  - 支持日志级别控制
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `programmatic` TR-3.1: 输出带时间戳的初始化流程
  - `programmatic` TR-3.2: 环境变量 `AKI_LOG=debug` 控制日志级别
- **Notes**: 参考 TODO.md 中的开发工作流

## [x] Task 4: 信号处理实现
- **Priority**: P0
- **Depends On**: Task 1
- **Description**:
  - 实现优雅退出机制
  - 处理系统信号（SIGTERM、SIGINT）
  - 添加崩溃转储功能
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `programmatic` TR-4.1: Ctrl+C 或 SIGTERM 正常清理
  - `programmatic` TR-4.2: 崩溃时生成转储文件
- **Notes**: 确保系统能够安全退出

## [x] Task 5: 输出管理实现
- **Priority**: P1
- **Depends On**: Task 2
- **Description**:
  - 实现显示器检测
  - 支持模式设置和 HiDPI 缩放
  - 处理显示器热插拔
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `programmatic` TR-5.1: 插入显示器自动识别分辨率
  - `programmatic` TR-5.2: 支持 HiDPI 缩放配置
- **Notes**: 参考 TODO.md 中的 Phase 1 目标

## [x] Task 6: 输入处理实现
- **Priority**: P1
- **Depends On**: Task 2
- **Description**:
  - 集成 libinput 库
  - 实现键盘映射和鼠标操作
  - 支持全局热键
- **Acceptance Criteria Addressed**: AC-2
- **Test Requirements**:
  - `programmatic` TR-6.1: `Esc` 退出，`Mod+Enter` 开终端
  - `programmatic` TR-6.2: 输入延迟 < 8ms
- **Notes**: 参考 TODO.md 中的输入延迟测量

## [x] Task 7: 窗口管理实现
- **Priority**: P1
- **Depends On**: Task 5, Task 6
- **Description**:
  - 实现 xdg-shell 协议支持
  - 实现 BSP 平铺布局
  - 支持基础浮动窗口
- **Acceptance Criteria Addressed**: AC-3
- **Test Requirements**:
  - `programmatic` TR-7.1: 能运行 `foot` 或 `alacritty`
  - `human-judgment` TR-7.2: 开多个终端自动排列
- **Notes**: 参考 TODO.md 中的窗口树结构设计

## [x] Task 8: 配置系统实现
- **Priority**: P1
- **Depends On**: Task 1
- **Description**:
  - 实现 Lua 脚本加载
  - 支持窗口规则设置
  - 实现配置热重载
- **Acceptance Criteria Addressed**: AC-5
- **Test Requirements**:
  - `programmatic` TR-8.1: 配置文件设置终端快捷键
  - `programmatic` TR-8.2: `Mod+R` 快捷键重新加载 Lua 配置
- **Notes**: 参考 TODO.md 中的新开发者入门任务

## [x] Task 9: 渲染系统实现
- **Priority**: P1
- **Depends On**: Task 2
- **Description**:
  - 实现 OpenGL ES 3.1 渲染引擎
  - 支持圆角窗口和阴影效果
  - 实现动画系统
- **Acceptance Criteria Addressed**: AC-4
- **Test Requirements**:
  - `human-judgment` TR-9.1: 所有窗口圆角，XWayland 兼容
  - `programmatic` TR-9.2: 120Hz 下 99% 帧 < 8.3ms
- **Notes**: 参考 TODO.md 中的渲染系统设计

## [x] Task 10: 插件系统实现
- **Priority**: P2
- **Depends On**: Task 1, Task 8
- **Description**:
  - 实现多进程隔离的插件宿主
  - 实现共享内存 IPC
  - 支持 Lua API 扩展
- **Acceptance Criteria Addressed**: AC-6
- **Test Requirements**:
  - `programmatic` TR-10.1: 插件崩溃后主进程存活
  - `programmatic` TR-10.2: 插件自动重启
- **Notes**: 参考 TODO.md 中的 ADR-003 设计决策