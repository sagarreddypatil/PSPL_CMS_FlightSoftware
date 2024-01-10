import os


def create_lockfile(file_path):
    try:
        with open(file_path, "w") as lockfile:
            lockfile.write("Locked")
        print(f"Lockfile created at: {file_path}")
    except IOError as e:
        print(f"Error creating lockfile: {e}")


def delete_lockfile(file_path):
    try:
        if os.path.exists(file_path):
            os.remove(file_path)
            print(f"Lockfile deleted from: {file_path}")
        else:
            print(f"No lockfile found at: {file_path}")
    except OSError as e:
        print(f"Error deleting lockfile: {e}")


def check_lockfile(file_path):
    return os.path.exists(file_path)
