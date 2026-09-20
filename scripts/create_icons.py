import os
import subprocess

src_img = "/Users/heng/.gemini/antigravity-ide/brain/cdfa6658-e35b-4073-85cc-337bbeab4f2b/clean_office_app_icon_1789880272384.jpg"
project_dir = "/Users/heng/Documents/manage_extansion_word"
iconset_dir = os.path.join(project_dir, "assets", "icon.iconset")

os.makedirs(iconset_dir, exist_ok=True)

sizes = [
    (16, "icon_16x16.png"),
    (32, "icon_16x16@2x.png"),
    (32, "icon_32x32.png"),
    (64, "icon_32x32@2x.png"),
    (128, "icon_128x128.png"),
    (256, "icon_128x128@2x.png"),
    (256, "icon_256x256.png"),
    (512, "icon_256x256@2x.png"),
    (512, "icon_512x512.png"),
    (1024, "icon_512x512@2x.png"),
]

for dim, filename in sizes:
    out_path = os.path.join(iconset_dir, filename)
    subprocess.run(["sips", "-s", "format", "png", "-z", str(dim), str(dim), src_img, "--out", out_path], check=True)

# Copy 512x512 as logo.png
logo_path = os.path.join(project_dir, "assets", "logo.png")
pkg_logo_path = os.path.join(project_dir, "packaging", "logo.png")
subprocess.run(["cp", os.path.join(iconset_dir, "icon_512x512.png"), logo_path], check=True)
subprocess.run(["cp", logo_path, pkg_logo_path], check=True)

# Generate icns
icns_path = os.path.join(project_dir, "assets", "AppIcon.icns")
subprocess.run(["iconutil", "-c", "icns", iconset_dir, "-o", icns_path], check=True)

print("Icons generated successfully!")
