# Manage_office_Tab_ribbon

<p align="center">
  <img src="assets/logo.png" width="128" height="128" alt="Office Extension & Ribbon Manager Logo" />
  <br/>
  <b>កម្មវិធីគ្រប់គ្រង Extension និង Ribbon Tab របស់ Microsoft Word & PowerPoint (C++ / Qt)</b>
  <br/>
  <i>Professional Office Ribbon & Add-in Management Studio for macOS & Windows with 100% Khmer Unicode Support</i>
</p>

---

## លក្ខណៈពិសេសចម្បង (Key Features)

### 1. គ្រប់គ្រង Add-ins របស់ Word & PowerPoint
- **ស្កេនដោយស្វ័យប្រវត្តិ (Auto-detection)**៖ រកឃើញ Add-ins ទាំងអស់ក្នុងម៉ាស៊ីន (Startup Templates `.dotm`, PowerPoint Add-ins `.ppam`, និង Custom Ribbon UI `.officeUI`)។
- **បើក/បិទ (Enable / Disable)**៖ បិទ ឬបើកដំណើរការ Add-in ភ្លាមៗដោយសុវត្ថិភាព។
- **ដំឡើង និងលុប (Install & Uninstall)**៖ ដំឡើង Add-in ថ្មីដោយគ្រាន់តែជ្រើសរើស File ឬលុប Add-in ចាស់ដែលលែងត្រូវការ។
- **ការការពារ និង Backup**៖ បង្កើតច្បាប់ចម្លង (Backup) ទុកជាស្វ័យប្រវត្តិនូវរាល់ពេលកែប្រែ ឬលុប Add-in នៅក្នុង `~/Documents/OfficeAddinBackups`។
- **បើកដំណើរការ Office ភ្លាមៗ**៖ មានប៊ូតុងបើកកម្មវិធី Word ឬ PowerPoint ពីក្នុង App ដោយផ្ទាល់។

### 2. ផ្ទាំង Ribbon XML Studio (Visual & Code Editor)
- **រចនាសម្ព័ន្ធបែប Visual Tree**៖ មើល និងរៀបចំ Hierarchy: `Ribbon` ➔ `Tabs` ➔ `Groups` ➔ `Buttons / Separators`។
- **គាំទ្រអក្សរខ្មែរពេញលេញ (Khmer Unicode Support)**៖ អាចកំណត់ Label ខ្មែរ (ឧទាហរណ៍៖ "បើក Mathtype-kh", "កែប្រែសមីការ", "ឧបករណ៍ខ្មែរ") ដោយមិនបារម្ភរឿងខូច Font ឬខូច Encoding។
- **Property Inspector**៖ កែសម្រួល ID, Label, ទំហំ (Large/Normal), Office Icon (`imageMso`), រូបភាពផ្ទាល់ខ្លួន (`image`), និង Macro Callback (`onAction`)។
- **Live XML Syntax Highlighter**៖ បង្ហាញកូដ XML ផ្ទាល់ភ្លាមៗ (Real-time) ជាមួយ Syntax Highlighting ពណ៌ស្រស់ស្អាត។
- **គំរូឧបករណ៍ខ្មែរ (Khmer Template)**៖ មានប៊ូតុងបញ្ចូល Template ឧបករណ៍ខ្មែរ (Khmer Tools Tab) ដោយស្វ័យប្រវត្តិ។
- **Sync & Export**៖ អាចកែប្រែកូដ XML ដោយដៃ រួចចុច Sync ទៅកាន់ Visual Tree វិញ ឬ Export ជាឯកសារ `.xml` / `.officeUI`។

---

## រចនាសម្ព័ន្ធថត និងកូដ (Project Structure)

```text
manage_extansion_word/
├── CMakeLists.txt                 # CMake configuration សម្រាប់ build C++17 និង Qt6
├── README.md                      # ឯកសារណែនាំនេះ
├── src/
│   ├── main.cpp                   # ចំណុចចាប់ផ្ដើមកម្មវិធី (Entry point)
│   ├── core/
│   │   ├── PlatformService.h      # Abstract interface សម្រាប់ OS
│   │   ├── MacPlatformService.h   # កំណត់ Path លើ macOS
│   │   ├── MacPlatformService.cpp
│   │   ├── WindowsPlatformService.h # កំណត់ Path លើ Windows (%APPDATA%)
│   │   ├── WindowsPlatformService.cpp
│   │   ├── ExtensionItem.h        # Data Model របស់ Add-in
│   │   ├── ExtensionManager.h     # Core logic (Scan, Enable, Disable, Install, Backup)
│   │   ├── ExtensionManager.cpp
│   │   ├── ZipHandler.h           # អាន និង Repack .dotm / .ppam OpenXML zip
│   │   └── ZipHandler.cpp
│   ├── ribbon/
│   │   ├── RibbonModel.h          # Model សម្រាប់ Tab, Group, Control
│   │   ├── RibbonModel.cpp
│   │   ├── RibbonXmlParser.h      # Parse CustomUI XML ទៅជា C++ Data Structure
│   │   ├── RibbonXmlParser.cpp
│   │   ├── RibbonXmlGenerator.h   # បង្កើត CustomUI XML មានស្តង់ដារ Microsoft
│   │   └── RibbonXmlGenerator.cpp
│   └── ui/
│       ├── MainWindow.h           # បង្អួចធំនៃកម្មវិធី
│       ├── MainWindow.cpp
│       ├── WordTabWidget.h        # ផ្ទាំងគ្រប់គ្រង Word
│       ├── WordTabWidget.cpp
│       ├── PowerPointTabWidget.h  # ផ្ទាំងគ្រប់គ្រង PowerPoint
│       ├── PowerPointTabWidget.cpp
│       ├── RibbonEditorWidget.h   # Visual Ribbon Studio & Live XML Editor
│       ├── RibbonEditorWidget.cpp
│       ├── XmlHighlighter.h       # Color highlighter សម្រាប់ XML
│       ├── XmlHighlighter.cpp
│       ├── StyleHelper.h          # ឌីហ្សាញបែប Modern Dark / Light Theme
│       └── StyleHelper.cpp
└── test/
    └── test_ribbon.cpp            # ការធ្វើតេស្តស្វ័យប្រវត្តិ (Automated Unit Tests)
```

---

## របៀប Compile និងដំណើរការ (Build & Run)

### លើ macOS:
1. តម្រូវការ៖ CMake (`brew install cmake`) និង Qt6 (`brew install qtbase`)
2. បង្កើត build directory និង compile:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qtbase -B build
   cmake --build build
   ```
3. ដំណើរការកម្មវិធី៖
   ```bash
   # ដំណើរការ App Bundle
   open build/OfficeExtensionManager.app
   # ឬដំណើរការពី terminal
   ./build/OfficeExtensionManager.app/Contents/MacOS/OfficeExtensionManager
   ```
4. ដំណើរការ Automated Unit Tests៖
   ```bash
   cmake --build build --target test_ribbon
   ./build/test_ribbon
   ```

### លើ Windows:
1. តម្រូវការ៖ Visual Studio 2019/2022 (MSVC C++), CMake, និង Qt6/Qt5
2. បើក Command Prompt ឬ PowerShell:
   ```cmd
   cmake -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
   cmake --build build --config Release
   ```
3. ដំណើរការកម្មវិធី៖
   ```cmd
   .\build\Release\OfficeExtensionManager.exe
   ```
