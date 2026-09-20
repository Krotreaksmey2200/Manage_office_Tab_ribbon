#include <QCoreApplication>
#include <QDebug>
#include <cassert>
#include <iostream>
#include "RibbonXmlParser.h"
#include "RibbonXmlGenerator.h"
#include "ZipHandler.h"
#include "PlatformService.h"
#include "ExtensionManager.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "=== Running OfficeExtensionManager Core Tests ===" << std::endl;

    // Test 1: Platform Service detection
    auto platform = IPlatformService::create();
    std::cout << "[Test 1] Platform name: " << platform->getPlatformName().toStdString() << std::endl;
    std::cout << "[Test 1] Word Startup: " << platform->getWordStartupPath().toStdString() << std::endl;
    assert(!platform->getWordStartupPath().isEmpty());
    std::cout << "  -> PASS!" << std::endl;

    // Test 2: Extension Scanning
    ExtensionManager extMgr(platform);
    auto wordExts = extMgr.scanWordExtensions();
    std::cout << "[Test 2] Found " << wordExts.size() << " Word extensions:" << std::endl;
    for (const auto& item : wordExts) {
        std::cout << "  - " << item.fileName.toStdString() 
                  << " (Enabled: " << (item.enabled ? "true" : "false") 
                  << ", CustomRibbon: " << (item.hasCustomRibbonXml ? "true" : "false") << ")" << std::endl;
    }
    assert(wordExts.size() > 0);
    std::cout << "  -> PASS!" << std::endl;

    // Test 3: Parse and Generate Ribbon XML (Khmer Unicode Fidelity)
    QString sampleXml = R"(<customUI xmlns="http://schemas.microsoft.com/office/2009/07/customui">
  <ribbon>
    <tabs>
      <tab id="tabMathTypeKh" label="Mathtype-kh">
        <group id="grpEquations" label="Mathtype-kh">
          <button id="btnOpenMathEditor" 
                  label="បើក Mathtype-kh" 
                  image="logo" 
                  size="large" 
                  onAction="OpenMacTeXMathEditor" 
                  screentip="បើក Mathtype-kh" 
                  supertip="បើកផ្ទាំងកម្មវិធី Mathtype-kh មកប្រើប្រាស់ផ្ទាល់" />
          <button id="btnEditEquation" 
                  label="កែប្រែសមីការ" 
                  image="edit_icon" 
                  size="large" 
                  onAction="OnEditEquationRibbonClick" />
        </group>
      </tab>
    </tabs>
  </ribbon>
</customUI>)";

    RibbonDefinition def;
    QString error;
    bool parsed = RibbonXmlParser::parse(sampleXml, def, error);
    std::cout << "[Test 3] Parsing sample XML: " << (parsed ? "Success" : "Failed") << std::endl;
    if (!parsed) {
        std::cerr << "Error: " << error.toStdString() << std::endl;
    }
    assert(parsed);
    assert(def.tabs.size() == 1);
    assert(def.tabs[0].id == "tabMathTypeKh");
    assert(def.tabs[0].groups.size() == 1);
    assert(def.tabs[0].groups[0].controls.size() == 2);
    assert(def.tabs[0].groups[0].controls[0].label == "បើក Mathtype-kh");
    assert(def.tabs[0].groups[0].controls[0].onAction == "OpenMacTeXMathEditor");
    assert(def.tabs[0].groups[0].controls[1].label == "កែប្រែសមីការ");
    std::cout << "  -> Tab ID: " << def.tabs[0].id.toStdString() << std::endl;
    std::cout << "  -> Button 1 Label (Khmer): " << def.tabs[0].groups[0].controls[0].label.toStdString() << std::endl;
    std::cout << "  -> Button 2 Label (Khmer): " << def.tabs[0].groups[0].controls[1].label.toStdString() << std::endl;

    // Test 4: Re-generate XML
    QString generatedXml = RibbonXmlGenerator::generateXml(def);
    assert(generatedXml.contains("បើក Mathtype-kh"));
    assert(generatedXml.contains("OpenMacTeXMathEditor"));
    assert(generatedXml.contains("កែប្រែសមីការ"));
    std::cout << "[Test 4] Re-generated XML contains all Khmer unicode text and callbacks." << std::endl;
    std::cout << "  -> PASS!" << std::endl;

    std::cout << "\nALL 4 TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
