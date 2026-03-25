# Aki Desktop - Product Requirement Document

## Overview
- **Summary**: Aki（秋/空/明）是一个专为 Arch Linux 设计的现代 Wayland 桌面环境，追求极致的输入响应速度（<8ms）与高度可定制的二次元视觉风格。
- **Purpose**: 为 Arch Linux 用户提供一个高性能、美观且高度可定制的 Wayland 桌面环境，满足对视觉效果和系统响应速度有较高要求的用户群体。
- **Target Users**: Arch Linux 用户，特别是对系统性能、视觉效果和定制性有较高要求的用户。

## Goals
- 实现输入延迟 < 8ms 的极致响应速度
- 提供全局/局部毛玻璃、Shader 动态壁纸、三种风格包（科技/治愈/极简）的视觉效果
- 支持 Lua 脚本无边界扩展和 GLSL 运行时热重载
- 深度集成 systemd，通过 AUR 分发，争取进入官方仓库
- 实现混合式窗口管理（BSP平铺 + 基础浮动）

## Non-Goals (Out of Scope)
- 支持非 Arch Linux 发行版
- 支持 X11 显示服务器
- 提供完整的 GUI 设置工具（仅提供基础配置文件）
- 兼容所有 Wayland 协议扩展（仅实现核心功能）

## Background & Context
- Wayland 作为现代显示服务器协议，提供了比 X11 更优的性能和安全性
- 现有 Wayland 合成器如 Sway 等在性能和视觉效果方面仍有提升空间
- Arch Linux 用户群体对系统性能和定制性有较高要求
- 二次元视觉风格在年轻用户中受欢迎

## Functional Requirements
- **FR-1**: 系统启动与基础显示
  - 能够启动并显示背景
  - 支持优雅退出和崩溃处理
- **FR-2**: 输出管理
  - 支持显示器检测、模式设置、HiDPI 缩放
  - 支持多显示器配置
- **FR-3**: 输入处理
  - 集成 libinput，支持键盘映射和鼠标操作
  - 支持全局热键和边缘手势
- **FR-4**: 窗口管理
  - 实现 BSP 平铺布局
  - 支持基础浮动窗口（对话框、右键菜单）
  - 支持混合模式（平铺/浮动切换）
- **FR-5**: 渲染系统
  - 实现 OpenGL ES 3.1 渲染引擎
  - 支持圆角窗口、阴影/发光效果
  - 实现毛玻璃效果和 Shader 动态壁纸
- **FR-6**: 配置系统
  - 支持 Lua 脚本配置
  - 支持窗口规则设置
  - 支持主题包加载
- **FR-7**: 插件系统
  - 实现多进程隔离的插件宿主
  - 支持 Lua API 扩展
  - 支持 GLSL 沙箱

## Non-Functional Requirements
- **NFR-1**: 性能
  - 输入延迟 < 8ms
  - 120Hz+ 高刷优化
  - 120Hz 下 99% 帧 < 8.3ms
- **NFR-2**: 可靠性
  - 插件崩溃不影响主进程
  - 优雅处理系统信号和异常
- **NFR-3**: 可扩展性
  - 支持 Lua 脚本无边界扩展
  - 支持 GLSL 运行时热重载
- **NFR-4**: 兼容性
  - 支持 XWayland 应用
  - 支持标准 Wayland 协议

## Constraints
- **Technical**: 
  - 仅支持 Arch Linux
  - 基于 wlroots 0.17
  - 使用 C++ 20、Lua 5.4、GLSL ES 3.1
- **Dependencies**: 
  - wlroots-0.17
  - wayland-server
  - wayland-protocols
  - libglvnd
  - mesa
  - libinput
  - xkbcommon
  - pixman
  - systemd
  - pam
  - lua-5.4

## Assumptions
- 用户使用 Arch Linux 发行版
- 目标硬件支持 OpenGL ES 3.1
- 用户熟悉基本的 Linux 命令行操作
- 开发环境已安装必要的依赖项

## Acceptance Criteria

### AC-1: 系统启动
- **Given**: 系统已安装 Aki Desktop
- **When**: 执行启动命令
- **Then**: 系统成功启动，显示背景，无崩溃
- **Verification**: `programmatic`

### AC-2: 输入延迟
- **Given**: 系统运行 Aki Desktop
- **When**: 执行输入操作（键盘/鼠标）
- **Then**: 输入延迟 < 8ms
- **Verification**: `programmatic`

### AC-3: 窗口管理
- **Given**: 系统运行 Aki Desktop
- **When**: 打开多个终端
- **Then**: 终端自动按照 BSP 布局排列
- **Verification**: `human-judgment`

### AC-4: 视觉效果
- **Given**: 系统运行 Aki Desktop
- **When**: 启用毛玻璃效果和 Shader 壁纸
- **Then**: 视觉效果符合预期，无卡顿
- **Verification**: `human-judgment`

### AC-5: 配置系统
- **Given**: 系统运行 Aki Desktop
- **When**: 修改 Lua 配置文件
- **Then**: 配置生效，无需重启
- **Verification**: `programmatic`

### AC-6: 插件系统
- **Given**: 系统运行 Aki Desktop
- **When**: 插件崩溃
- **Then**: 主进程继续运行，插件自动重启
- **Verification**: `programmatic`

## Open Questions
- [ ] 如何优化核显性能以确保在低配置硬件上也能流畅运行？
- [ ] 如何实现更高效的 IPC 机制以减少插件通信延迟？
- [ ] 如何确保 XWayland 应用的兼容性和性能？