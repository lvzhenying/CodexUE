# Codex Assistant for UE5

这是一个可直接放入 Unreal Engine 5 项目 `Plugins/` 目录的编辑器插件示例。

## 功能

- 在 UE5 顶部菜单 **Window -> Codex Assistant** 打开助手面板。
- 在面板里输入需求，发送到兼容 OpenAI Responses API 的接口。
- 显示 AI 回复，适合代码草稿、蓝图思路、重构建议等场景。

## 安装

1. 将 `Plugins/CodexAssistant` 复制到你的 UE5 工程目录。
2. 重新生成项目文件并编译。
3. 打开 UE5，启用插件（如未自动启用）。

## 配置

在 `Project Settings -> Plugins -> Codex Assistant` 中配置：

- `API Base URL`：默认 `https://api.openai.com/v1/responses`
- `Model`：默认 `gpt-5.2-codex`
- `API Key`：你的密钥
- `System Prompt`
- `Max Output Tokens`

> API Key 存在 `EditorPerProjectUserSettings` 配置中，建议不要提交到版本库。

## 目录结构

```
Plugins/
  CodexAssistant/
    CodexAssistant.uplugin
    Source/CodexAssistant/
      CodexAssistant.Build.cs
      Public/
        CodexAssistantModule.h
        CodexAssistantSettings.h
      Private/
        CodexAssistantModule.cpp
        SCodexAssistantWindow.h
        SCodexAssistantWindow.cpp
```
