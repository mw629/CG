import os

engine_path = r"c:\Users\Masat\source\repos\MyEngine"
resources_dir = os.path.join(engine_path, "Resources")

# Get all resources
all_resources = []
for root, _, files in os.walk(resources_dir):
    for f in files:
        all_resources.append(os.path.join(root, f))

# Get all search files
search_files = []
for root, _, files in os.walk(engine_path):
    for f in files:
        if f.endswith(('.cpp', '.h', '.json', '.vcxproj', '.filters', '.txt', '.md', '.ini')):
            search_files.append(os.path.join(root, f))

file_contents = []
for sf in search_files:
    try:
        with open(sf, 'r', encoding='utf-8') as f:
            file_contents.append(f.read())
    except UnicodeDecodeError:
        try:
            with open(sf, 'r', encoding='shift_jis') as f:
                file_contents.append(f.read())
        except Exception:
            pass

unused_resources = []
for res in all_resources:
    res_name = os.path.basename(res)
    used = False
    for content in file_contents:
        if res_name in content:
            used = True
            break
    if not used:
        unused_resources.append(res)

print(f"Found {len(unused_resources)} unused resources out of {len(all_resources)}")
with open(os.path.join(engine_path, "unused.txt"), "w", encoding="utf-8") as f:
    for u in unused_resources:
        f.write(u + "\n")
