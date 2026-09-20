# Manage_office_Tab_ribbon

<p align="center">
  <img src="assets/logo.png" width="128" height="128" alt="Office Extension & Ribbon Manager Logo" />
  <br/>
  <b>Office Extension & Ribbon Manager</b>
  <br/>
  <i>Professional Desktop Studio for Managing Microsoft Office (Word & PowerPoint) Add-ins, Startup Templates, and Custom Ribbon XML with 100% Khmer Unicode Support.</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=c%2B%2B" alt="C++17" />
  <img src="https://img.shields.io/badge/Qt-6.x-41CD52?style=flat-square&logo=qt" alt="Qt6" />
  <img src="https://img.shields.io/badge/Platform-macOS%20%7C%20Windows-lightgrey?style=flat-square" alt="Platform" />
  <img src="https://img.shields.io/badge/Architecture-Apple%20Silicon%20%7C%20Intel-blue?style=flat-square" alt="Architecture" />
  <img src="https://img.shields.io/badge/License-MIT-green?style=flat-square" alt="License" />
</p>

---

## 📖 Overview

**Office Extension & Ribbon Manager** is a high-performance cross-platform desktop application built with modern **C++17** and **Qt 6**. It provides an intuitive, unified graphical interface for managing Microsoft Word and PowerPoint extensions (such as `.dotm`, `.ppam`, `.ppa`, `.wll`, and `Word.officeUI`) as well as creating, customizing, and visually previewing full Microsoft Office Ribbon Bars and XML.

It is specifically engineered to handle complex multilingual scenarios, offering flawless **Khmer Unicode** encoding preservation, macOS administrator privilege escalation for system-level templates, and standalone installer packaging.

---

## ✨ Key Features

### 1. Complete Add-in & Template Management
* **Automated System & User Discovery**:
  * Detects both **User Startup Templates** (`~/Library/Group Containers/UBF8T346G9.Office/...`) and **System Global Templates** (`/Library/Application Support/Microsoft/Office365/...`).
  * Scans Word `.dotm`, `.dotx`, `.wll`, PowerPoint `.ppam`, `.ppa`, and custom Ribbon UI files (`Word.officeUI`).
* **One-Click Enable / Disable**:
  * Safely toggles add-ins by appending or removing `.disabled` suffixes without corrupting the files.
* **Privilege Escalation on macOS**:
  * Seamlessly handles root-owned system templates (`root:wheel`) in `/Library/` using native macOS AppleScript authorization (Touch ID / administrator password prompt).
* **Safe Automated Backups**:
  * Automatically backs up original `.dotm` or `.ppam` files into `~/Documents/OfficeAddinBackups/` before performing any modification or uninstallation.
* **Office Integration**:
  * Quick-launch buttons to open Microsoft Word or Microsoft PowerPoint directly from the application.

### 2. Full Ribbon Bar & CustomUI Editor
* **Live Ribbon Bar Preview**:
  * Interactive horizontal Ribbon preview mirroring Microsoft Office's look and feel.
  * Dynamically inspect and manage tabs, groups, and controls.
* **Visual Hierarchy Tree**:
  * Tree view displaying `Ribbon` ➔ `Tab` ➔ `Group` ➔ `Button / Control`.
  * Add, delete, rename, and reorder tabs, groups, and buttons directly.
* **Property Inspector**:
  * Edit `id`, `label`, `size` (Large / Normal), `imageMso` (built-in Office icons), custom `image`, and VBA/Macro `onAction` callbacks.
* **Real-time XML Syntax Highlighter**:
  * Live code editor with syntax highlighting for Office 2010+ (`customUI14.xml`) and Office 2007 (`customUI.xml`).
  * Bi-directional synchronization: edit via visual tree or directly in code.
* **100% Khmer Unicode Support**:
  * Fully preserves Khmer Unicode strings (e.g., `បើក Mathtype-kh`, `កែប្រែសមីការ`, `ឧបករណ៍ខ្មែរ`) without encoding loss or corrupt character conversions.

