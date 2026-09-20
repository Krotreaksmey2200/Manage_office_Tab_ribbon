#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_DIR"

echo "=== 1. Building application with CMake ==="
/opt/homebrew/bin/cmake --build build

echo "=== 2. Deploying Qt frameworks with macdeployqt ==="
/opt/homebrew/bin/macdeployqt build/OfficeExtensionManager.app -verbose=0

echo "=== 3. Preparing and Signing App Bundle for Launchpad ==="
rm -rf packaging/pkgroot
mkdir -p packaging/pkgroot/Applications
cp -R build/OfficeExtensionManager.app "packaging/pkgroot/Applications/Office Extension & Ribbon Manager.app"
codesign --force --deep -s - "packaging/pkgroot/Applications/Office Extension & Ribbon Manager.app"

echo "=== 4. Building Component PKG ==="
pkgbuild --root packaging/pkgroot \
         --identifier com.khmeroffice.extensionmanager \
         --version 1.0.0 \
         --install-location / \
         packaging/OfficeExtensionManager_component.pkg

echo "=== 5. Building Final Distribution PKG ==="
mkdir -p release
productbuild --distribution packaging/distribution.xml \
             --package-path packaging \
             --resources packaging \
             release/Office-Ribbon-Extension-Manager-v1.0.pkg

cp release/Office-Ribbon-Extension-Manager-v1.0.pkg ./Office-Ribbon-Extension-Manager-v1.0.pkg

echo "=== SUCCESS: Created Office-Ribbon-Extension-Manager-v1.0.pkg ==="
