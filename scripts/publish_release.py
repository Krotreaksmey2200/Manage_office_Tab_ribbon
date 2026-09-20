import os
import sys
import json
import subprocess
import urllib.request
import urllib.parse

def get_git_token():
    proc = subprocess.run(
        ["git", "credential", "fill"],
        input="url=https://github.com/Krotreaksmey2200/Manage_office_Tab_ribbon.git\n",
        text=True,
        capture_output=True,
        check=True
    )
    for line in proc.stdout.splitlines():
        if line.startswith("password="):
            return line.split("=", 1)[1]
    raise RuntimeError("Failed to retrieve git token")

def main():
    token = get_git_token()
    repo = "Krotreaksmey2200/Manage_office_Tab_ribbon"
    tag = "v1.0.0"
    pkg_path = "/Users/heng/Documents/manage_extansion_word/Office-Ribbon-Extension-Manager-v1.0.pkg"

    if not os.path.exists(pkg_path):
        print(f"Error: {pkg_path} does not exist!")
        sys.exit(1)

    print("=== 1. Checking or creating GitHub Release ===")
    create_url = f"https://api.github.com/repos/{repo}/releases"
    headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
        "User-Agent": "OfficeExtensionManager-Release"
    }

    # Check if release already exists
    req = urllib.request.Request(f"https://api.github.com/repos/{repo}/releases/tags/{tag}", headers=headers)
    release_data = None
    try:
        with urllib.request.urlopen(req) as resp:
            release_data = json.loads(resp.read().decode())
            print(f"Release {tag} already exists (ID: {release_data['id']})")
    except urllib.error.HTTPError as e:
        if e.code == 404:
            print(f"Creating new release for tag {tag}...")
        else:
            print(f"HTTP Error: {e.code} - {e.read().decode()}")
            sys.exit(1)

    if not release_data:
        release_payload = {
            "tag_name": tag,
            "target_commitish": "main",
            "name": f"Office Extension & Ribbon Manager {tag}",
            "body": (
                "## Office Extension & Ribbon Manager v1.0.0\n\n"
                "### 📦 Download & Installation\n"
                "Download the standalone macOS installer below:\n"
                "- **`Office-Ribbon-Extension-Manager-v1.0.pkg`**\n\n"
                "Simply double-click the `.pkg` file and follow the on-screen instructions to install to `/Applications`.\n\n"
                "### ✨ Key Features\n"
                "- **Full Ribbon Bar Studio:** Visual live preview and code editor for Office CustomUI XML.\n"
                "- **Word & PowerPoint Add-in Management:** Easily toggle (enable/disable), install, and uninstall `.dotm` / `.ppam` / `.officeUI`.\n"
                "- **100% Khmer Unicode Support:** Full preservation of Khmer text and VBA callbacks.\n"
                "- **macOS System Privilege Support:** Automatically handles root-owned templates in `/Library/`.\n"
                "- **Pre-bundled Dependencies:** Completely standalone, no Qt installation needed."
            ),
            "draft": False,
            "prerelease": False
        }
        req = urllib.request.Request(
            create_url,
            data=json.dumps(release_payload).encode('utf-8'),
            headers={**headers, "Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req) as resp:
            release_data = json.loads(resp.read().decode())
            print(f"Created release: {release_data['html_url']}")

    release_id = release_data["id"]

    # Check existing assets and delete duplicate if present
    print("=== 2. Checking existing assets ===")
    asset_name = os.path.basename(pkg_path)
    for asset in release_data.get("assets", []):
        if asset["name"] == asset_name:
            print(f"Deleting existing asset {asset['name']} (ID: {asset['id']})...")
            del_req = urllib.request.Request(
                f"https://api.github.com/repos/{repo}/releases/assets/{asset['id']}",
                headers=headers,
                method="DELETE"
            )
            with urllib.request.urlopen(del_req) as resp:
                pass

    print(f"=== 3. Uploading {asset_name} ({os.path.getsize(pkg_path)} bytes) to GitHub Release ===")
    upload_url = f"https://uploads.github.com/repos/{repo}/releases/{release_id}/assets?name={urllib.parse.quote(asset_name)}"
    
    with open(pkg_path, "rb") as f:
        pkg_data = f.read()

    upload_headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "Content-Type": "application/octet-stream",
        "User-Agent": "OfficeExtensionManager-Release"
    }

    upload_req = urllib.request.Request(upload_url, data=pkg_data, headers=upload_headers, method="POST")
    with urllib.request.urlopen(upload_req) as resp:
        asset_info = json.loads(resp.read().decode())
        print(f"\n🎉 Successfully uploaded asset!")
        print(f"Download URL: {asset_info.get('browser_download_url')}")
        print(f"Release URL: {release_data.get('html_url')}")

if __name__ == "__main__":
    main()
