import os

def sanitize(name):
    return name.replace('.', '_').replace('-', '_').replace('/', '_')

def generate_embedded_header(input_dir, output_file):
    header_lines = [
        "#pragma once",
        "",
        "// Auto-generated resource header",
        "#include <stddef.h>",
        "",
        "typedef struct {",
        "    const char* name;",
        "    const unsigned char* data;",
        "    unsigned int length;",
        "} ResourceEntry;",
        ""
    ]

    resource_entries = []

    for subdir, _, files in os.walk(input_dir):
        for file in files:
            rel_path = os.path.relpath(os.path.join(subdir, file), input_dir).replace("\\", "/")
            full_path = os.path.join(subdir, file)
            var_name = sanitize(rel_path)
            
            with open(full_path, 'rb') as f:
                data = f.read()

            byte_array = ', '.join(f'0x{b:02x}' for b in data)
            header_lines.append(f"static const unsigned char {var_name}[] = {{ {byte_array} }};")
            header_lines.append(f"static const unsigned int {var_name}_len = {len(data)};\n")

            resource_entries.append(f'    {{"{rel_path}", {var_name}, {var_name}_len}}')

    header_lines.append("static const ResourceEntry embedded_resources[] = {")
    header_lines.extend(resource_entries)
    header_lines.append("};")
    header_lines.append(f"static const size_t embedded_resource_count = {len(resource_entries)};")

    with open(output_file, 'w') as f:
        f.write('\n'.join(header_lines))

    print(f"Header generated: {output_file}")

if __name__ == "__main__":
    input_dir = "resources"
    output_file = "embedded/embedded_resources.h"
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    generate_embedded_header(input_dir, output_file)
