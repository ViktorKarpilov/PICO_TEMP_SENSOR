import os

def convert_html_to_cpp(html_file, output_file, variable_name):
    # Read HTML
    with open(html_file, 'r', encoding='utf-8') as f:
        html_content = f.read()

    # Escape for C++
    escaped = html_content.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n').replace('\r', '')

    # Generate header
    header = f'''#ifndef {variable_name}_H
        #define {variable_name}_H
        #include <string>
        namespace HtmlResources {{
            const std::string {variable_name} = "{escaped}";
        }}
        #endif
    '''

    # Create output directory and write file
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header)

    print(f"✅ {html_file} → {output_file}")

# Convert your HTML files
convert_html_to_cpp('html/config.html', './generated/config_html.h', 'CONFIG_PAGE')
convert_html_to_cpp('html/status.html', './generated/status_html.h', 'STATUS_PAGE')

print("🎉 All HTML files converted!")