---

## 🗂️ Project Structure

```text
manage_extansion_word/
├── CMakeLists.txt                 # CMake configuration for C++17 & Qt6
├── README.md                      # English documentation
├── resources.qrc                  # Qt embedded resource catalog
├── run.sh                         # Development quick-run script
├── assets/
│   ├── AppIcon.icns               # Multi-resolution macOS application icon
│   ├── logo.png                   # Clean 512x512 application emblem
│   └── icon.iconset/              # Icon set (16x16 to 1024x1024 Retina)
├── packaging/
│   ├── distribution.xml           # macOS productbuild distribution descriptor
│   ├── welcome.html               # Installer welcome page
│   └── conclusion.html            # Installer success page
├── scripts/
│   ├── build_pkg.sh               # Automated macOS standalone .pkg builder
│   └── create_icons.py            # Iconset and .icns generator
├── src/
│   ├── main.cpp                   # Application entry point
│   ├── core/
│   │   ├── PlatformService.h      # Abstract OS platform interface
│   │   ├── MacPlatformService.h   # macOS Office directory resolution
│   │   ├── WindowsPlatformService.h # Windows Office directory resolution (%APPDATA%)
│   │   ├── ExtensionItem.h        # Data model for add-ins and startup files
│   │   ├── ExtensionManager.h     # Core logic (scan, enable, disable, install, backup)
│   │   └── ZipHandler.h           # OpenXML ZIP compression and Ribbon XML extraction
│   ├── ribbon/
│   │   ├── RibbonModel.h          # Data structures for Tab, Group, and Controls
│   │   ├── RibbonXmlParser.h      # Standard-compliant CustomUI XML parser
│   │   └── RibbonXmlGenerator.h   # Generates valid Microsoft CustomUI XML
│   └── ui/
│       ├── MainWindow.h           # Main application window
│       ├── WordTabWidget.h        # Word extension management view
│       ├── PowerPointTabWidget.h  # PowerPoint extension management view
│       ├── AllRibbonTabsWidget.h  # Full interactive Ribbon Bar preview & editor
│       ├── RibbonEditorWidget.h   # Visual tree & XML code studio
│       ├── XmlHighlighter.h       # Syntax highlighter for XML
│       └── StyleHelper.h          # Office 365 Light & Dark themes
└── test/
    └── test_ribbon.cpp            # Automated unit tests for core functionality
```

---

## 🛠️ Build & Installation

### Prerequisites

* **C++ Compiler**: Clang (macOS) or MSVC 2019+ (Windows) supporting C++17
* **CMake**: Version 3.16 or higher
* **Qt**: Qt 6.x (Core, Gui, Widgets, Xml)

---

### Building on macOS

1. **Install dependencies via Homebrew**:
   ```bash
   brew install cmake qtbase
   ```

2. **Configure and compile**:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qtbase -B build
   cmake --build build
   ```

3. **Run the application**:
   ```bash
   open build/OfficeExtensionManager.app
   ```

4. **Run Automated Unit Tests**:
   ```bash
   cmake --build build --target test_ribbon
   ./build/test_ribbon
   ```

---

### Building Standalone macOS Installer (`.pkg`)

To create a distributable `.pkg` installer that bundles all Qt frameworks and libraries (no dependencies required on client machines):

```bash
./scripts/build_pkg.sh
```

The installer will be generated at:
`./Office-Ribbon-Extension-Manager-v1.0.pkg`

Double-click the `.pkg` file to install the application into `/Applications/`.

---

### Building on Windows

1. Open **Developer Command Prompt for Visual Studio**.
2. Configure and build:
   ```cmd
   cmake -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
   cmake --build build --config Release
   ```
3. Run the executable:
   ```cmd
   .\build\Release\OfficeExtensionManager.exe
   ```

---

## 🔄 Updating the Repository

To push future changes and updates to GitHub:

```bash
git add .
git commit -m "Describe your updates here"
git push
```

---

## 📄 License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.
