import sys

def replace_instance(filename, instance_name):
    processed_instance = instance_name.lower().replace(" ", "_")

    with open(filename, "r", encoding="utf-8") as file:
        content = file.read()

    updated_content = content.replace("{{instance}}", processed_instance)

    print(updated_content)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <filename> <instance_name>")
        sys.exit(1)

    replace_instance(sys.argv[1], sys.argv[2])
