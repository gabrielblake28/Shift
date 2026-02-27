# Shift

A CLI tool that automates your terminal workspace setup. Launch your projects with a single command.

## What It Does

Shift spawns WezTerm windows with pre-configured layouts, tabs, and commands. Define your workspaces once, then jump into development instantly.

## Installation

```bash
make
```

## Usage

```bash
./shift <command>
```

## Configuration

Workspaces are defined in `config/shift_config.json`:

```json
{
  "myproject": {
    "driver": "wezterm",
    "root": "/path/to/project",
    "window": "new",
    "layout": {
      "tabs": [
        { "name": "Editor", "panes": ["nvim ."] },
        { "name": "Server", "panes": ["npm run dev"] }
      ]
    }
  }
}
```

Set a custom config path:
```bash
export SHIFT_CONFIG_PATH=/path/to/config.json
```

## Example

```bash
# Opens workspace with neovim and dev server running
./shift myproject
```

## Requirements

- macOS
- WezTerm
- cJSON

## Building

```bash
make
make clean  # remove build files
```
