"""
General summary:
	* Build list of <interface, UUID> pairs.
	*
To build list of pairs, use some of the following regexes on separate lines.
"uuid([^)]\))"
"interface ([^ :]) \?\:"
"""

import glob
import os
import re
import sys

def get_all_interfaces(file_or_dir):
	result = {}
	if os.path.isdir(file_or_dir):
		for filename in glob.glob(os.path.join(file_or_dir, "*.idl")):
			get_all_interfaces_for_file(result, filename)
	else:
		get_all_interfaces_for_file(result, file_or_dir)
	return result

def get_all_interfaces_for_file(result, filename):
	file_contents = open(filename, "r").readlines()
	for index, line in enumerate(file_contents):
		uuid_matches = re.findall("uuid\(([^)]*)\)", line)
		if uuid_matches:
			for index2 in range(index + 1, len(file_contents)):
				line_to_check = file_contents[index2]
				if "uuid(" in line_to_check:
					sys.stderr.write("WARNING: Did not match interface in file %s:\n" % filename)
					sys.stderr.write("  %s" % line)
					break

				interface_name_matches = re.findall("interface ([^ :]*) ?\:", line_to_check)
				if interface_name_matches:
					result[interface_name_matches[0]] = uuid_matches[0]
					break

		if not uuid_matches and ("uuid(" in line):
			sys.stderr.write("WARNING: uuid on line, but could not find a match in file %s:\n" % filename)
			sys.stderr.write("  %s" % line)

def main():
	if len(sys.argv) != 3:
		sys.stderr.write("Usage: find_idl_guid_differences.py <file1> <file2>\n")
		sys.exit(1)

	file1_interfaces = get_all_interfaces(sys.argv[1])
	file2_interfaces = get_all_interfaces(sys.argv[2])
	for name, uuid in file1_interfaces.iteritems():
		new_uuid = file2_interfaces.get(name, None)
		if not new_uuid:
			sys.stderr.write("WARNING: %s not present in the new IDL file.\n" % name)
			continue

		if new_uuid.lower() != uuid.lower():
			sys.stderr.write("The UUID for %s has changed.\n" % name)
			sys.stderr.write("   Old UUID: %s\n" % uuid)
			sys.stderr.write("   New UUID: %s\n" % new_uuid)

if __name__ == "__main__":
	main()

